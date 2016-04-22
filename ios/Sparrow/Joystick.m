//
//  JLJoystick.m
//  JLJoyStick
//
//  Created by Lewis, Jordan on 6/24/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#import "Joystick.h"

@implementation Joystick

@synthesize theAngle, isUsingJoyStick, delegate;

- (void)stickMoveTo:(CGPoint)deltaToCenter
{
    CGRect fr = stick.frame;
    fr.origin.x = deltaToCenter.x;
    fr.origin.y = deltaToCenter.y;
    stick.frame = fr;
    
    theAngle = atan2(stick.center.y-(self.frame.size.width/2), stick.center.x-(self.frame.size.height/2));
    
    if ([delegate respondsToSelector:@selector(joystick:angleChanged:)]) {
        [delegate joystick:self angleChanged:theAngle];
    }
}

- (void)touchEvent:(NSSet *)touches
{
    
    if([touches count] != 1)
        return ;
    
    UITouch *touch = [touches anyObject];
    
    CGPoint touchPoint = [touch locationInView:self];
    CGPoint dtarget, dir;
    dir.x = touchPoint.x - mCenter.x;
    dir.y = touchPoint.y - mCenter.y;
    
    double len = sqrt(dir.x * dir.x + dir.y * dir.y);
    
    if(len < 10.0 && len > -10.0)
    {
        // center pos
        dtarget.x = 0.0;
        dtarget.y = 0.0;
        dir.x = 0;
        dir.y = 0;
        isUsingJoyStick = NO;
    }
    else
    {
        double len_inv = (1.0 / len);
        dir.x *= len_inv;
        dir.y *= len_inv;
        dtarget.x = dir.x * STICK_CENTER_TARGET_POS_LEN;
        dtarget.y = dir.y * STICK_CENTER_TARGET_POS_LEN;
        isUsingJoyStick = YES;
    }
    [self stickMoveTo:dtarget];
    
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self touchEvent:touches];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self touchEvent:touches];
}

-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    stick.center = CGPointMake(self.frame.size.width/2, self.frame.size.height/2);
    theAngle = 0;
    isUsingJoyStick = NO;
}

- (id)initWithLocation:(CGPoint)location size:(int)size backgroundColor:(UIColor *)bgColor andStickColor:(UIColor *)sColor {
    self = [super initWithFrame:CGRectMake(location.x, location.y, size, size)];
    
    mCenter.x = size/2;
    mCenter.y = size/2;
    STICK_CENTER_TARGET_POS_LEN = size/4;
    
    isUsingJoyStick = NO;
    
    backGround = [[UIView alloc] initWithFrame:CGRectMake(0, 0, size, size)];
    backGround.backgroundColor = bgColor;
    backGround.layer.cornerRadius = size/2;
    [self addSubview:backGround];
    
    stick = [[UIView alloc] initWithFrame:CGRectMake(0, 0, size, size)];
    stick.backgroundColor = [UIColor clearColor];
    stick.layer.cornerRadius = size/2;
    
    UIView *inner = [[UIView alloc] initWithFrame:CGRectMake(size/4, size/4, size/2, size/2)];
    inner.backgroundColor = sColor;
    inner.layer.cornerRadius = size/4;
    [stick addSubview:inner];
    
    [self addSubview:stick];
    
    self.userInteractionEnabled = YES;
    [self setBackgroundColor:[UIColor clearColor]];
    
    return self;
}


// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
    //[backGround drawInRect:rect];
}


@end
