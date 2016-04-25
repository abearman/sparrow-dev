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
    
    override func viewDidLoad() {
        super.viewDidLoad()
        mapView.delegate = self
        let loc1 = CLLocationCoordinate2DMake(37.430020, -122.173302)
        onLocationUpdate(loc1)
    }

    
    func onLocationUpdate(newLoc: CLLocationCoordinate2D) {
        self.locations.append(newLoc)
        
        drawMarker(newLoc)
//        if (marker != nil) {
//            self.mapView.removeOverlay(marker!)
//        }
//        marker = MKCircle(centerCoordinate: newLoc, radius: 30)
//        self.mapView.addOverlay(marker!)

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
    
    func drawMarker(coordinate: CLLocationCoordinate2D) {
        if (marker != nil) {
            mapView.removeAnnotation(marker!)
        }
        marker = CurrentLocationIcon(coordinate: coordinate)
        mapView.addAnnotation(marker!)
    }
    
    func mapView(mapView: MKMapView, viewForAnnotation annotation: MKAnnotation) -> MKAnnotationView? {
        if (annotation is CurrentLocationIcon) {
            var markerView: MKAnnotationView
            if (locations.count <= 1) {
                markerView = MKAnnotationView(annotation: annotation, reuseIdentifier: "currentLocationMarker")
                markerView.image = UIImage(named: "current_location_icon")
            } else {
                markerView = mapView.dequeueReusableAnnotationViewWithIdentifier("currentLocationMarker")!
            }
            return markerView
        }
        return MKAnnotationView()
    }
    
    func drawPath() {
        if (locations.count == 1) {
            return
        }
        if (self.path != nil) {
            self.mapView.removeOverlay(path!)
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
