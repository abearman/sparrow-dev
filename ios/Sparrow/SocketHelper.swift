//
//  SocketHelper.swift
//  Sparrow
//
//  Created by Amy Bearman on 5/22/16.
//  Copyright © 2016 Andrew Lim. All rights reserved.
//

import Foundation

class SocketHelper {
    
    // The IP address that the server is running on
    
    let HOSTNAME = "10.1.1.188"
    let PORT = "5000"
    
    var states: Dictionary<String, String>?
    
    lazy var socket: SocketIOClient = { [weak self] in
        return SocketIOClient(socketURL: NSURL(string: (self?.buildSocketAddr)!)!)
    }()
    
    lazy var buildSocketAddr: String = { [weak self] in
        return "http://\(self?.HOSTNAME):\(self?.PORT)"
    }()
    
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
    
}