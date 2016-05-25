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
    
    var droneTBC: DroneTabBarController?
    
    @IBOutlet var coordinateReadingLabel: UILabel!
    @IBOutlet var altitudeReadingLabel: UILabel!
    @IBOutlet var waypointButton: UIButton!
    @IBOutlet var sarButton: UIButton!
    @IBOutlet var dropPinButton: UIButton!
    @IBOutlet var launchButton: UIButton!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Disable the launch button until a connection is received
        launchButton.enabled = false
        launchButton.alpha = 0.5
        
        waypointButton.setTitleColor(UIColor.whiteColor(), forState: UIControlState.Normal)
        
        debugPrint("Connecting to server control socket...")
        initializeSocket()
        connectToSocket()
    }
    
    override func viewDidAppear(animated: Bool) {
        updateLaunchButton()
    }

    // =================================== SERVER ===================================
    
    
    override func initializeSocket() {
        super.initializeSocket()
        
        // Initializes launch button on connection
        socket.on("connect") {[weak self] data, ack in
            debugPrint("received connect event")
            // Enable launch button
            self?.launchButton.enabled = true
            self?.launchButton.alpha = 1.0
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
        
        // Unpack JSON data
        let encodedData = data[0].dataUsingEncoding(NSUTF8StringEncoding, allowLossyConversion: false)!
        do {
            if let jsonData: AnyObject = try NSJSONSerialization.JSONObjectWithData(encodedData, options: .AllowFragments) {
                let latitude = jsonData["lat"] as? Double
                let longitude = jsonData["lon"] as? Double
                let altitude = jsonData["alt"] as? Double
                
                // Update coordinate label
                if let latitudeVal = latitude {
                    if let longitudeVal = longitude {
                        coordinateReadingLabel.text = "(" + String(format: "%.3f", latitudeVal) + "N, " + String(format: "%.3f", longitudeVal) + "W)"
                    }
                }
                
                // Update altitude label
                if var altitudeVal = altitude {
                    if altitudeVal < 0.0 { altitudeVal = 0.0 }
                    altitudeReadingLabel.text = String(format: "%.3f", altitudeVal) + " m"
                }
            }
            
        } catch {
            print("error serializing JSON: \(error)")
            
        }
    }
    
    // =================================== LAUNCH BUTTON ===================================
    
    var isInFlight: Bool {
        get {
            return droneTBC!.controlBarModel.isInFlight
        } set {
            droneTBC!.controlBarModel.isInFlight = newValue
            updateLaunchButton()
        }
    }
    
    var isTakingOff: Bool {
        get {
            return droneTBC!.controlBarModel.isTakingOff
        } set {
            droneTBC!.controlBarModel.isTakingOff = newValue
            updateLaunchButton()
        }
    }
    
    var isLanding: Bool {
        get {
            return droneTBC!.controlBarModel.isLanding
        } set {
            droneTBC!.controlBarModel.isLanding = newValue
            updateLaunchButton()
        }
    }
    
    @IBAction func launchButtonClicked(sender: AnyObject) {
        let mapViewController:MapViewController = self.parentViewController as! MapViewController
        // Takeoff command
        if (!isInFlight) {
            debugPrint("Sending take off request")
            isInFlight = true
            isTakingOff = true
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
    
    // =================================== DROP PIN BUTTON ===================================
    
    @IBAction func dropPinButtonClicked(sender: AnyObject) {
        if let droneVC = self.parentViewController as? MapViewController {
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
            }
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
        //return UIModalPresentationStyle.None
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
