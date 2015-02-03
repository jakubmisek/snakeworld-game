//
//  SnakeworldAppDelegate.m
//  Snakeworld
//

#import "SnakeworldAppDelegate.h"
#import "EAGLView.h"

@implementation TowerAppDelegate

@synthesize window;
@synthesize glView;

- (void)applicationDidFinishLaunching:(UIApplication *)application {

	glView.animationInterval = 1.0 / 60.0;
	[glView startAnimation];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	[glView AppTerminating];
}

- (void)applicationWillResignActive:(UIApplication *)application
{
	[glView AppResigning];
}

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application
{
	[glView MemoryWarning];
}


- (void)applicationDidBecomeActive:(UIApplication *)application
{
	[glView AppActive];
}

- (void)dealloc {
	[window release];
	[glView release];
	[super dealloc];
}

@end
