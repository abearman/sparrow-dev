
//
//  DetailViewController.swift
//  Sparrow
//
//  Created by Andrew Lim on 4/21/16.
//  Copyright © 2016 Andrew Lim. All rights reserved.
//

import UIKit
import Foundation
import MapKit

class DroneViewController: UIViewController, AnalogueStickDelegate {
    
    
    @IBOutlet weak var logo: UIImageView!
    
    @IBOutlet weak var dropPinButton:UIButton!
    
    @IBOutlet weak var launchButton:UIButton!
    @IBOutlet weak var launchButtonBG: UIView!
    
    @IBOutlet weak var coordinateLabel:UILabel!
    
    @IBOutlet weak var altitudeReadingLabel:UILabel!
    
    @IBOutlet weak var analogueStick: AnalogueStick!
    
    @IBOutlet weak var altitudeSlider: UISlider! {
        didSet {
            altitudeSlider.transform = CGAffineTransformMakeRotation(CGFloat(-M_PI_2))
            let thumbImage = UIImage(named:"slider_thumb")
            let ratio : CGFloat = CGFloat (0.33)
            let size = CGSizeMake(thumbImage!.size.width * ratio, thumbImage!.size.height * ratio)
            altitudeSlider.setThumbImage(self.imageWithImage(thumbImage!, scaledToSize: size), forState: UIControlState.Normal)
            // altitudeSlider.setMaximumTrackImage(UIImage(named:"slider_track"), forState: UIControlState.Normal)
            // altitudeSlider.setMinimumTrackImage(UIImage(named:"slider_track"), forState: UIControlState.Normal)
        }
    }
    
    @IBOutlet weak var rotationSlider: UISlider! {
        didSet {
            let thumbImage = UIImage(named:"slider_thumb")
            let ratio : CGFloat = CGFloat (0.33)
            let size = CGSizeMake(thumbImage!.size.width * ratio, thumbImage!.size.height * ratio)
            rotationSlider.setThumbImage(self.imageWithImage(thumbImage!, scaledToSize: size), forState: UIControlState.Normal)

            // rotationSlider.setMaximumTrackImage(UIImage(named:"slider_track"), forState: UIControlState.Normal)
            // rotationSlider.setMinimumTrackImage(UIImage(named:"slider_track"), forState: UIControlState.Normal)
        }
    }
        
    @IBOutlet weak var mapView: MKMapView!
    
    @IBOutlet weak var cameraView: UIView!
    
    var inFlight: Bool = false
    
    // let socket = SocketIOClient(socketURL: NSURL(string: "http://10.34.172.4:5000")!, options: [.Nsp("/control")])
    
    let socket = SocketIOClient(socketURL: NSURL(string: "http://10.34.174.47:5000")!)
    
    
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
        logo.image = UIImage(named: "logo_icon")
        analogueStick.delegate = self
        mapView.layer.borderWidth = 5
        mapView.layer.borderColor = UIColor.blackColor().CGColor
        self.addHandlers()
        debugPrint("Connecting to server control socket...")
        self.socket.connect()
    }
    
    func imageWithImage(image:UIImage, scaledToSize newSize:CGSize) -> UIImage{
        UIGraphicsBeginImageContextWithOptions(newSize, false, 0.0);
        image.drawInRect(CGRectMake(0, 0, newSize.width, newSize.height))
        let newImage:UIImage = UIGraphicsGetImageFromCurrentImageContext()
        UIGraphicsEndImageContext()
        return newImage
    }
    
    func addHandlers() {
        // Our socket handlers go here
        self.socket.onAny {print("Got event: \($0.event), with items: \($0.items)")}
        
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
            HTTPPostJSON("http://10.34.174.47:5000/control/land", jsonObj: []) {
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
            HTTPPostJSON("http://10.34.174.47:5000/control/take_off", jsonObj: []) {
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
    
    @IBAction func altitudeSliderChange(sender: AnyObject) {
        debugPrint("Altitude slider changed to %f", self.altitudeSlider.value)
        let altitudeCommandArgs = [
            "altitude": self.altitudeSlider.value
        ]
        socket.emit("altitude_cmd", altitudeCommandArgs)
    }
    
    @IBAction func altitudeSliderReleased(sender: AnyObject) {
        let initialValue = self.altitudeSlider.value
        let finalValue = Float(0)
        let difference = fabs(initialValue - finalValue)
        let duration = Double(difference / 5.0)
        UIView.animateWithDuration(0.2, delay: 0, options: .CurveLinear, animations: { () -> Void in
            self.altitudeSlider.setValue(initialValue, animated: true)
            }) { (completed) -> Void in
                UIView.animateWithDuration(duration, delay: 0.1, options: .CurveLinear, animations: { () -> Void in
                    self.altitudeSlider.setValue(finalValue, animated: true)
                    }, completion: nil)
        }
    }
    
    @IBAction func rotationSliderChange(sender: AnyObject) {
        debugPrint("Rotation slider changed to %f", self.rotationSlider.value)
        let rotationCommandArgs = [
            "heading": self.rotationSlider.value
        ]
        debugPrint("Emitting rotation_cmd...")
        socket.emit("rotation_cmd", rotationCommandArgs)
    }
    
    @IBAction func rotationSliderReleased(sender: AnyObject) {
        let initialValue = self.rotationSlider.value
        let finalValue = Float(0)
        let difference = fabs(initialValue - finalValue)
        let duration = Double(difference / 5.0)
        UIView.animateWithDuration(0.2, delay: 0, options: .CurveLinear, animations: { () -> Void in
            self.rotationSlider.setValue(initialValue, animated: true)
            }) { (completed) -> Void in
                UIView.animateWithDuration(duration, delay: 0.1, options: .CurveLinear, animations: { () -> Void in
                    self.rotationSlider.setValue(finalValue, animated: true)
                    }, completion: nil)
        }
    }
    
    func analogueStickDidChangeValue(analogueStick: AnalogueStick!) {
        debugPrint("analogue x is: %f", self.analogueStick.xValue)
        debugPrint("analogue y is: %f", self.analogueStick.yValue)
        // TODO: determine duration dynamically
        let lateralCommandArgs = [
            "x_offset": self.analogueStick.xValue,
            "y_offset": self.analogueStick.yValue,
            "duration": 1
        ]
        
        socket.emit("lateral_cmd", lateralCommandArgs)
    }

}

