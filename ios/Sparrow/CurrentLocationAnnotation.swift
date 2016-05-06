//
//  CurrentLocationAnnotation.swift
//  Sparrow
//
//  Created by Pavitra Rengarajan on 4/24/16.
//  Copyright © 2016 Andrew Lim. All rights reserved.
//

import Foundation
import MapKit

class CurrentLocationAnnotation: NSObject, MKAnnotation {
    let coordinate: CLLocationCoordinate2D
    let angle: Double
    
    init(coordinate: CLLocationCoordinate2D, angle: Double) {
        self.coordinate = coordinate
        self.angle = angle
        super.init()
    }
    
}
