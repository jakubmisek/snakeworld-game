//
//  EAGLView.m
//  Snakeworld
//



#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>

#import "EAGLView.h"
#import "MainObj.h"
#import "LoadTexture.h"
#import <math.h>
#import "Touches.h"


#define USE_DEPTH_BUFFER 0

#define kFilteringFactor			0.1
#define kAccelerometerFrequency		10.0 // Hz
#define kAnimationInterval  1.0 / 27.0
UIAccelerationValue		accel[3];

// A class extension to declare private methods
@interface EAGLView ()

EAGLView* view;

@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, assign) NSTimer *animationTimer;

- (BOOL) createFramebuffer;
- (void) destroyFramebuffer;

@end


@implementation EAGLView

@synthesize context;
@synthesize animationTimer;
@synthesize animationInterval;

MainObj* _mainObj;

// You must implement this
+ (Class)layerClass
{
	return [CAEAGLLayer class];
}

- (void)AppTerminating
{
	_mainObj->Shutdown();
	_mainObj->TearDown();
}

- (void)AppResigning
{
	_mainObj->Freeze();
	_mainObj->PauseGame();
	[self setAnimationInterval:(1.0 / 3.0)];
}

- (void)MemoryWarning
{
	_mainObj->LowMemory();
}

- (void)AppActive
{
	_mainObj->Thaw();
	[self setAnimationInterval:kAnimationInterval];
}

//The GL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id)initWithCoder:(NSCoder*)coder
{
	[[UIApplication sharedApplication] setStatusBarHidden:YES animated:NO];
	[[UIApplication sharedApplication] setStatusBarOrientation: UIInterfaceOrientationLandscapeRight animated:NO];

	if ((self = [super initWithCoder:coder])) {
		// Get the layer
		CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
		
		eaglLayer.opaque = YES;
		eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
		   [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
		
		context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
		
		if (!context || ![EAGLContext setCurrentContext:context]) {
			[self release];
			return nil;
		}
		[self setAnimationInterval:kAnimationInterval];
	}
	_mainObj = new MainObj();

	[[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0 / kAccelerometerFrequency)];
	[[UIAccelerometer sharedAccelerometer] setDelegate:self];
	[[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0 / kAccelerometerFrequency)];
	[[UIAccelerometer sharedAccelerometer] setDelegate:self];
	
	[self setMultipleTouchEnabled:YES];

//	TDStartSoundEngine();

	return self;
}

extern bool AlertVisible;
extern int AlertResult;
extern int AlertBtn1;
extern int AlertBtn2;
extern int AlertBtn3;

- (void)drawView
{
	if (AlertVisible) return;
	
	[EAGLContext setCurrentContext:context];
	
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);

	if (!AlertVisible)
	{
		if (accel[0] < -.5)
			_mainObj->SetOrientation(-1);
		else if (accel[0] > .5)
			_mainObj->SetOrientation(1);
		else
			_mainObj->SetOrientation(0);
		_mainObj->Render();
	}
	
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
	[context presentRenderbuffer:GL_RENDERBUFFER_OES];
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
	if (buttonIndex == 0) AlertResult = AlertBtn1;
	if (buttonIndex == 1) AlertResult = AlertBtn2;
	if (buttonIndex == 2) AlertResult = AlertBtn3;
	AlertVisible = false;
}


- (void)layoutSubviews {
	[EAGLContext setCurrentContext:context];
	[self destroyFramebuffer];
	[self createFramebuffer];
	[self drawView];
}


- (BOOL)createFramebuffer
{
	view = self;	
	glGenFramebuffersOES(1, &viewFramebuffer);
	glGenRenderbuffersOES(1, &viewRenderbuffer);
	
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
	[context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer*)self.layer];
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderbuffer);
	
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
	
	if (USE_DEPTH_BUFFER) {
		glGenRenderbuffersOES(1, &depthRenderbuffer);
		glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
		glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, backingWidth, backingHeight);
		glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);
	}

	if(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
		NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
		return NO;
	}

	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
	[context presentRenderbuffer:GL_RENDERBUFFER_OES];
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
	[context presentRenderbuffer:GL_RENDERBUFFER_OES];
	
	_mainObj->Setup(480,320);
	
	_mainObj->Restore();
	
	return YES;
}


- (void)destroyFramebuffer
{
	
	glDeleteFramebuffersOES(1, &viewFramebuffer);
	viewFramebuffer = 0;
	glDeleteRenderbuffersOES(1, &viewRenderbuffer);
	viewRenderbuffer = 0;
	
	if(depthRenderbuffer) {
		glDeleteRenderbuffersOES(1, &depthRenderbuffer);
		depthRenderbuffer = 0;
	}
}


- (void)startAnimation
{
	self.animationTimer = [NSTimer scheduledTimerWithTimeInterval:animationInterval target:self selector:@selector(drawView) userInfo:nil repeats:YES];
}


- (void)stopAnimation
{
	self.animationTimer = nil;
}


- (void)setAnimationTimer:(NSTimer *)newTimer
{
	[animationTimer invalidate];
	animationTimer = newTimer;
}


- (void)setAnimationInterval:(NSTimeInterval)interval
{
	
	animationInterval = interval;
	if (animationTimer) {
		[self stopAnimation];
		[self startAnimation];
	}
}

- (void)dealloc
{
	delete _mainObj;
//	TDStopSoundEngine();
	[self stopAnimation];

	if ([EAGLContext currentContext] == context) {
		[EAGLContext setCurrentContext:nil];
	}
	
	[context release];	
	[super dealloc];
}

