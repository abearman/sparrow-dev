//
//  UIColorExtensions.swift
//  Sparrow
//
//  Created by Amy Bearman on 5/23/16.
//  Copyright © 2016 Andrew Lim. All rights reserved.
//

import Foundation
import UIKit

extension UIColor {
    
    func redColor() -> Int {
        var fRed : CGFloat = 0
        var fGreen : CGFloat = 0
        var fBlue : CGFloat = 0
        var fAlpha: CGFloat = 0
        if self.getRed(&fRed, green: &fGreen, blue: &fBlue, alpha: &fAlpha) {
            let iRed = Int(fRed * 255.0)
            
            //  (Bits 24-31 are alpha, 16-23 are red, 8-15 are green, 0-7 are blue).
            return iRed
        } else {
            // Could not extract RGBA components:
            return 0
        }
    }
    
    func greenColor() -> Int {
        var fRed : CGFloat = 0
        var fGreen : CGFloat = 0
        var fBlue : CGFloat = 0
        var fAlpha: CGFloat = 0
        if self.getRed(&fRed, green: &fGreen, blue: &fBlue, alpha: &fAlpha) {
            let iGreen = Int(fGreen * 255.0)
            
            //  (Bits 24-31 are alpha, 16-23 are red, 8-15 are green, 0-7 are blue).
            return iGreen
        } else {
            // Could not extract RGBA components:
            return 0
        }
    }
    
    func blueColor() -> Int {
        var fRed : CGFloat = 0
        var fGreen : CGFloat = 0
        var fBlue : CGFloat = 0
        var fAlpha: CGFloat = 0
        if self.getRed(&fRed, green: &fGreen, blue: &fBlue, alpha: &fAlpha) {
            let iBlue = Int(fBlue * 255.0)
            
            //  (Bits 24-31 are alpha, 16-23 are red, 8-15 are green, 0-7 are blue).
            return iBlue
        } else {
            // Could not extract RGBA components:
            return 0
        }
    }
}