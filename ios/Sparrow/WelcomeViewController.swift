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
    
    var placeholderIPAddress = "10.1.1.188"
    
    override func viewDidLoad() {
        ipTextField.attributedPlaceholder =
            NSAttributedString(string: placeholderIPAddress, attributes: [NSForegroundColorAttributeName: UIColor.darkGrayColor()])
    }
    
    // Sets up the IP address in the DroneVC
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        if let identifier = segue.identifier {
            if identifier == "Drone View" {
                if !(ipTextField.text?.isEmpty)! {
                    DroneViewController.HOSTNAME = ipTextField.text!
                }
                
                // Otherwise just use the placeholder text
                DroneViewController.HOSTNAME = placeholderIPAddress
                
            }
        }
    }
    
}
