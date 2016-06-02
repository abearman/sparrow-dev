//
//  ControlBarViewController.swift
//  Sparrow
//
//  Created by Amy Bearman on 5/22/16.
//  Copyright © 2016 Andrew Lim. All rights reserved.
//

import UIKit
import MapKit

class ControlBarViewController: DroneViewController, UIPopoverPresentationControllerDelegate {
    
    var droneVC = MapViewController()
    
    var controlBarModel = ControlBarModel()
    
    @IBOutlet var coordinateReadingLabel: UILabel!
    @IBOutlet var altitudeReadingLabel: UILabel!
    @IBOutlet var waypointButton: UIButton!
    @IBOutlet var sarButton: UIButton!
    @IBOutlet var dropPinButton: UIButton!
    @IBOutlet var launchButton: UIButton!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Disable the launch button until a connection is received
        disableLaunchButton()
       
        waypointButton.setTitleColor(UIColor.whiteColor(), forState: UIControlState.Normal)
    }
    
    override func viewDidLayoutSubviews() {
        droneVC = self.parentViewController as! MapViewController
        
        debugPrint("Connecting to server control socket...")
        initializeSocket()
        connectToSocket()
    }
    
    override func viewDidAppear(animated: Bool) {
        updateLaunchButton()
        updateWaypointButtons()
    }

    // =================================== SERVER ===================================
    
    
    override func initializeSocket() {
        super.initializeSocket()
        
        // Initializes launch button on connection
        socket.on("connect") {[weak self] data, ack in
            debugPrint("received connect event")
            self?.enableLaunchButton()
        }
        
        // Constant fetching for latest GPS coordinates
        socket.on("gps_pos_ack") {[weak self] data, ack in
            debugPrint("received gps_pos_ack event")
            self?.handleGPSPos(data)
            return
        }
    }
    
    // =================================== COORD/ALT LABEL UPDATING ===================================
    
    override func handleGPSPos(data: AnyObject) {
        super.handleGPSPos(data)
            
        // Update coordinate label
        coordinateReadingLabel.text = "(" + String(format: "%.3f", latestLat) + "N, " + String(format: "%.3f", latestLong) + "W)"
        
        // Update altitude label
        var altitudeVal = latestAlt
        if altitudeVal < 0.0 {
            altitudeVal = 0.0
        }
        altitudeReadingLabel.text = String(format: "%.3f", altitudeVal) + " m"
    }
    
    // =================================== FREEZE BUTTON ===================================
    
    @IBAction func freezeButtonClicked(sender: AnyObject) {
        debugPrint("Freeze button clicked")
        let lateralCommandArgs = [
            "direction": "stop"
        ]
        socket.emit("lateral_cmd", lateralCommandArgs)
    }
    
    
    // =================================== LAUNCH BUTTON ===================================
    
    var isInFlight: Bool {
        get {
            return controlBarModel.isInFlight
        } set {
            controlBarModel.isInFlight = newValue
            updateLaunchButton()
            updateWaypointButtons()
        }
    }
    
    var isTakingOff: Bool {
        get {
            return controlBarModel.isTakingOff
        } set {
            controlBarModel.isTakingOff = newValue
            updateLaunchButton()
            updateWaypointButtons()
        }
    }
    
    var isLanding: Bool {
        get {
            return controlBarModel.isLanding
        } set {
            controlBarModel.isLanding = newValue
            updateLaunchButton()
            updateWaypointButtons()
        }
    }
    
    func enableLaunchButton() {
        launchButton.enabled = true
        launchButton.alpha = 1.0
    }
    
    func disableLaunchButton() {
        launchButton.enabled = false
        launchButton.alpha = 0.5
    }
    
    @IBAction func launchButtonClicked(sender: AnyObject) {
        let mapViewController:MapViewController = self.parentViewController as! MapViewController
        // Takeoff command
        if (!isInFlight) {
            debugPrint("Sending take off request")
            isInFlight = true
            isTakingOff = true
            self.droneVC.addCommandToQueue(LAUNCH_DRONE)
            
            HTTPPostJSON(buildSocketAddr + "/control/take_off", jsonObj: []) {
                (data: String, error: String?) -> Void in
                if error != nil {
                    print(error)
                } else {  // Done taking off
                    print("data is : \n\n\n")
                    debugPrint("Take off request completed.")
                    print(data)
                    self.isTakingOff = false
                }
            }
            mapViewController.overlayView.hidden = true
            
            // Land command
        } else {
            debugPrint("Sending land request")
            isLanding = true
            self.droneVC.addCommandToQueue(LAND_DRONE)
            
            HTTPPostJSON(buildSocketAddr + "/control/land", jsonObj: []) {
                (data: String, error: String?) -> Void in
                if error != nil {
                    print(error)
                } else {  // Done landing
                    print("data is : \n\n\n")
                    debugPrint("Land request completed.")
                    print(data)
                    self.isLanding = false
                    self.isInFlight = false
                    dispatch_async(dispatch_get_main_queue()) { [weak self] in
                        self?.performSegueWithIdentifier("summarySegue", sender: nil)
                    }
                }
            }
        }
    }
    
    func updateLaunchButton() {
        dispatch_async(dispatch_get_main_queue()) { [weak self] in
            let isInFlight = (self?.isInFlight)!
            let isTakingOff = (self?.isTakingOff)!
            let isLanding = (self?.isLanding)!
            if !isInFlight && !isTakingOff && !isLanding {  // On ground, waiting for launch
                self?.launchButton.setTitle("LAUNCH", forState: UIControlState.Normal)
                self?.launchButton.backgroundColor = UIColor(red: 123.0/255.0, green: 220.0/255.0, blue: 153.0/255.0, alpha: 1.0)
                self?.launchButton.enabled = true
            } else if (isInFlight && isTakingOff && !isLanding) {  // Taking off
                self?.launchButton.setTitle("TAKING OFF", forState: UIControlState.Normal)
                self?.launchButton.backgroundColor = UIColor.lightGrayColor()
                self?.launchButton.enabled = false
            } else if (isInFlight && !isTakingOff && !isLanding) {  // In flight, waiting for land
                self?.launchButton.setTitle("LAND", forState: UIControlState.Normal)
                self?.launchButton.backgroundColor = UIColor.redColor()
                self?.launchButton.enabled = true
            } else if (isInFlight && !isTakingOff && isLanding) {  // Landing
                self?.launchButton.setTitle("LANDING", forState: UIControlState.Normal)
                self?.launchButton.backgroundColor = UIColor.lightGrayColor()
                self?.launchButton.enabled = false
            }
        }
    }
    
    func updateWaypointButtons() {
        dispatch_async(dispatch_get_main_queue()) { [weak self] in
            let isInFlight = (self?.isInFlight)!
            let isTakingOff = (self?.isTakingOff)!
            let isLanding = (self?.isLanding)!
            if !isInFlight && !isTakingOff && !isLanding {  // On ground, waiting for launch
                self?.disableWaypointButtons()
            } else if (isInFlight && isTakingOff && !isLanding) {  // Taking off
                self?.disableWaypointButtons()
            } else if (isInFlight && !isTakingOff && !isLanding) {  // In flight, waiting for land
                self?.enableWaypointButtons()
            } else if (isInFlight && !isTakingOff && isLanding) {  // Landing
                self?.disableWaypointButtons()
            }
        }
    }
    
    func disableWaypointButtons() {
        waypointButton.enabled = false
        waypointButton.alpha = 0.5
        sarButton.enabled = false
        sarButton.alpha = 0.5
        dropPinButton.enabled = false
        dropPinButton.alpha = 0.5
    }
    
    func enableWaypointButtons() {
        waypointButton.enabled = true
        waypointButton.alpha = 1
        sarButton.enabled = true
        sarButton.alpha = 1
        dropPinButton.enabled = true
        dropPinButton.alpha = 1

    }
    
    // =================================== DROP PIN BUTTON ===================================
    
    @IBAction func dropPinButtonClicked(sender: AnyObject) {
        if let loc = droneVC.locations.last {
            if (droneVC.pinLocations.count > 0 &&
                loc.latitude == droneVC.pinLocations.last!.coordinate.latitude &&
                loc.longitude == droneVC.pinLocations.last!.coordinate.longitude
                ) {
                return;
            }
            let pin = MKPointAnnotation()
            pin.coordinate = loc
            droneVC.pinLocations.append(pin)
            droneVC.mapView.addAnnotation(pin)
            
            droneVC.addCommandToQueue(String(format: "Pin at (%.03fN, %.03fW)", loc.latitude, loc.longitude))
        }
    }
    
    // =================================== WAYPOINT BUTTON ===================================
    
    @IBAction func waypointButtonToggle(sender: AnyObject) {
        if (!waypointButton.selected) {
            waypointButton.selected = true
            waypointButton.backgroundColor = UIColor(red: 21.0/255.0, green: 126.0/255.0, blue: 251.0/255.0, alpha: 0.5)
            
        } else {
            waypointButton.selected = false
            waypointButton.backgroundColor = UIColor(red: 21.0/255.0, green: 126.0/255.0, blue: 251.0/255.0, alpha: 1.0)
        }
    }
    
    // =================================== SAR PATHS ===================================
    
    func adaptivePresentationStyleForPresentationController(controller: UIPresentationController) -> UIModalPresentationStyle {
        return UIModalPresentationStyle.FullScreen
    }
    
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        if segue.identifier == "popoverSegue" {
            if let popoverTVC = segue.destinationViewController as? SARMenuViewController {
                if let droneVC = self.parentViewController as? MapViewController {
                    popoverTVC.modalPresentationStyle = UIModalPresentationStyle.Popover
                    popoverTVC.popoverPresentationController!.delegate = self
                    popoverTVC.delegate = droneVC
                    segue.destinationViewController.popoverPresentationController!.sourceView = sender as! UIButton
                    segue.destinationViewController.popoverPresentationController!.sourceRect = (sender as! UIButton).bounds
                }
            }
        } else if segue.identifier == "summarySegue" {
            if let popoverTVC = segue.destinationViewController as? SummaryViewController {
                if let droneVC = self.parentViewController as? MapViewController {
                    popoverTVC.modalPresentationStyle = UIModalPresentationStyle.Popover
                    popoverTVC.popoverPresentationController!.delegate = self
                    popoverTVC.delegate = droneVC
                    popoverTVC.popoverPresentationController?.permittedArrowDirections = UIPopoverArrowDirection.init(rawValue: 0)
                    popoverTVC.popoverPresentationController!.sourceRect = CGRectMake(CGRectGetMidX(self.view.bounds), CGRectGetMidY(self.view.bounds),0,0)
                    popoverTVC.pinLocations = droneVC.pinLocations
                    let mapViewController:MapViewController = self.parentViewController as! MapViewController
                    mapViewController.overlayView.hidden = false
                }
            }
        }
    }
    
}
