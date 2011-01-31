/*
 *  jx_CGImageUtils.c
 *  CreateRotateWriteCFImage
 *
 *  Created by Jan on 31.01.11.
 *  Copyright 2011 geheimwerk.de. All rights reserved.
 *
 */

#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>

#include "jx_CGImageUtils.h"

#ifndef MAX
#define MAX(a,b) ({__typeof__(a) _a = (a); __typeof__(b) _b = (b); (_a > _b) ? _a : _b; })
#endif

static inline CGFloat jx_radians(CGFloat degrees) {
	return degrees * M_PI/180;
}

CGImageRef jx_CGImageCreateRotatedClockwiseByAngle(CGImageRef imgRef, CGFloat angle) {
	CGFloat angleInRadians = jx_radians(-angle);
	CGFloat width = CGImageGetWidth(imgRef);
	CGFloat height = CGImageGetHeight(imgRef);
	
	// Prepare CGRect of correct size to contain rotated CGImage
	CGRect imgRect = CGRectMake(0, 0, width, height);
	CGAffineTransform transform = CGAffineTransformMakeRotation(angleInRadians);
	CGRect rotatedRect = CGRectApplyAffineTransform(imgRect, transform);
	
	// CGContext setup
	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
	CGContextRef context = CGBitmapContextCreate(NULL,
												 rotatedRect.size.width,
												 rotatedRect.size.height,
												 CGImageGetBitsPerComponent(imgRef),
												 0,
												 colorSpace,
												 kCGImageAlphaPremultipliedFirst);
#ifndef JX_CGIMAGE_HIGH_QUALITY_TRANSFORMS
	CGContextSetAllowsAntialiasing(context, FALSE);
	CGContextSetInterpolationQuality(context, kCGInterpolationNone);
#else
	CGContextSetAllowsAntialiasing(context, TRUE);
	CGContextSetInterpolationQuality(context, kCGInterpolationHigh);
#endif
	CGColorSpaceRelease(colorSpace);
	
	// Rotate the CGContext around its center
	CGContextTranslateCTM(context,
						  +(rotatedRect.size.width/2),
						  +(rotatedRect.size.height/2));
	CGContextRotateCTM(context, angleInRadians);
	
	// Draw the image into the rotated context
	CGContextDrawImage(context, CGRectMake(-imgRect.size.width/2, 
										   -imgRect.size.height/2,
										   imgRect.size.width, 
										   imgRect.size.height),
					   imgRef);
	
	CGImageRef rotatedImage = CGBitmapContextCreateImage(context);
	//CFMakeCollectable(rotatedImage);
	
	CFRelease(context);
	
	return rotatedImage;
}

void jx_CGImageExportToURL(CGImageRef image, CFURLRef url) {
	// CHANGEME: Could use some error handling
 
	CFStringRef type = kUTTypePNG;  //public.png
	size_t count = 1; 
	CFDictionaryRef options = NULL;
	CGImageDestinationRef dest = CGImageDestinationCreateWithURL(url, type, count, options);
	
	CGImageDestinationAddImage(dest, image, NULL);
	
	CGImageDestinationFinalize(dest);
	
	CFRelease(dest);
}

CGImageRef jx_CFImageCreateFromRGBBytesInCFDataRef(CFDataRef rgbData, size_t width, size_t height, size_t components) {
	CGDataProviderRef provider = CGDataProviderCreateWithCFData(rgbData);
	
	CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();
	
	CGImageRef rgbImageRef = CGImageCreate(width, 
										   height, 
										   sizeof(UInt8) * 8, 
										   sizeof(UInt8) * 8 * components, 
										   width * components, 
										   colorspace, 
										   kCGImageAlphaNone, 
										   provider, 
										   NULL, 
										   true, 
										   kCGRenderingIntentDefault);
	//CFMakeCollectable(rgbImageRef);
	
	CGColorSpaceRelease(colorspace);

	CGDataProviderRelease(provider);
	
	return rgbImageRef;
}

CGImageRef jx_CFImageCreateFromRGBByteArray(UInt8 *pixelData, size_t width, size_t height, size_t components) {
	CFDataRef rgbData = CFDataCreate(NULL, pixelData, width * height * sizeof(UInt8) * components);
	CGImageRef rgbImageRef = jx_CFImageCreateFromRGBBytesInCFDataRef(rgbData, width, height, components);
	CFRelease(rgbData);
	
	return rgbImageRef;
}
