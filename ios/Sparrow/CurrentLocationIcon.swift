//
//  CurrentLocationIcon.swift
//  Sparrow
//
//  Created by Pavitra Rengarajan on 4/24/16.
//  Copyright © 2016 Andrew Lim. All rights reserved.
//

import Foundation
import MapKit

class CurrentLocationIcon: NSObject, MKAnnotation {
    let coordinate: CLLocationCoordinate2D
    
    init(coordinate: CLLocationCoordinate2D) {
        self.coordinate = coordinate
        super.init()
    }
    
}
