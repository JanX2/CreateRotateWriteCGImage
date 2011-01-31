/*
 *  jx_CGImageUtils.h
 *  CreateRotateWriteCFImage
 *
 *  Created by Jan on 31.01.11.
 *  Copyright 2011 geheimwerk.de. All rights reserved.
 *
 */

#define JX_CGIMAGE_HIGH_QUALITY_TRANSFORMS

CGImageRef jx_CGImageCreateRotatedClockwiseByAngle(CGImageRef imgRef, CGFloat angle);
void jx_CGImageExportToURL(CGImageRef image, CFURLRef url);
CGImageRef jx_CFImageCreateFromRGBBytesInCFDataRef(CFDataRef rgbData, size_t width, size_t height, size_t components);
CGImageRef jx_CFImageCreateFromRGBByteArray(UInt8 *pixelData, size_t width, size_t height, size_t components);