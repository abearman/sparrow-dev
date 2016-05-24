//
//  SummaryViewController.swift
//  Sparrow
//
//  Created by Catherine Dong on 5/24/16.
//  Copyright © 2016 Andrew Lim. All rights reserved.
//

import Foundation
import Darwin
import MapKit
import UIKit

class SummaryViewController: UIViewController, MKMapViewDelegate {
    
    @IBOutlet weak var summaryMap: MKMapView!
    var delegate: MapViewController? = nil
    var pinLocations: [MKAnnotation] = []
    
    override func viewDidLoad() {
        super.viewDidLoad()
        summaryMap.delegate = self
        self.preferredContentSize = CGSize(width: 900, height: 700)
        for pin in pinLocations {
            summaryMap.addAnnotation(pin)
        }
        summaryMap.showAnnotations(pinLocations, animated: true)
    }
    
    @IBAction func close(sender: AnyObject) {
        let tmpController = self.presentingViewController;
        self.dismissViewControllerAnimated(true, completion: {()->Void in
            tmpController!.dismissViewControllerAnimated(true, completion: nil);
        });
    }
    
    func mapView(mapView: MKMapView, viewForAnnotation annotation: MKAnnotation) -> MKAnnotationView? {
        if (annotation is MKPointAnnotation) {
            if let pinIcon = self.summaryMap.dequeueReusableAnnotationViewWithIdentifier("pinIcon") {
                return pinIcon
            } else {
                let pinIcon = MKAnnotationView(annotation: annotation, reuseIdentifier: "pinIcon")
                pinIcon.image = UIImage(named: "pin_icon")
                print("HEREERERERERER")
                return pinIcon
            }
        }
        return MKAnnotationView()
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
        }
    }

    
}