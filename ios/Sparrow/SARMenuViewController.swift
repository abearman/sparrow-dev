//
//  SARMenuViewController.swift
//  Sparrow
//
//  Created by Pavitra Rengarajan on 5/2/16.
//  Copyright © 2016 Andrew Lim. All rights reserved.
//

import Foundation
import Darwin
import MapKit
import UIKit

class SARMenuViewController: UITableViewController {
    
    var delegate: MapViewController? = nil
    
    var data = ["Line", "Radial", "Sector"]
    var rowHeight = 40
    
    let PI = 3.141596
    // let STEP : Double = 20
    // let DEGREE_STEP = 0.0002 // 20 m
    // let STEP : Double = 3
    // let DEGREE_STEP = 0.00003
    let RADIAL_OFFSETS : [(dNorth: Double, dEast: Double)] =
        [(0, 0), (1, 0), (1, 1), (-1, 1), (-1, -1), (2, -1), (2, 2), (-2, 2)]
    /* Additional radial points: (-2, -2), (3, -2), (3, 3) */
    let LINE_OFFSETS : [(dNorth: Double, dEast: Double)] =
        [(0, 0), (0, 1), (-4, 1), (-4, 2), (0, 2), (0, 3), (-4, 3), (-4, 4), (0, 4), (0, 5), (-4, 5)]
    let SECTOR_OFFSETS : [(dNorth: Double, dEast: Double)] =
        [(0, 0), (4, 0), (3, -1), (2, 0), (1, 1), (2, 2), (2, 0), (2, -2), (1, -1), (2, 0)]
    
    override func viewDidLoad() {
        super.viewDidLoad()
        let height = data.count * Int(rowHeight) - 1
        self.preferredContentSize = CGSize(width: 300, height: height)
        
    }
    
    // MARK: - Table view data source
    
    override func numberOfSectionsInTableView(tableView: UITableView) -> Int {
        return 1
    }
    
    override func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return data.count
    }
    
    
    override func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCellWithIdentifier("sarCell", forIndexPath: indexPath)
        cell.textLabel!.text = data[indexPath.row]
        
        return cell
    }
    
    override func tableView(tableView: UITableView, heightForRowAtIndexPath indexPath: NSIndexPath) -> CGFloat {
        return CGFloat(rowHeight)
    }
    
    func getLocationMeters(origLat: Double, origLon: Double, dNorth: Double, dEast: Double) -> (Double, Double) {
        let earth_radius : Double = 6378137.0
        let dLat : Double = dNorth / earth_radius
        let dLon : Double = dEast / (earth_radius * cos(PI*origLat/180))
        
        let newLat = origLat + (dLat * 180/PI)
        let newLon = origLon + (dLon * 180/PI)
        return (newLat, newLon)
    }
    
    override func tableView(tableView: UITableView, didSelectRowAtIndexPath indexPath: NSIndexPath) {
        
        self.dismissViewControllerAnimated(true, completion: nil)
        self.delegate!.sarControlView.hidden = false
        
        if indexPath.row == 0 {
            self.delegate!.pathType = "line"
            self.delegate!.offsets = LINE_OFFSETS
        } else if indexPath.row == 1 {
            self.delegate!.pathType = "radial"
            self.delegate!.offsets = RADIAL_OFFSETS
        } else if indexPath.row == 2 {
            self.delegate!.pathType = "sector"
            self.delegate!.offsets = SECTOR_OFFSETS
        }
        
        self.delegate!.drawPreview()
        
    
        
        //        let sarArgs = [
        //            "lat": delegate!.latestLat,
        //            "lon": delegate!.latestLong,
        //            "altitude": delegate!.latestAlt,
        //            "sar_type": sar_type
        //        ]
        //        delegate!.socket.emit("sar_path", sarArgs)
        
        
    }
    
}