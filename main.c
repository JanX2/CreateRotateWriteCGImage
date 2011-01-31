#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>
#include <unistd.h>


#ifndef MAX
#define MAX(a,b) ({__typeof__(a) _a = (a); __typeof__(b) _b = (b); (_a > _b) ? _a : _b; })
#endif


#define WIDTH	64
#define HEIGHT	128

#define COMPONENTS	3

#define JX_CGIMAGE_HIGH_QUALITY_TRANSFORMS

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
	CFStringRef type = kUTTypePNG;  //public.png
	size_t count = 1; 
	CFDictionaryRef options = NULL;
	CGImageDestinationRef dest = CGImageDestinationCreateWithURL(url, type, count, options);
	
	CGImageDestinationAddImage(dest, image, NULL);
	
	CGImageDestinationFinalize(dest);
	
	CFRelease(dest);
}


int main (int argc, const char * argv[]) {
	// There probably is a better way…
	char work_path[PATH_MAX];
	size_t size = PATH_MAX;
	getcwd(work_path, size);
	CFStringRef workPath = CFStringCreateWithCString(kCFAllocatorDefault, work_path, kCFStringEncodingUTF8);
	CFURLRef workDirURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, workPath, kCFURLPOSIXPathStyle, true);
	CFRelease(workPath);
	
	CFURLRef url = CFURLCreateWithFileSystemPathRelativeToBase(kCFAllocatorDefault, CFSTR("ExportPNGTest.png"), kCFURLPOSIXPathStyle, false, workDirURL);
	
	// Raw pixel data memory of WIDTH * HEIGHT pixel size
	UInt8 pixelData[WIDTH * HEIGHT * COMPONENTS];
	
#if 0
	// Fill the raw pixel buffer with orange (R: 100%, G: 50%, B: 0%) for this test
	for (size_t ui = 0; ui < WIDTH * HEIGHT * COMPONENTS; ui++) pixelData[ui] = (UInt8)(0.5f * ((COMPONENTS-1) - (ui%COMPONENTS)) * 255);
#else
	// Fill the raw pixel buffer with an orange-to-black gradient with orange starting in the lower left corner 
	size_t ui = 0;
	float pixel_value = 0;
	for (size_t y = 0; y < HEIGHT; y++) {
		for (size_t x = 0; x < WIDTH; x++) {
			ui = (y * WIDTH + x) * COMPONENTS;
			for (size_t c = 0; c < COMPONENTS; c++) {
				pixel_value = (0.5f * ((COMPONENTS-1) - c) * (float)(WIDTH - x) / WIDTH  * (float)(HEIGHT - y) / HEIGHT) * 255;
				pixelData[ui+c] = (UInt8)pixel_value;
			}
		}
	}
#endif
	
	CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();
	
	CFDataRef rgbData = CFDataCreate(NULL, pixelData, WIDTH * HEIGHT * COMPONENTS);
	CGDataProviderRef provider = CGDataProviderCreateWithCFData(rgbData);
	
	CGImageRef rgbImageRef = CGImageCreate(WIDTH, 
										   HEIGHT, 
										   sizeof(UInt8)*8, 
										   sizeof(UInt8)*8*COMPONENTS, 
										   WIDTH * COMPONENTS, 
										   colorspace, 
										   kCGImageAlphaNone, 
										   provider, 
										   NULL, 
										   true, 
										   kCGRenderingIntentDefault);
	
	CFRelease(rgbData);
	CGDataProviderRelease(provider);
	
	CGColorSpaceRelease(colorspace);
	
	// Rotate left by 90°
	CGImageRef rotatedRGBImageRef = jx_CGImageCreateRotatedClockwiseByAngle(rgbImageRef, -90.0f);
	CGImageRelease(rgbImageRef);
	rgbImageRef = rotatedRGBImageRef;
	
	// Write the CGImage to a PNG file
	jx_CGImageExportToURL(rgbImageRef, url);
	
	
	CGImageRelease(rgbImageRef);
	
	CFRelease(workDirURL);
	CFRelease(url);
	
    return 0;
}
