//
//  WelcomeViewController.swift
//  Sparrow
//
//  Created by Amy Bearman on 6/1/16.
//  Copyright © 2016 Andrew Lim. All rights reserved.
//

import UIKit

class WelcomeViewController: UIViewController, UITextFieldDelegate {
    
    @IBOutlet weak var ipTextField: UITextField!
    
    var ipAddress = "10.1.1.188"
    
    override func viewDidLoad() {
        ipTextField.attributedPlaceholder =
            NSAttributedString(string: ipAddress, attributes: [NSForegroundColorAttributeName: UIColor.darkGrayColor()])
    }
    
    
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        if let identifier = segue.identifier {
            if identifier == "Drone View" {
                if !(ipTextField.text?.isEmpty)! {
                    ipAddress = ipTextField.text!
                    
                    // Set up the IP address in the DroneVC
                    DroneViewController.HOSTNAME = ipAddress
                }
            }
        }
    }
    
}
