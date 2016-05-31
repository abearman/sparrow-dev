//
//  MapViewController.swift
//  Sparrow
//
//  Created by Andrew Lim on 4/21/16.
//  Copyright © 2016 Andrew Lim. All rights reserved.
//

import UIKit
import Foundation
import MapKit

class MapViewController: DroneViewController, MKMapViewDelegate, UITableViewDataSource, UITableViewDelegate {
        
    @IBOutlet weak var mapView: MKMapView!
    @IBOutlet weak var sarControlView: UIView!
    @IBOutlet weak var sarStepSlider: UISlider!
    @IBOutlet weak var sarControlCancelButton: UIButton!
    @IBOutlet weak var sarControlConfirmButton: UIButton!
    @IBOutlet weak var overlayView: UIView!
    
    var pathType: String = "";
    var offsets : [(dNorth: Double, dEast: Double)] = [];

    override func viewDidLoad() {
        super.viewDidLoad()
        
        sarControlView.hidden = true
        mapView.delegate = self
        mapView.showsCompass = true
        mapView.layer.borderWidth = 2
        mapView.layer.borderColor = UIColor.darkGrayColor().CGColor
        let longPressRec = UILongPressGestureRecognizer(target: self, action: #selector(MapViewController.dropWaypoint(_:)))
        self.mapView.addGestureRecognizer(longPressRec)
        
        commandsQueueTableView.hidden = true
        
        debugPrint("Connecting to server control socket...")
        initializeSocket()
        connectToSocket()
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
    }
    
    
    // ============= OVERLAY ============= 
    
    @IBAction func overlayExited(sender: AnyObject) {
        overlayView.hidden = true
    }
    
// =================================== SERVER ===================================
    
    override func initializeSocket() {
        super.initializeSocket()
        
        // Constant fetching for latest GPS coordinates
        socket.on("gps_pos_ack") {[weak self] data, ack in
            debugPrint("received gps_pos_ack event")
            self?.handleGPSPos(data)
            return
        }
    }
    
    
    override func handleGPSPos(data: AnyObject) {
        super.handleGPSPos(data)

        // Update MKMapView
        if (latestLat != 0.0) && (latestLong != 0.0) {
            let loc = CLLocationCoordinate2DMake(latestLat, latestLong)
            onLocationUpdate(loc, yaw: latestYaw)
        }
    }

    
// =================================== MOVEMENT CONTROL ===================================
    
    @IBAction func stopDrone(sender: AnyObject) {
        
    }

    func dropWaypoint(gestureRecognizer: UILongPressGestureRecognizer) {
        for childVC in self.childViewControllers {
            if let controlBarVC = childVC as? ControlBarViewController {
                if (gestureRecognizer.state != UIGestureRecognizerState.Began) {
                    return;
                }
                
                if (!controlBarVC.waypointButton.selected) {
                    return;
                }
                
                let touchPoint = gestureRecognizer.locationInView(mapView)
                let loc = mapView.convertPoint(touchPoint, toCoordinateFromView: mapView)
                let waypoint = WaypointAnnotation(coordinate: loc)
                mapView.addAnnotation(waypoint)
                
                addCommandToQueue(String(format: "Waypoint at (%.03fN, %.03fW)", loc.latitude, loc.longitude))

                let waypointArgs = [
                    "lat": loc.latitude,
                    "lon": loc.longitude
                ]
                socket.emit("waypoint_cmd", waypointArgs)
                
                controlBarVC.waypointButton.selected = false
                controlBarVC.waypointButton.backgroundColor = UIColor(red: 21.0/255.0, green: 126.0/255.0, blue: 251.0/255.0, alpha: 1.0)
            }
        }
    }
    
// =================================== MAP VIEW ===================================
    
    var locations: [CLLocationCoordinate2D] = []
    var path: MKPolyline?
    var marker: MKAnnotation?
    var pinLocations: [MKAnnotation] = []

    func onLocationUpdate(newLoc: CLLocationCoordinate2D, yaw: Double) {
        self.locations.append(newLoc)
        drawMarker(newLoc, droneHeading: yaw)
        
        if (locations.count == 1) {
            let region = MKCoordinateRegionMake(newLoc, MKCoordinateSpanMake(0.01, 0.01))
            self.mapView.setRegion(region, animated: true)
        }
        
        drawPath()
    }
    
    @IBAction func clearMap(sender: AnyObject) {
        locations.removeAll()
        mapView.removeOverlay(path!)
    }

    func drawMarker(coordinate: CLLocationCoordinate2D, droneHeading: Double) {
        print("Map heading: \(mapView.camera.heading)")  // This is in degrees
        print("Drone yaw: \(radiansToDegrees(droneHeading))")  // This is in radians
        let mapHeadingRadians = degreesToRadians(mapView.camera.heading)
        let mapDroneYaw = -(mapHeadingRadians - droneHeading)
        let newMarker = CurrentLocationAnnotation(coordinate: coordinate, angle: mapDroneYaw)
        mapView.addAnnotation(newMarker)
        if (marker != nil) {
            mapView.removeAnnotation(marker!)
        }
        marker = newMarker
    }
    
    func degreesToRadians(degrees: Double) -> Double {
        return degrees * M_PI / 180
    }
    
    func radiansToDegrees(radians: Double) -> Double {
        return radians * 180 / M_PI
    }
    
    func mapView(mapView: MKMapView, regionWillChangeAnimated animated: Bool) {
        drawMarker(CLLocationCoordinate2D(latitude: latestLat, longitude: latestLong), droneHeading: latestYaw)
    }

    func drawPath() {
        if (locations.count == 1) {
            return
        }
        if (self.path != nil) {
            self.mapView.removeOverlay(path!)
        }
        self.path = MKPolyline(coordinates: &locations, count: locations.count)
        
        // TODO: area visualizations - improve latency
        /*for (index, location) in locations.enumerate() {
            if (index % 5 == 0) {
                let circle : MKCircle = MKCircle(centerCoordinate: location, radius: 10)
                //self.mapView.addOverlay(circle)
            }
        }*/
        
        self.mapView.addOverlay(self.path!)
    }
    
    func mapView(mapView: MKMapView, viewForAnnotation annotation: MKAnnotation) -> MKAnnotationView? {
        if (annotation is CurrentLocationAnnotation) {
            // Direction is only correct if map is oriented with North up
            let angle = (annotation as! CurrentLocationAnnotation).angle
            let currentLocIcon = MKAnnotationView(annotation: annotation, reuseIdentifier: "currentLocIcon")
            currentLocIcon.image = UIImage(named: "current_loc_icon")?.rotate(CGFloat(angle))
            return currentLocIcon
        }
        else if (annotation is WaypointAnnotation) {
            if let waypointIcon = self.mapView.dequeueReusableAnnotationViewWithIdentifier("waypointIcon") {
                return waypointIcon
            } else {
                let waypointIcon = MKAnnotationView(annotation: annotation, reuseIdentifier: "waypointIcon")
                waypointIcon.image = UIImage(named: "waypoint_icon")
                return waypointIcon
            }
        }
        else if (annotation is MKPointAnnotation) {
            if let pinIcon = self.mapView.dequeueReusableAnnotationViewWithIdentifier("pinIcon") {
                return pinIcon
            } else {
                let pinIcon = MKAnnotationView(annotation: annotation, reuseIdentifier: "pinIcon")
                pinIcon.image = UIImage(named: "pin_icon")
                return pinIcon
            }
        }
        return MKAnnotationView()
    }
    
    func mapView(mapView: MKMapView, rendererForOverlay overlay: MKOverlay) -> MKOverlayRenderer {
        if (overlay is MKPolyline) {
            let pathRenderer = MKPolylineRenderer(overlay: overlay)
            pathRenderer.strokeColor = UIColor.blueColor()
            pathRenderer.lineWidth = 2.0
            return pathRenderer
        } else if (overlay is MKCircle) {
            let circleRenderer = MKCircleRenderer(overlay: overlay)
            circleRenderer.fillColor = UIColor.blueColor()
            circleRenderer.alpha = 0.01
            return circleRenderer
        }
        return MKPolylineRenderer();
    }
    
    func mapView(mapView: MKMapView, didAddAnnotationViews views: [MKAnnotationView]) {
        for view in views {
            if (view.reuseIdentifier == "pinIcon") {
                let endFrame = view.frame
                view.frame = CGRectMake(view.frame.origin.x, view.frame.origin.y - self.view.frame.size.height, view.frame.size.width, view.frame.size.height);
                UIView.animateWithDuration(
                    0.5, delay: 0.0, options: UIViewAnimationOptions.CurveEaseIn,
                    animations:{() in
                        view.frame = endFrame
                    },
                    completion:{(Bool) in
                        UIView.animateWithDuration(0.05, delay: 0.0, options: UIViewAnimationOptions.CurveEaseInOut, animations:{() in
                            view.transform = CGAffineTransformMakeScale(1.0, 0.8)
                            },
                            completion: {(Bool) in
                                UIView.animateWithDuration(0.3, delay: 0.0, options: UIViewAnimationOptions.CurveEaseInOut, animations:{() in
                                    view.transform = CGAffineTransformIdentity
                                    },
                                    completion: nil)
                        })
                    }
                )
            }
            else if (view.reuseIdentifier == "waypointIcon") {
                view.transform = CGAffineTransformMakeScale(1.5, 1.5)
                UIView.animateWithDuration(0.3, animations: {
                    view.transform = CGAffineTransformIdentity
                })
            }
        }
    }
    
    var sarLocations: [CLLocationCoordinate2D] = []
    var sarPreviewPath: MKPolyline?
    let DEFAULT_STEP : Double = 3
    
    func drawPreview() {
        let step : Double = Double(self.sarStepSlider.value)
        let degreeStep : Double = step * 0.00001
        let originalLat = self.latestLat
        let originalLong = self.latestLong
        for offset in offsets {
            let nextWaypoint = (originalLat + offset.dNorth * degreeStep,
                                originalLong + offset.dEast * degreeStep)
            let loc = CLLocationCoordinate2DMake(nextWaypoint.0, nextWaypoint.1)
            sarLocations.append(loc)
        }
        
        // show the preview
        sarPreviewPath = MKPolyline(coordinates: &sarLocations, count: sarLocations.count)
        self.mapView.addOverlay(sarPreviewPath!)
    }
    
    @IBAction func sarConfirmButtonClicked(sender: AnyObject) {
        self.sarControlView.hidden = true
        
        addCommandToQueue("Navigate \(pathType) SAR path")
        
        self.sarStepSlider.value = Float(DEFAULT_STEP)
        self.mapView.removeOverlay(sarPreviewPath!)
        self.sarLocations = []
        
        let step : Double = Double(self.sarStepSlider.value)
        let degreeStep : Double = step * 0.00001
        
        // travel the path
        let priority = DISPATCH_QUEUE_PRIORITY_DEFAULT
        dispatch_async(dispatch_get_global_queue(priority, 0)) {
            let originalLat = self.latestLat
            let originalLong = self.latestLong
            for offset in self.offsets {
                // let nextWaypoint = self.getLocationMeters(self.delegate!.latestLat, origLon: self.delegate!.latestLong, dNorth: Double(offset.dNorth * self.STEP), dEast: Double(offset.dEast * self.STEP))
                let nextWaypoint = (originalLat + offset.dNorth * degreeStep,
                                    originalLong + offset.dEast * degreeStep)
                let waypointArgs = [
                    "lat": nextWaypoint.0,
                    "lon": nextWaypoint.1
                ]
                self.socket.emit("waypoint_cmd", waypointArgs)
                let distance = sqrt(pow(Double(offset.dNorth * step), 2) + pow(Double(offset.dEast * step), 2))
                sleep(UInt32(distance / 0.5)) // change back to 2
            }
            print("FINISHED SAR PATH")
        }
        
    }
    
    @IBAction func sarCancelButtonClicked(sender: AnyObject) {
        self.sarControlView.hidden = true
        self.sarStepSlider.value = Float(DEFAULT_STEP)
        self.mapView.removeOverlay(sarPreviewPath!)
        self.sarLocations = []
    }
    
    @IBAction func stepSliderChanged(sender: AnyObject) {
        self.mapView.removeOverlay(sarPreviewPath!)
        self.sarLocations = []
        drawPreview()
    }
    
    
    //////////////// Commands queue ////////////////
    
    @IBOutlet var commandsQueueTableView: UITableView!
    
    var commandsQueue = [String]() {
        didSet {
            if (commandsQueue.count > 0) {
                commandsQueueTableView.hidden = false
                hideCommandsQueueButton.hidden = false
            }
            
            commandsQueueTableView.reloadData()
            let indexPath = NSIndexPath(forRow: commandsQueue.count - 1, inSection: 0)
            commandsQueueTableView.scrollToRowAtIndexPath(indexPath, atScrollPosition: UITableViewScrollPosition.Bottom, animated: true)
        }
    }
    
    @IBOutlet var hideCommandsQueueButton: UIButton!
    
    @IBAction func hideCommandsQueue(sender: AnyObject) {
        commandsQueueTableView.hidden = true
        hideCommandsQueueButton.hidden = true
    }
    
    func addCommandToQueue(command: String) {
        commandsQueue.append(command)
    }

    func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return commandsQueue.count
    }
    
    func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCellWithIdentifier("Command Identifier", forIndexPath: indexPath)
        cell.textLabel?.text = commandsQueue[indexPath.row]
        return cell
    }
    
    func tableView(tableView: UITableView, heightForRowAtIndexPath indexPath: NSIndexPath) -> CGFloat {
        return 40.0
    }
    
    
}
