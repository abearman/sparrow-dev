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
        self.marker = CurrentLocationIcon(coordinate: coordinate)
        self.mapView.addAnnotation(self.marker!)
    }
    
    func mapView(mapView: MKMapView, viewForAnnotation annotation: MKAnnotation) -> MKAnnotationView? {
        if (annotation is CurrentLocationIcon) {
            var markerView: MKAnnotationView
            if (locations.count <= 1) {
                markerView = MKAnnotationView(annotation: annotation, reuseIdentifier: "currentLocationMarker")
                markerView.image = UIImage(named: "current_location_icon")
            } else {
                markerView = self.mapView.dequeueReusableAnnotationViewWithIdentifier("currentLocationMarker")!
            }
            return markerView
        }
        else if (annotation is MKPointAnnotation) {
            let pinView = MKPinAnnotationView()
            pinView.pinTintColor = UIColor.redColor()
            pinView.animatesDrop = true
            return pinView
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
}
