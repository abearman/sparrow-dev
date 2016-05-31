//
//  LateralMovementViewController.swift
//  Sparrow
//
//  Created by Amy Bearman on 5/22/16.
//  Copyright © 2016 Andrew Lim. All rights reserved.
//

import UIKit

class LateralMovementViewController: DroneViewController {
    
    var droneVC = MapViewController()

    @IBOutlet var lateralButtonsView: UIImageView!
    
    override func viewDidLayoutSubviews() {
        droneVC = self.parentViewController as! MapViewController
    }
    
    @IBAction func lateralButtonDown(sender: AnyObject) {
        if let button = sender as? UIButton {
            if let buttonID = button.restorationIdentifier {
                debugPrint("Lateral button %@ pressed", buttonID)
                droneVC.addCommandToQueue("Move \(buttonID)")
                let lateralCommandArgs = [
                    "direction": buttonID
                ]
                socket.emit("lateral_cmd", lateralCommandArgs)
            }
        }
    }
    
    @IBAction func lateralButtonUp(sender: AnyObject) {
        debugPrint("Lateral button released")
        let lateralCommandArgs = [
            "direction": "stop"
        ]
        socket.emit("lateral_cmd", lateralCommandArgs)
    }
    
    
}
