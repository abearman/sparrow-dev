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

class MapViewController: DroneViewController, MKMapViewDelegate {
        
    @IBOutlet weak var mapView: MKMapView!
        
    @IBOutlet weak var altitudeSlider: UISlider! {
        didSet {
            altitudeSlider.transform = CGAffineTransformMakeRotation(CGFloat(-M_PI_2))
            let thumbImage = UIImage(named:"slider_thumb")
            let ratio : CGFloat = CGFloat (0.8)
            let size = CGSizeMake(thumbImage!.size.width * ratio, thumbImage!.size.height * ratio)
            altitudeSlider.setThumbImage(self.imageWithImage(thumbImage!, scaledToSize: size), forState: UIControlState.Normal)
            altitudeSlider.setMaximumTrackImage(UIImage(named:"slider_track"), forState: UIControlState.Normal)
            altitudeSlider.setMinimumTrackImage(UIImage(named:"slider_track"), forState: UIControlState.Normal)
            altitudeSlider.continuous = false
        }
    }
    @IBOutlet weak var rotationSlider: UISlider! {
        didSet {
            let thumbImage = UIImage(named:"slider_thumb")
            let ratio : CGFloat = CGFloat (0.8)
            let size = CGSizeMake(thumbImage!.size.width * ratio, thumbImage!.size.height * ratio)
            rotationSlider.setThumbImage(self.imageWithImage(thumbImage!, scaledToSize: size), forState: UIControlState.Normal)
            rotationSlider.setMaximumTrackImage(UIImage(named:"slider_track"), forState: UIControlState.Normal)
            rotationSlider.setMinimumTrackImage(UIImage(named:"slider_track"), forState: UIControlState.Normal)
            rotationSlider.continuous = false
        }
    }

    override func viewDidLoad() {
        super.viewDidLoad()
            
        mapView.delegate = self
        mapView.rotateEnabled = false
        mapView.showsCompass = true
        mapView.layer.borderWidth = 2
        mapView.layer.borderColor = UIColor.darkGrayColor().CGColor
        let longPressRec = UILongPressGestureRecognizer(target: self, action: #selector(MapViewController.dropWaypoint(_:)))
        self.mapView.addGestureRecognizer(longPressRec)
        // TODO: remove dummy initial location below
        // let startLoc = CLLocationCoordinate2DMake(37.430020, -122.173302)
        // let startYaw = M_PI/2.0
        // onLocationUpdate(startLoc, yaw: startYaw)

        debugPrint("Connecting to server control socket...")
        initializeSocket()
        connectToSocket()
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
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
        
        // Unpack JSON data
        let encodedData = data[0].dataUsingEncoding(NSUTF8StringEncoding, allowLossyConversion: false)!
        do {
            if let jsonData: AnyObject = try NSJSONSerialization.JSONObjectWithData(encodedData, options: .AllowFragments) {
                let latitude = jsonData["lat"] as? Double
                let longitude = jsonData["lon"] as? Double
                let altitude = jsonData["alt"] as? Double
                let yaw = jsonData["yaw"] as? Double
                
                latestLat = latitude!
                latestLong = longitude!
                latestAlt = altitude!
                

                // Update MKMapView
                let loc = CLLocationCoordinate2DMake(latitude!, longitude!)
                onLocationUpdate(loc, yaw: yaw!)
            }
            
        } catch {
            print("error serializing JSON: \(error)")

        }
    }

    
// =================================== MOVEMENT CONTROL ===================================

    func dropWaypoint(gestureRecognizer: UILongPressGestureRecognizer) {
        for childVC in self.childViewControllers {
            if let controlBarVC = childVC as? ControlBarViewController {
                if (gestureRecognizer.state != UIGestureRecognizerState.Began) {
                    return;
                }
                
                if (!controlBarVC.waypointButton.selected) {
                    return;
                }
                
                let touchPoint = gestureRecognizer.locationInView(self.mapView)
                let loc = self.mapView.convertPoint(touchPoint, toCoordinateFromView: self.mapView)
                let waypoint = WaypointAnnotation(coordinate: loc)
                self.mapView.addAnnotation(waypoint)
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
    
    @IBAction func rotationSliderChange(sender: AnyObject) {
        debugPrint("Rotation slider changed to %f", self.rotationSlider.value)
        let rotationCommandArgs = [
            "heading": self.rotationSlider.value
        ]
        debugPrint("Emitting rotation_cmd...")
        socket.emit("rotation_cmd", rotationCommandArgs)
    }
    
    @IBAction func rotationSliderReleased(sender: AnyObject) {
        let initialValue = self.rotationSlider.value
        let finalValue = Float(0)
        UIView.animateWithDuration(0.15, delay: 0, options: .CurveLinear, animations: { () -> Void in
            self.rotationSlider.setValue(initialValue, animated: true)
            }) { (completed) -> Void in
                UIView.animateWithDuration(0.15, delay: 0.1, options: .CurveLinear, animations: { () -> Void in
                    self.rotationSlider.setValue(finalValue, animated: true)
                    }, completion: nil)
        }
    }
    
    func imageWithImage(image:UIImage, scaledToSize newSize:CGSize) -> UIImage{
        UIGraphicsBeginImageContextWithOptions(newSize, false, 0.0);
        image.drawInRect(CGRectMake(0, 0, newSize.width, newSize.height))
        let newImage:UIImage = UIGraphicsGetImageFromCurrentImageContext()
        UIGraphicsEndImageContext()
        return newImage
    }
    
// =================================== MAP VIEW ===================================
    
    var locations: [CLLocationCoordinate2D] = []
    var path: MKPolyline?
    var marker: MKAnnotation?
    var pinLocations: [MKAnnotation] = []
    
    var latestLat: Double = 0.0
    var latestLong: Double = 0.0
    var latestAlt: Double = 0.0

    func onLocationUpdate(newLoc: CLLocationCoordinate2D, yaw: Double) {
        self.locations.append(newLoc)
        
        drawMarker(newLoc, yaw: yaw)
        
        if (locations.count == 1) {
            let region = MKCoordinateRegionMake(newLoc, MKCoordinateSpanMake(0.01, 0.01))
            self.mapView.setRegion(region, animated: true)
        }
        
        drawPath()
    }

    func drawMarker(coordinate: CLLocationCoordinate2D, yaw: Double) {
        if (marker != nil) {
            mapView.removeAnnotation(marker!)
        }
        marker = CurrentLocationAnnotation(coordinate: coordinate, angle: yaw)
        mapView.addAnnotation(marker!)
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
    
}
