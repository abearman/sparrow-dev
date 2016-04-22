//
//  DetailViewController.swift
//  Sparrow
//
//  Created by Andrew Lim on 4/21/16.
//  Copyright © 2016 Andrew Lim. All rights reserved.
//

import UIKit
import Foundation

class DroneViewController: UIViewController {

    @IBOutlet weak var titleLabel:UILabel!
    
    @IBOutlet weak var joystick: Joystick!

    @IBOutlet weak var takeOffButton: UIButton!
    @IBOutlet weak var landButton: UIButton!
    
    @IBOutlet weak var altitudeTitle: UILabel!
    @IBOutlet weak var upButton: UIButton!
    @IBOutlet weak var downButton: UIButton!
    @IBOutlet weak var altitudeLabel: UILabel!
    
    
    
    var detailItem: AnyObject? {
        didSet {
            // Update the view.
            self.configureView()
        }
    }

    func configureView() {
        // Update the user interface for the detail item.
        if let detail = self.detailItem {
            if let label = self.titleLabel {
                label.text = detail.valueForKey("timeStamp")!.description
            }
        }
    }

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        self.configureView()
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    func HTTPsendRequest(request: NSMutableURLRequest, callback: ((String, String?) -> Void)!) {
        let task = NSURLSession.sharedSession().dataTaskWithRequest(request,completionHandler :
            {
                data, response, error in
                if error != nil {
                    callback("", (error!.localizedDescription) as String)
                } else {
                    callback(NSString(data: data!, encoding: NSUTF8StringEncoding) as! String,nil)
                }
        })
        
        task.resume() //Tasks are called with .resume()
    }
    
    func HTTPPostJSON(url: String, jsonObj: AnyObject,
        callback: (String, String?) -> Void) {
            let request = NSMutableURLRequest(URL: NSURL(string: url)!)
            request.HTTPMethod = "POST"
            request.addValue("application/json",
                forHTTPHeaderField: "Content-Type")
            let jsonString = JSONStringify(jsonObj)
            let data: NSData = jsonString.dataUsingEncoding(
                NSUTF8StringEncoding)!
            request.HTTPBody = data
            HTTPsendRequest(request,callback: callback)
    }
    
    
    @IBAction func landButtonClicked(sender: AnyObject) {
        debugPrint("Sending land request")
        HTTPPostJSON("http://10.34.161.233:5000/control/land", jsonObj: [])
            {
                (data: String, error: String?) -> Void in
                if error != nil {
                    print(error)
                } else {
                    print("data is : \n\n\n")
                    debugPrint("Land request completed.")
                    print(data)
                }
        }
    }
    
    @IBAction func takeOffButtonClicked(sender: AnyObject) {
        debugPrint("Sending take off request")
        HTTPPostJSON("http://10.34.161.233:5000/control/take_off", jsonObj: [])
            {
                (data: String, error: String?) -> Void in
                if error != nil {
                    print(error)
                } else {
                    print("data is : \n\n\n")
                    debugPrint("Take off request completed.")
                    print(data)
                }
        }
    }

}