- (void)accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration
{
	//Use a basic low-pass filter to only keep the gravity in the accelerometer values
	accel[0] = acceleration.x * kFilteringFactor + accel[0] * (1.0 - kFilteringFactor);
	accel[1] = acceleration.y * kFilteringFactor + accel[1] * (1.0 - kFilteringFactor);
	accel[2] = acceleration.z * kFilteringFactor + accel[2] * (1.0 - kFilteringFactor);
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event 
{
	for (UITouch* atouch in [event allTouches])
	{
		if (atouch.phase == UITouchPhaseBegan)
		{
			CGPoint touchPoint = [atouch locationInView:self];
			int finger = theTouches.TouchStart(touchPoint.x, 480 - touchPoint.y);
			if (finger != -1)
				_mainObj->TouchBegan(finger);
		}
	}
}


- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	for (UITouch* atouch in [event allTouches])
	{
		if (atouch.phase == UITouchPhaseMoved)
		{
			CGPoint touchPoint = [atouch locationInView:self];
			int finger = theTouches.TouchMove(touchPoint.x, 480 - touchPoint.y);
			if (finger != -1)
				_mainObj->TouchDragging(finger);
		}
	}
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	for (UITouch* atouch in [event allTouches])
	{
		if (atouch.phase == UITouchPhaseEnded)
		{
			CGPoint touchPoint = [atouch locationInView:self];
			int finger = theTouches.TouchEnd(touchPoint.x, 480 - touchPoint.y);
			if (finger != -1)
				_mainObj->TouchEnded(finger);
		}
	}
}


- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event 
{
	for (UITouch* atouch in [event allTouches])
	{
		if (atouch.phase == UITouchPhaseCancelled)
		{
			CGPoint touchPoint = [atouch locationInView:self];
			int finger = theTouches.TouchEnd(touchPoint.x, 480 - touchPoint.y);
			if (finger != -1)
				_mainObj->TouchEnded(finger);
		}
	}
}

/*
 - (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event 
{
	int counter = 0;
	NSSet* ns = [event allTouches];
	for (UITouch* atouch in ns)
	{
		CGPoint touchPoint = [atouch locationInView:self];
		touch_began[counter].x = touchPoint.x;
		touch_began[counter].y = 480 - touchPoint.y;
		touch_began[counter].ticks = GetTickCount();
		touch[counter].x = touchPoint.x;
		touch[counter].y = 480 - touchPoint.y;
		touch[counter].ticks = GetTickCount();
		touch_type[counter] = TOUCHTYPE_CLICK;
		counter++;
		if (counter > 2) counter = 2;
	}
	theMainObj->TouchBegan();
}


- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	int counter = 0;
	for (UITouch* atouch in [event allTouches])
	{
		if (touch_type[counter] != TOUCHTYPE_ENDED)
		{
			CGPoint touchPoint = [atouch locationInView:self];
			touch[counter].x = touchPoint.x;
			touch[counter].y = 480 - touchPoint.y;
			touch[counter].ticks = GetTickCount();
			if (touch_type[counter] != TOUCHTYPE_DRAG)
			{
				if (abs(touch_began[counter].x - touch[counter].x) > 8 ||
					abs(touch_began[counter].y - touch[counter].y) > 8 )
				{
					touch_type[counter] = TOUCHTYPE_DRAG;
				}
			}
		}
		counter++;
		if (counter > 2) counter = 2;
	}
	theMainObj->TouchDragging();
}

float absf(float val)
{
	if (val < 0.0f) return 0.0f - val;
	return val;
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	int counter = 0;
	for (UITouch* atouch in [event allTouches])
	{
		if (touch_type[counter] != TOUCHTYPE_ENDED)
		{
			CGPoint touchPoint = [atouch locationInView:self];
			uint tc = GetTickCount();
			float x1 = absf((float)(touch_ended[counter].x) - touchPoint.x);
			float y1 = absf((float)(touch_ended[counter].y) - (480 - touchPoint.y));
			if ((touch_ended[counter].ticks > tc - 500) &&
				x1 < 8 &&
				y1 < 8 && touch_type[counter] == TOUCHTYPE_CLICK)
				touch_type[counter] = TOUCHTYPE_DOUBLECLICK;
			touch_ended[counter].x = touchPoint.x;
			touch_ended[counter].y = 480 - touchPoint.y;
			touch_ended[counter].ticks = tc;
		}
		counter++;
		if (counter > 2) counter = 2;
	}
	theMainObj->TouchEnded();
}


- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event 
{
	int counter = 0;
	for (UITouch* atouch in [event allTouches])
	{
		if (touch_type[counter] != TOUCHTYPE_ENDED)
		{
			CGPoint touchPoint = [atouch locationInView:self];
			uint tc = GetTickCount();
			float x1 = absf((float)(touch_ended[counter].x) - touchPoint.x);
			float y1 = absf((float)(touch_ended[counter].y) - (480 - touchPoint.y));
			if ((touch_ended[counter].ticks > tc - 500) &&
				x1 < 8 &&
				y1 < 8 && touch_type[counter] == TOUCHTYPE_CLICK)
				touch_type[counter] = TOUCHTYPE_DOUBLECLICK;
			touch_ended[counter].x = touchPoint.x;
			touch_ended[counter].y = 480 - touchPoint.y;
			touch_ended[counter].ticks = tc;
		}
		counter++;
		if (counter > 2) counter = 2;
	}
	theMainObj->TouchEnded();
}
*/
@end
