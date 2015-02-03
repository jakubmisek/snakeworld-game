//
//  SnakeworldAppDelegate.h
//  Snakeworld
//

#import <UIKit/UIKit.h>

@class EAGLView;

@interface SnakeworldAppDelegate : NSObject <UIAccelerometerDelegate, UIApplicationDelegate> {
	IBOutlet UIWindow *window;
	IBOutlet EAGLView *glView;
}

@property (nonatomic, retain) UIWindow *window;
@property (nonatomic, retain) EAGLView *glView;

@end

