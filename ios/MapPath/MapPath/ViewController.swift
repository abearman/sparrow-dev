//
//  ViewController.swift
//  MapPath
//
//  Created by Catherine Dong on 4/23/16.
//  Copyright © 2016 Sparrow. All rights reserved.
//

import UIKit
import MapKit

class ViewController: UIViewController, MKMapViewDelegate {

    @IBOutlet weak var mapView: MKMapView!
    var locations: [CLLocationCoordinate2D] = []
    var path: MKPolyline?
    var marker: MKAnnotation?
    var pinLocations: [MKAnnotation] = []
    
    override func viewDidLoad() {
        super.viewDidLoad()
        self.mapView.delegate = self
        let longPressRec = UILongPressGestureRecognizer(target: self, action: #selector(ViewController.dropWaypoint(_:)))
        self.mapView.addGestureRecognizer(longPressRec)
//        let startLoc = CLLocationCoordinate2DMake(37.421513, -122.168934)
//        onLocationUpdate(startLoc)
    }

    
    func onLocationUpdate(newLoc: CLLocationCoordinate2D) {
        self.locations.append(newLoc)
        
        drawMarker(newLoc)

        let region = MKCoordinateRegionMake(newLoc, MKCoordinateSpanMake(0.01, 0.01))
        self.mapView.setRegion(region, animated: true)
        
        drawPath()
    }
    
    @IBAction func gotoBob(sender: AnyObject) {
        let loc = CLLocationCoordinate2DMake(37.421513, -122.168934)
        onLocationUpdate(loc)
    }
    
    @IBAction func gotoCaltrain(sender: AnyObject) {
        let loc = CLLocationCoordinate2DMake(37.443203, -122.164461)
        onLocationUpdate(loc)
    }
    
    @IBAction func gotoGates(sender: AnyObject) {
        let loc = CLLocationCoordinate2DMake(37.430020, -122.173302)
        onLocationUpdate(loc)
    }
    
    @IBAction func dropPin(sender: AnyObject) {
        if let loc = self.locations.last {
            if (self.pinLocations.count > 0 &&
                loc.latitude == self.pinLocations.last!.coordinate.latitude &&
                loc.longitude == self.pinLocations.last!.coordinate.longitude
                ) {
                return;
            }
            let pin = MKPointAnnotation()
            pin.coordinate = loc
            self.pinLocations.append(pin)
            self.mapView.addAnnotation(pin)
        }
    }
    
    func drawMarker(coordinate: CLLocationCoordinate2D) {
        if (self.marker != nil) {
            self.mapView.removeAnnotation(self.marker!)
        }
        self.marker = CurrentLocationAnnotation(coordinate: coordinate)
        self.mapView.addAnnotation(self.marker!)
    }
    
    func mapView(mapView: MKMapView, viewForAnnotation annotation: MKAnnotation) -> MKAnnotationView? {
        if (annotation is CurrentLocationAnnotation) {
            if let currentLocIcon = self.mapView.dequeueReusableAnnotationViewWithIdentifier("currentLocIcon") {
                return currentLocIcon
            } else {
                let currentLocIcon = MKAnnotationView(annotation: annotation, reuseIdentifier: "currentLocIcon")
                currentLocIcon.image = UIImage(named: "current_location_icon")
                return currentLocIcon
            }
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
    
    func drawPath() {
        if (locations.count == 1) {
            return
        }
        if (self.path != nil) {
            self.mapView.removeOverlay(self.path!)
        }
        self.path = MKPolyline(coordinates: &locations, count: locations.count)
        self.mapView.addOverlay(self.path!)
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
            return circleRenderer
        }
        return MKPolylineRenderer();
    }

    func dropWaypoint(gestureRecognizer: UILongPressGestureRecognizer) {
        if (gestureRecognizer.state != UIGestureRecognizerState.Began) {
            return;
        }
        
        let touchPoint = gestureRecognizer.locationInView(self.mapView)
        let loc = self.mapView.convertPoint(touchPoint, toCoordinateFromView: self.mapView)
        let waypoint = WaypointAnnotation(coordinate: loc)
        self.mapView.addAnnotation(waypoint)
        // TODO: send goto waypoint message to server
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
