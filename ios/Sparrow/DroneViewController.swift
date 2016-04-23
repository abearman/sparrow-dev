//
//  DetailViewController.swift
//  Sparrow
//
//  Created by Andrew Lim on 4/21/16.
//  Copyright © 2016 Andrew Lim. All rights reserved.
//

import UIKit
import Foundation

class DroneViewController: UIViewController, AnalogueStickDelegate {
    
    @IBOutlet weak var dropPinButton:UIButton!
    
    @IBOutlet weak var launchButton:UIButton!
    @IBOutlet weak var launchButtonBG: UIView!
    
    @IBOutlet weak var coordinateLabel:UILabel!
    
    @IBOutlet weak var altitudeReadingLabel:UILabel!
    
    @IBOutlet weak var analogueStick: AnalogueStick!
    
    var inFlight: Bool = false
    
    weak var socket: SocketIOClient!
    
    weak var delegate: AnalogueStickDelegate?
    
    var detailItem: AnyObject? {
        didSet {
            // Update the view.
            self.configureView()
        }
    }

    func configureView() {}

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        self.configureView()
        analogueStick.delegate = self
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
    
    func updateLaunchButton() {
        if (self.inFlight) {
            launchButton.setTitle("Land", forState: UIControlState.Normal)
            launchButtonBG.backgroundColor = UIColor(red: 255, green: 0, blue: 0, alpha: 1.0)
        } else {
            launchButton.setTitle("Launch", forState: UIControlState.Normal)
            launchButtonBG.backgroundColor = UIColor(red: 0, green: 255, blue: 0, alpha: 1.0)
        }
    }
    
    
    @IBAction func launchButtonClicked(sender: AnyObject) {
        if (self.inFlight) {
            debugPrint("Sending land request")
            HTTPPostJSON("http://192.168.2.116:5000/control/land", jsonObj: []) {
                    (data: String, error: String?) -> Void in
                    if error != nil {
                        print(error)
                    } else {
                        print("data is : \n\n\n")
                        debugPrint("Land request completed.")
                        print(data)
                    }
            }
            self.inFlight = false
        } else {
            debugPrint("Sending take off request")
            HTTPPostJSON("http://192.168.2.116:5000/control/take_off", jsonObj: []) {
                    (data: String, error: String?) -> Void in
                    if error != nil {
                        print(error)
                    } else {
                        print("data is : \n\n\n")
                        debugPrint("Take off request completed.")
                        print(data)
                    }
            }
            self.inFlight = true
        }
        self.updateLaunchButton()
    }
    
    
    @IBAction func dropPinButtonClicked(sender: AnyObject) {
        /* TODO: drop pin on map */
    }
    
    
    func analogueStickDidChangeValue(analogueStick: AnalogueStick!) {
        debugPrint("analogue x is: %f", self.analogueStick.xValue)
        debugPrint("analogue y is: %f", self.analogueStick.yValue)
        
    }

}

