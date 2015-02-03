/*
 *  LoadTexture.m
 *  Snakeworld
 *
 */

#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <mach/mach_time.h>
#include <sys/time.h>
#include <string>
#include "EAGLView.h"

//#import "LoadTexture.h"

bool AlertVisible = false;
int AlertResult = 0;
int AlertBtn1 = 0;
int AlertBtn2 = 0;
int AlertBtn3 = 0;

extern EAGLView* view;

int mac_last_ticks = 0;
int mac_last_seconds = 0;
int mac_current_ticks = 0;

void TDAlert(std::string title, std::string msg, std::string btn1, int btn1_val, std::string btn2, int btn2_val, std::string btn3, int btn3_val)
{
	AlertVisible = true;
	AlertResult = 0;
	AlertBtn1 = btn1_val;
	AlertBtn2 = btn2_val;
	AlertBtn3 = btn3_val;

	NSString* ns_title = [NSString stringWithCString:title.data()];
	NSString* ns_msg = [NSString stringWithCString:msg.data()];
	NSString* ns_btn1 = [NSString stringWithCString:btn1.data()];
	NSString* ns_btn2 = [NSString stringWithCString:btn2.data()];
	NSString* ns_btn3 = [NSString stringWithCString:btn3.data()];
	
	UIAlertView *alert;
	if ([ns_btn3 length] != 0)
	{
		alert = [[UIAlertView alloc] initWithTitle:ns_title message:ns_msg
				  delegate:view cancelButtonTitle:ns_btn1 otherButtonTitles:ns_btn2,  ns_btn3, nil];
	}
	else
	{
		if ([ns_btn2 length] != 0)
		{
			alert = [[UIAlertView alloc] initWithTitle:ns_title message:ns_msg
										  delegate:view cancelButtonTitle:ns_btn1 otherButtonTitles:ns_btn2, nil];
		}
		else
		{
			alert = [[UIAlertView alloc] initWithTitle:ns_title message:ns_msg
										  delegate:view cancelButtonTitle:ns_btn1 otherButtonTitles:nil];
		}
	}
	[alert show];
	[alert release];
	//	[ns_msg release];
	//	[ns_btn1 release];
	//	[ns_btn2 release];
}

int TDAlertResult()
{
	int res = AlertResult;
	AlertResult = 0;
	return res;
}

std::string TDSubmitURL(std::string url)
{
	NSString* ns = [NSString stringWithCString:url.data()];
	NSString* fixedURL = (NSString *) CFURLCreateStringByAddingPercentEscapes(NULL, (CFStringRef)ns, NULL, NULL, kCFStringEncodingUTF8);
	NSURL* nsurl = [NSURL  URLWithString:fixedURL];
	NSString *content = [NSString stringWithContentsOfURL:nsurl encoding:NSASCIIStringEncoding error:NULL];
//	[ns release];
//	[fixedURL release];
//	[nsurl release];
	
	if (content == nil)
		return std::string("FAIL");
	std::string ret = [content cStringUsingEncoding:NSASCIIStringEncoding];
//	[content release];
	return ret;
}

std::string TDiPhoneUniqueID()
{
	NSString* sid = [[UIDevice currentDevice] uniqueIdentifier];
	return [sid cStringUsingEncoding:NSASCIIStringEncoding];
}

uint GetTickCount()
{
//	CFTimeInterval		time;
////	float				dTime;
//	time = CFAbsoluteTimeGetCurrent();
//	UnsignedWide uw_ticks;
//	Microseconds(&uw_ticks);
//	return time;
//	return uw_ticks.lo;

	timeval time;	
	gettimeofday(&time, NULL);

	if (mac_last_ticks == 0)
	{
		mac_last_seconds = time.tv_sec;
		mac_last_ticks = time.tv_usec / 1000;
	}
	
	mac_current_ticks += (time.tv_sec - mac_last_seconds) * 1000;
	mac_current_ticks += (((int)time.tv_usec / 1000) - mac_last_ticks);
	
	mac_last_seconds = time.tv_sec;
	mac_last_ticks = time.tv_usec / 1000;
	
	return mac_current_ticks;
}

const char* PathForFile(const char* fname)
{
	NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString* documentsDirectory = [paths objectAtIndex:0];
	NSString* ns_fname = [NSString stringWithCString:fname];
	NSString* writableDBPath = [documentsDirectory stringByAppendingPathComponent:ns_fname];
//	[ns_fname release];
	return [writableDBPath cStringUsingEncoding:NSASCIIStringEncoding];
	
//	NSString* fname_ns = [NSString stringWithCString:fname];
//	NSString* ext_ns = [NSString stringWithCString:ext];
//	NSString* ret = [[NSBundle mainBundle] pathForResource:fname_ns ofType:ext_ns];
//	return [ret cStringUsingEncoding:NSASCIIStringEncoding];
}

const char* BundlePathName(const char* fname, const char* ext)
{
	NSBundle*				bundle = [NSBundle mainBundle];
	NSString* ns = [NSString stringWithCString:fname];
	NSString* nsext = [NSString stringWithCString:ext];
	
	const char* str = [[bundle pathForResource:ns ofType:nsext ] UTF8String];
	return str;
}


bool LoadTexture(unsigned char** buffer, unsigned int* size, unsigned int* width, unsigned int* height, const char* filename)
{
/*	unsigned char** buffer;
	unsigned int* size;
	unsigned int* width;
	unsigned int* height;
	const char* filename;
*/
	
	CGImageRef spriteImage;
	CGContextRef spriteContext;
	GLubyte *spriteData;
	// Creates a Core Graphics image from an image file
	NSString* ns = [NSString stringWithCString:filename];
	spriteImage = [UIImage imageNamed:ns].CGImage;
//	[ns release];
	// Get the width and height of the image
	*width = CGImageGetWidth(spriteImage);
	*height = CGImageGetHeight(spriteImage);
	// Texture dimensions must be a power of 2. If you write an application that allows users to supply an image,
	// you'll want to add code that checks the dimensions and takes appropriate action if they are not a power of 2.
	
	if(spriteImage) 
	{
		// Allocated memory needed for the bitmap context
		*size = *width * *height * 4;
		spriteData = (GLubyte *) malloc(*size);
		// Uses the bitmatp creation function provided by the Core Graphics framework. 
		spriteContext = CGBitmapContextCreate(spriteData, *width, *height, 8, *width * 4, CGImageGetColorSpace(spriteImage), kCGImageAlphaPremultipliedLast);
		// After you create the context, you can draw the sprite image to the context.
		CGContextDrawImage(spriteContext, CGRectMake(0.0, 0.0, (CGFloat)(*width), (CGFloat)(*height)), spriteImage);
		// You don't need the context at this point, so you need to release it to avoid memory leaks.
		CGContextRelease(spriteContext);
		*buffer = spriteData;
		return true;
	}
	return false;
}

std::string Format(const char* szFormat, ...)
{
	char szBuff[1024];
	va_list arg;
	va_start(arg, szFormat);
	vsnprintf(szBuff, sizeof(szBuff), szFormat, arg);
	va_end(arg);
	return std::string(szBuff);
}

void TDGotoWebSite(std::string site)
{
}