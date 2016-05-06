//
//  SARMenuViewController.swift
//  Sparrow
//
//  Created by Pavitra Rengarajan on 5/2/16.
//  Copyright © 2016 Andrew Lim. All rights reserved.
//

import Foundation

class SARMenuViewController: UITableViewController {
    
    var delegate: DroneViewController? = nil
    
    var data = ["Line", "Radial", "Sector"]
    var rowHeight = 40
    
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
    
    override func tableView(tableView: UITableView, didSelectRowAtIndexPath indexPath: NSIndexPath) {
        //let selectedCell = tableView.cellForRowAtIndexPath(indexPath)!
        var sar_type : String = ""
        if indexPath.row == 0 {
            sar_type = "line"
        } else if indexPath.row == 1 {
            sar_type = "radial"
        } else if indexPath.row == 2 {
            sar_type = "sector"
        }
        print(sar_type)
        let sarArgs = [
            "lat": delegate!.latestLat,
            "lon": delegate!.latestLong,
            "altitude": delegate!.latestAlt,
            "sar_type": sar_type
        ]
        delegate!.socket.emit("sar_path", sarArgs)
        self.dismissViewControllerAnimated(true, completion: nil)
    }

}