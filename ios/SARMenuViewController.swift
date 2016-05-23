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
    let STEP : Double = 3
    let DEGREE_STEP = 0.00003
    let RADIAL_OFFSETS : [(dNorth: Double, dEast: Double)] =
        [(1, 0), (1, 1), (-1, 1), (-1, -1), (2, -1), (2, 2), (-2, 2), (-2, -2), (3, -2), (3, 3)]
    let LINE_OFFSETS : [(dNorth: Double, dEast: Double)] =
        [(0, 1), (-4, 1), (-4, 2), (0, 2), (0, 3), (-4, 3), (-4, 4), (0, 4), (0, 5), (-4, 5)]
    let SECTOR_OFFSETS : [(dNorth: Double, dEast: Double)] =
        [(4, 0), (3, -1), (2, 0), (1, 1), (2, 2), (2, 0), (2, -2), (1, -1), (2, 0)]
    
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
    
    var sarLocations: [CLLocationCoordinate2D] = []
    var sarPreviewPath: MKPolyline?
    
    override func tableView(tableView: UITableView, didSelectRowAtIndexPath indexPath: NSIndexPath) {
        
        self.dismissViewControllerAnimated(true, completion: nil)
        
        
        //let selectedCell = tableView.cellForRowAtIndexPath(indexPath)!
        var offsets : [(dNorth: Double, dEast: Double)] = []
        var sar_type : String = ""
        if indexPath.row == 0 {
            sar_type = "line"
            offsets = LINE_OFFSETS
        } else if indexPath.row == 1 {
            sar_type = "radial"
            offsets = RADIAL_OFFSETS
        } else if indexPath.row == 2 {
            sar_type = "sector"
            offsets = SECTOR_OFFSETS
        }
        print(sar_type)
        
        let originalLat = self.delegate!.latestLat
        let originalLong = self.delegate!.latestLong
        for offset in offsets {
            let nextWaypoint = (originalLat + offset.dNorth * self.DEGREE_STEP,
                                originalLong + offset.dEast * self.DEGREE_STEP)
            let loc = CLLocationCoordinate2DMake(nextWaypoint.0, nextWaypoint.1)
            sarLocations.append(loc)
        }
        sarPreviewPath = MKPolyline(coordinates: &sarLocations, count: sarLocations.count)
        self.delegate?.mapView.addOverlay(sarPreviewPath!)
        
        let delayTime = dispatch_time(DISPATCH_TIME_NOW, Int64(3 * Double(NSEC_PER_SEC)))
        dispatch_after(delayTime, dispatch_get_main_queue()) {
            self.delegate?.mapView.removeOverlay(self.sarPreviewPath!)
            
            let priority = DISPATCH_QUEUE_PRIORITY_DEFAULT
            dispatch_async(dispatch_get_global_queue(priority, 0)) {
                let originalLat = self.delegate!.latestLat
                let originalLong = self.delegate!.latestLong
                for offset in offsets {
                    // let nextWaypoint = self.getLocationMeters(self.delegate!.latestLat, origLon: self.delegate!.latestLong, dNorth: Double(offset.dNorth * self.STEP), dEast: Double(offset.dEast * self.STEP))
                    let nextWaypoint = (originalLat + offset.dNorth * self.DEGREE_STEP,
                                        originalLong + offset.dEast * self.DEGREE_STEP)
                    let waypointArgs = [
                        "lat": nextWaypoint.0,
                        "lon": nextWaypoint.1
                    ]
                    self.delegate!.socket.emit("waypoint_cmd", waypointArgs)
                    let distance = sqrt(pow(Double(offset.dNorth * self.STEP), 2) + pow(Double(offset.dEast * self.STEP), 2))
                    sleep(UInt32(distance / 0.5)) // change back to 2
                }
                print("FINISHED SAR PATH")
            }
            
        }
        
        
        
        
        //        let sarArgs = [
        //            "lat": delegate!.latestLat,
        //            "lon": delegate!.latestLong,
        //            "altitude": delegate!.latestAlt,
        //            "sar_type": sar_type
        //        ]
        //        delegate!.socket.emit("sar_path", sarArgs)
        
        
    }
    
}