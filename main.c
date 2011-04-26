#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>
#include <unistd.h>

#include "jx_CGImageUtils.h"

#define WIDTH	64
#define HEIGHT	128

#define COMPONENTS	3


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
	
	CGImageRef rgbImageRef = jx_CGImageCreateFromRGBByteArray(pixelData, WIDTH, HEIGHT, COMPONENTS);
	
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
