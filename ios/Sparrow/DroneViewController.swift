//
//  DroneViewController.swift
//  Sparrow
//
//  Created by Amy Bearman on 5/22/16.
//  Copyright © 2016 Andrew Lim. All rights reserved.
//

import UIKit

class DroneViewController: UIViewController {
    
    // The IP address that the server is running on
    
    let HOSTNAME = "10.30.113.50"
    let PORT = "5000"
    
    var states: Dictionary<String, String>?
    
    lazy var buildSocketAddr: String = {
        "http://\(self.HOSTNAME):\(self.PORT)"
    }()
    
    lazy var socket: SocketIOClient = {
        return SocketIOClient(socketURL: NSURL(string: self.buildSocketAddr)!)
    }()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        debugPrint("Connecting to server control socket...")
        initializeSocket()
        connectToSocket()
    }
    
    func initializeSocket() {
        socket.onAny {print("Got event: \($0.event), with items: \($0.items)")}
    }

    func connectToSocket() {
        socket.connect()
    }

    var latestLat: Double = 0.0
    var latestLong: Double = 0.0
    var latestAlt: Double = 0.0
    var latestYaw: Double = 0.0
    
    func handleGPSPos(data: AnyObject) {
        debugPrint("in handleGPSPos")
        
        if let jsonData = processJSONData(data) {
            if let latitude = jsonData["lat"] as? Double {
                latestLat = latitude
            }
            if let longitude = jsonData["lon"] as? Double {
                latestLong = longitude
            }
            if let altitude = jsonData["alt"] as? Double {
                latestAlt = altitude
            }
            if let yaw = jsonData["yaw"] as? Double {
                latestYaw = yaw
            }
        }
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
    
    func processJSONData(data: AnyObject) -> AnyObject? {
        // Try to unpack JSON data
        if let arrData = data as? [AnyObject] {
            if !arrData.isEmpty {
                if let dataString = arrData[0] as? String {
                    if let encodedData = dataString.dataUsingEncoding(NSUTF8StringEncoding, allowLossyConversion: false) {
                        do {
                            if let jsonData: AnyObject = try NSJSONSerialization.JSONObjectWithData(encodedData, options: .AllowFragments) {
                                return jsonData
                            }
                        } catch {
                            print("error serializing JSON: \(error)")
                            
                        }
                    }
                }
            }
        }
        return nil
    }
    
    func imageWithImage(image:UIImage, scaledToSize newSize:CGSize) -> UIImage{
        UIGraphicsBeginImageContextWithOptions(newSize, false, 0.0);
        image.drawInRect(CGRectMake(0, 0, newSize.width, newSize.height))
        let newImage:UIImage = UIGraphicsGetImageFromCurrentImageContext()
        UIGraphicsEndImageContext()
        return newImage
    }
    
}
