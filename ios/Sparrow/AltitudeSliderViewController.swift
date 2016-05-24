//
//  AltitudeSliderViewController.swift
//  Sparrow
//
//  Created by Amy Bearman on 5/23/16.
//  Copyright © 2016 Andrew Lim. All rights reserved.
//

import UIKit

class AltitudeSliderViewController: DroneViewController {
    
    @IBOutlet var labels: [UILabel]!
    
    @IBOutlet var altitudeSlider: UISlider! {
        didSet {
            altitudeSlider.transform = CGAffineTransformMakeRotation(CGFloat(-M_PI_2))
            let thumbImage = UIImage(named:"slider_thumb")
            let ratio : CGFloat = CGFloat (0.8)
            let size = CGSizeMake(thumbImage!.size.width * ratio, thumbImage!.size.height * ratio)
            altitudeSlider.setThumbImage(imageWithImage(thumbImage!, scaledToSize: size), forState: UIControlState.Normal)
            altitudeSlider.setMaximumTrackImage(UIImage(named:"slider_track"), forState: UIControlState.Normal)
            altitudeSlider.setMinimumTrackImage(UIImage(named:"slider_track"), forState: UIControlState.Normal)
            altitudeSlider.continuous = false
        }
    }
    
    @IBAction func altitudeSliderChange(sender: AnyObject) {
        debugPrint("Altitude slider changed to %f", self.altitudeSlider.value)
        let altitudeCommandArgs = [
            "dalt": self.altitudeSlider.value
        ]
        socket.emit("altitude_cmd", altitudeCommandArgs)
    }
    
    @IBAction func altitudeSliderReleased(sender: AnyObject) {
        let initialValue = self.altitudeSlider.value
        let finalValue = Float(0)
        UIView.animateWithDuration(0.15, delay: 0, options: .CurveLinear, animations: { () -> Void in
            self.altitudeSlider.setValue(initialValue, animated: true)
        }) { (completed) -> Void in
            UIView.animateWithDuration(0.15, delay: 0, options: .CurveLinear, animations: { () -> Void in
                self.altitudeSlider.setValue(finalValue, animated: true)
                }, completion: nil)
        }
    }
}
