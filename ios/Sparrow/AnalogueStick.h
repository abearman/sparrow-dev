//
//  AnalogueStick.h
//  Controller
//

#import <UIKit/UIKit.h>

@class  AnalogueStick;

@protocol AnalogueStickDelegate <NSObject>

@optional
- (void)analogueStickDidChangeValue:(AnalogueStick *)analogueStick;

@end

@interface AnalogueStick : UIView

@property (nonatomic, readonly) CGFloat xValue;
@property (nonatomic, readonly) CGFloat yValue;
@property (nonatomic, assign) BOOL invertedYAxis;

@property (nonatomic, assign) IBOutlet id <AnalogueStickDelegate> delegate;

@property (nonatomic, readonly) UIImageView *backgroundImageView;
@property (nonatomic, readonly) UIImageView *handleImageView;

@end
