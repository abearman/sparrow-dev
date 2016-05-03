//
//  WaypointAnnotation.swift
//  Sparrow
//
//  Created by Catherine Dong on 5/3/16.
//  Copyright © 2016 Andrew Lim. All rights reserved.
//

import Foundation
import MapKit

class WaypointAnnotation: NSObject, MKAnnotation {
    let coordinate: CLLocationCoordinate2D
    
    init(coordinate: CLLocationCoordinate2D) {
        self.coordinate = coordinate
        super.init()
    }
    
}
