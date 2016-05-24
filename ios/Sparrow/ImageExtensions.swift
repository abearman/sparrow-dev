//
//  ImageExtensions.swift

//  Sparrow
//
//  Created by Catherine Dong on 5/5/16.
//  Copyright © 2016 Andrew Lim. All rights reserved.
//

import Foundation
import UIKit

extension UIImage {
    func rotate(radians: CGFloat) -> UIImage {
        // calculate the size of the rotated view's containing box for our drawing space
        let rotatedViewBox = UIView(frame: CGRect(origin: CGPointZero, size: size))
        let t = CGAffineTransformMakeRotation(radians);
        rotatedViewBox.transform = t
        let rotatedSize = rotatedViewBox.frame.size
        
        // Create the bitmap context
        UIGraphicsBeginImageContext(rotatedSize)
        let bitmap = UIGraphicsGetCurrentContext()
        
        // Move the origin to the middle of the image so we will rotate and scale around the center.
        CGContextTranslateCTM(bitmap, rotatedSize.width / 2.0, rotatedSize.height / 2.0);
        
        //   // Rotate the image context
        CGContextRotateCTM(bitmap, radians + CGFloat(M_PI));
        
        CGContextDrawImage(bitmap, CGRectMake(-size.width / 2, -size.height / 2, size.width, size.height), CGImage)
        
        let newImage = UIGraphicsGetImageFromCurrentImageContext()
        UIGraphicsEndImageContext()
        
        return newImage
    }
    
    func averagePixelColor() -> UIColor {
        
        let rgba = UnsafeMutablePointer<CUnsignedChar>.alloc(4)
        let colorSpace = CGColorSpaceCreateDeviceRGB()
        let info = CGBitmapInfo(rawValue: CGImageAlphaInfo.PremultipliedLast.rawValue)
        let context: CGContextRef = CGBitmapContextCreate(rgba, 1, 1, 8, 4, colorSpace, CGImageAlphaInfo.PremultipliedLast.rawValue)
        
        CGContextDrawImage(context, CGRectMake(0, 0, 1, 1), self.CGImage)
        
        if rgba[3] > 0 {
            
            let alpha: CGFloat = CGFloat(rgba[3]) / 255.0
            let multiplier: CGFloat = alpha / 255.0
            
            return UIColor(red: CGFloat(rgba[0]) * multiplier, green: CGFloat(rgba[1]) * multiplier, blue: CGFloat(rgba[2]) * multiplier, alpha: alpha)
            
        } else {
            
            return UIColor(red: CGFloat(rgba[0]) / 255.0, green: CGFloat(rgba[1]) / 255.0, blue: CGFloat(rgba[2]) / 255.0, alpha: CGFloat(rgba[3]) / 255.0)
        }
    }
    /*public func averagePixelColor() -> UIColor {
        let colorSpace = CGColorSpaceCreateDeviceRGB()
        //var rgba = [CUnsignedChar](count: 4, repeatedValue: 0)
        
        let context = CGBitmapContextCreate(nil, 1, 1, 8, 4, colorSpace, CGImageAlphaInfo.PremultipliedLast.rawValue | CGBitmapInfo.ByteOrder32Big.rawValue)
        CGContextDrawImage(context, CGRectMake(0, 0, 1, 1), self.CGImage)
        CGColorSpaceRelease(colorSpace);
        CGContextRelease(context);
        
        if(rgba[3] &gt; 0) {
            CGFloat alpha = ((CGFloat)rgba[3])/255.0;
            CGFloat multiplier = alpha/255.0;
            return [UIColor colorWithRed:((CGFloat)rgba[0])*multiplier
                green:((CGFloat)rgba[1])*multiplier
                blue:((CGFloat)rgba[2])*multiplier
                alpha:alpha];
        }
        else {
            return [UIColor colorWithRed:((CGFloat)rgba[0])/255.0
                green:((CGFloat)rgba[1])/255.0
                blue:((CGFloat)rgba[2])/255.0
                alpha:((CGFloat)rgba[3])/255.0];
        }
    }*/
}
