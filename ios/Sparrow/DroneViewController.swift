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

class DroneViewController: UIViewController, AnalogueStickDelegate, MKMapViewDelegate, UIPopoverPresentationControllerDelegate {
    
    @IBOutlet weak var mapView: MKMapView!
    @IBOutlet weak var cameraView: UIView!
    @IBOutlet weak var videoImage: UIImageView!
    @IBOutlet weak var dropPinButton:UIButton!
    @IBOutlet weak var sarPathButton: UIButton!
    @IBOutlet weak var launchButton:UIButton!
    @IBOutlet weak var coordinateLabel:UILabel!
    @IBOutlet weak var altitudeReadingLabel:UILabel!
    @IBOutlet weak var analogueStick: AnalogueStick!
    @IBOutlet weak var altitudeSlider: UISlider! {
        didSet {
            altitudeSlider.transform = CGAffineTransformMakeRotation(CGFloat(-M_PI_2))
            let thumbImage = UIImage(named:"slider_thumb")
            let ratio : CGFloat = CGFloat (0.8)
            let size = CGSizeMake(thumbImage!.size.width * ratio, thumbImage!.size.height * ratio)
            altitudeSlider.setThumbImage(self.imageWithImage(thumbImage!, scaledToSize: size), forState: UIControlState.Normal)
            altitudeSlider.setMaximumTrackImage(UIImage(named:"slider_track"), forState: UIControlState.Normal)
            altitudeSlider.setMinimumTrackImage(UIImage(named:"slider_track"), forState: UIControlState.Normal)
            altitudeSlider.continuous = false
        }
    }
    @IBOutlet weak var rotationSlider: UISlider! {
        didSet {
            let thumbImage = UIImage(named:"slider_thumb")
            let ratio : CGFloat = CGFloat (0.8)
            let size = CGSizeMake(thumbImage!.size.width * ratio, thumbImage!.size.height * ratio)
            rotationSlider.setThumbImage(self.imageWithImage(thumbImage!, scaledToSize: size), forState: UIControlState.Normal)
            rotationSlider.setMaximumTrackImage(UIImage(named:"slider_track"), forState: UIControlState.Normal)
            rotationSlider.setMinimumTrackImage(UIImage(named:"slider_track"), forState: UIControlState.Normal)
            rotationSlider.continuous = false
        }
    }
    
    
// TODO: Are these variables necessary?
//    weak var delegate: AnalogueStickDelegate?
//    var detailItem: AnyObject? {
//        didSet {
//            // Update the view.
//            self.configureView()
//        }
//    }

    override func viewDidLoad() {
        super.viewDidLoad()
        
        analogueStick.delegate = self
        
        mapView.delegate = self
        mapView.layer.borderWidth = 2
        mapView.layer.borderColor = UIColor.darkGrayColor().CGColor
        let longPressRec = UILongPressGestureRecognizer(target: self, action: #selector(DroneViewController.dropWaypoint(_:)))
        self.mapView.addGestureRecognizer(longPressRec)
        // TODO: remove dummy initial location below
        let startLoc = CLLocationCoordinate2DMake(37.430020, -122.173302)
        onLocationUpdate(startLoc)
        
        // on load set a default zoom
        let region = MKCoordinateRegionMake(startLoc, MKCoordinateSpanMake(0.01, 0.01))
        self.mapView.setRegion(region, animated: true)

        debugPrint("Connecting to server control socket...")
        
        // Creating the socket
        initializeSocket()
    }

    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
    }
    
    
    
// =================================== SERVER ===================================
    
    // The IP address that the server is running on
    let HOSTNAME = "10.34.165.75"
    //let HOSTNAME = "10.1.1.188"
    let PORT = "5000"
    
    
    private var buildSocketAddr: String {
        get {
            return "http://" + HOSTNAME + ":" + PORT
        }
    }
    
    var socket = SocketIOClient(socketURL: NSURL(string: "")!)
    
    func initializeSocket() {
        socket = SocketIOClient(socketURL: NSURL(string: buildSocketAddr)!)
        socket.onAny {print("Got event: \($0.event), with items: \($0.items)")}
        
        // constant fetching for latest GPS coordinates
        socket.on("gps_pos_ack") {[weak self] data, ack in
            debugPrint("received gps_pos_ack event")
            self?.handleGPSPos(data)
            return
        }
        
        // video frame rendering
        self.socket.on("ios_frame") {[weak self] data, ack in
            debugPrint("received ios_frame event")
            self?.handleFrame(data)
            return
        }
        
        //socket.joinNamespace("/control")
        socket.connect()
    }
    
    
    func handleGPSPos(data: AnyObject) {

        debugPrint("in handleGPSPos")
        
        // Unpack JSON data
        let encodedData = data[0].dataUsingEncoding(NSUTF8StringEncoding, allowLossyConversion: false)!
        do {
            if let jsonData: AnyObject = try NSJSONSerialization.JSONObjectWithData(encodedData, options: .AllowFragments) {
                let latitude = jsonData["lat"] as? Double
                let longitude = jsonData["lon"] as? Double
                let altitude = jsonData["alt"] as? Double
                debugPrint("got latitude: ", latitude)
                debugPrint("got longitude: ", longitude)
                debugPrint("got altitude: ", altitude)
                latestLat = latitude!
                latestLong = longitude!
                latestAlt = altitude!
                
                // Update coordinate label
                coordinateLabel.text = "(" + String(format: "%.3f", latitude!) + "N, " + String(format: "%.3f", longitude!) + "W)"
                altitudeReadingLabel.text = String(format: "%.3f", altitude!) + " m"
                
                // Create CLLocation
                let loc = CLLocationCoordinate2DMake(latitude!, longitude!)
                
                // Call onLocationUpdate
                onLocationUpdate(loc)
            }
            
        } catch {
            print("error serializing JSON: \(error)")

        }
    }
    
    struct PixelData {
        var a: UInt8 = 0
        var r: UInt8 = 0
        var g: UInt8 = 0
        var b: UInt8 = 0
    }
    
    func handleFrame(data: AnyObject) {
        
        debugPrint("in handleFrame")
        
        // Unpack JSON data
        let encoded = data[0]["image"] as! String
        let image_data = NSData(base64EncodedString: encoded, options: NSDataBase64DecodingOptions(rawValue: 0))
        var compressed_image_bytes = Array(UnsafeBufferPointer(start: UnsafePointer<UInt8>(image_data!.bytes), count: image_data!.length))
        
        let decompressed_data : NSData = try! image_data!.gunzippedData()
        var image_bytes = Array(UnsafeBufferPointer(start: UnsafePointer<UInt8>(decompressed_data.bytes), count: decompressed_data.length))
        
        let count = 921600
        
        debugPrint("Data length received: ", image_data!.length)
        debugPrint("Pixel count: ", count)
        
        
        // Create pixel data array
        var pixel_data = [PixelData](count: count, repeatedValue: PixelData())
        
        for i in 0 ..< count {
            pixel_data[i].a = 255
            pixel_data[i].r = image_bytes[3 * i]
            pixel_data[i].g = image_bytes[3 * i + 1]
            pixel_data[i].b = image_bytes[3 * i + 2]
        }
        
        let width = 1280
        let height = 720
        let bitmapCount: Int = pixel_data.count
        let elementLength: Int = sizeof(PixelData)
        let render: CGColorRenderingIntent = CGColorRenderingIntent.RenderingIntentDefault
        let rgbColorSpace = CGColorSpaceCreateDeviceRGB()
        let bitmapInfo: CGBitmapInfo = CGBitmapInfo(rawValue: CGImageAlphaInfo.PremultipliedFirst.rawValue)
        let providerRef: CGDataProvider? = CGDataProviderCreateWithCFData(NSData(bytes: &pixel_data, length: bitmapCount * elementLength))
        let cgimage: CGImage? = CGImageCreate(width, height, 8, 32, width * elementLength, rgbColorSpace, bitmapInfo, providerRef, nil, true, render)
        
        if cgimage != nil {
            debugPrint("Generated image")
            let new_image = UIImage(CGImage: cgimage!)
            self.videoImage.image =  new_image
        }
    }
        
    func imageWithImage(image:UIImage, scaledToSize newSize:CGSize) -> UIImage{
        UIGraphicsBeginImageContextWithOptions(newSize, false, 0.0);
        image.drawInRect(CGRectMake(0, 0, newSize.width, newSize.height))
        let newImage:UIImage = UIGraphicsGetImageFromCurrentImageContext()
        UIGraphicsEndImageContext()
        return newImage
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
    

    
// =================================== MOVEMENT CONTROL ===================================
    
    var isInFlight: Bool = false    // Same as !isLanded
    var isLanding: Bool = false
    var isTakingOff: Bool = false
    
    @IBAction func launchButtonClicked(sender: AnyObject) {
        // Takeoff command
        if (!isInFlight) {
            debugPrint("Sending take off request")
            isInFlight = true
            isTakingOff = true
            updateLaunchButton()  // Update button to gray, "Taking Off"
            HTTPPostJSON(buildSocketAddr + "/control/take_off", jsonObj: []) {
                (data: String, error: String?) -> Void in
                if error != nil {
                    print(error)
                } else {  // Done taking off
                    print("data is : \n\n\n")
                    debugPrint("Take off request completed.")
                    print(data)
                    self.isTakingOff = false
                    self.updateLaunchButton()  // Update button to reenable red, "LAND"
                }
            }
            
        // Land command
        } else {
            debugPrint("Sending land request")
            isLanding = true
            updateLaunchButton()  // Update button to gray, "Landing"
            HTTPPostJSON(buildSocketAddr + "/control/land", jsonObj: []) {
                (data: String, error: String?) -> Void in
                if error != nil {
                    print(error)
                } else {  // Done landing
                    print("data is : \n\n\n")
                    debugPrint("Land request completed.")
                    print(data)
                    self.isLanding = false
                    self.isInFlight = false
                    self.updateLaunchButton()  // Update button to reenable green, "LAUNCH"
                }
            }
        }
    }
    
    func updateLaunchButton() {
        if (!isInFlight && !isTakingOff && !isLanding) {  // On ground, waiting for launch
            launchButton.setTitle("LAUNCH", forState: UIControlState.Normal)
            launchButton.backgroundColor = UIColor.greenColor()
        } else if (isInFlight && isTakingOff && !isLanding) {  // Taking off
            launchButton.setTitle("TAKING OFF", forState: UIControlState.Normal)
            launchButton.backgroundColor = UIColor.lightGrayColor()
        } else if (isInFlight && !isTakingOff && !isLanding) {  // In flight, waiting for land
            launchButton.setTitle("LAND", forState: UIControlState.Normal)
            launchButton.backgroundColor = UIColor.redColor()
        } else if (isInFlight && !isTakingOff && isLanding) {  // Landing
            launchButton.setTitle("LANDING", forState: UIControlState.Normal)
            launchButton.backgroundColor = UIColor.lightGrayColor()
        }
            
        /*if (inFlight) {
            if (isLanding) {
                launchButton.setTitle("LANDING", forState: UIControlState.Normal)
                launchButton.backgroundColor = UIColor.lightGrayColor()
            } else {
                launchButton.setTitle("LAND", forState: UIControlState.Normal)
                launchButton.backgroundColor = UIColor.redColor()
            }
        } else {
            launchButton.setTitle("LAUNCH", forState: UIControlState.Normal)
            launchButton.backgroundColor = UIColor.greenColor()
        }*/
    }

    @IBAction func altitudeSliderChange(sender: AnyObject) {
        debugPrint("Altitude slider changed to %f", self.altitudeSlider.value)
        let altitudeCommandArgs = [
            "dalt": self.altitudeSlider.value
        ]
        socket.emit("altitude_cmd", altitudeCommandArgs)
    }  
    
    @IBAction func altitudeSliderReleased(sender: AnyObject) {
        let initialValue = self.altitudeSlider.value
        let finalValue = Float(0)
        UIView.animateWithDuration(0.15, delay: 0, options: .CurveLinear, animations: { () -> Void in
            self.altitudeSlider.setValue(initialValue, animated: true)
            }) { (completed) -> Void in
                UIView.animateWithDuration(0.15, delay: 0, options: .CurveLinear, animations: { () -> Void in
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
        UIView.animateWithDuration(0.15, delay: 0, options: .CurveLinear, animations: { () -> Void in
            self.rotationSlider.setValue(initialValue, animated: true)
            }) { (completed) -> Void in
                UIView.animateWithDuration(0.15, delay: 0.1, options: .CurveLinear, animations: { () -> Void in
                    self.rotationSlider.setValue(finalValue, animated: true)
                    }, completion: nil)
        }
    }
    
    func analogueStickDidChangeValue(analogueStick: AnalogueStick!) {
        debugPrint("analogue x is: %f", self.analogueStick.xValue)
        debugPrint("analogue y is: %f", self.analogueStick.yValue)
        // TODO: determine duration dynamically
        /*let lateralCommandArgs = [
            "x_offset": self.analogueStick.xValue,
            "y_offset": self.analogueStick.yValue,
            "duration": 1
        ]
        
        //socket.emit("lateral_cmd", lateralCommandArgs)
         */
    }
    
    @IBAction func lateralButtonDown(sender: AnyObject) {
        if let button = sender as? UIButton {
            if let buttonID = button.restorationIdentifier {
                debugPrint("Lateral button %@ pressed", buttonID)
                let lateralCommandArgs = [
                    "direction": buttonID
                ]
                socket.emit("lateral_cmd", lateralCommandArgs)
            }
        }
    }
    
    @IBAction func lateralButtonUp(sender: AnyObject) {
        debugPrint("Lateral button released")
        let lateralCommandArgs = [
            "direction": "stop"
        ]
        socket.emit("lateral_cmd", lateralCommandArgs)
    }
    
    
    
// =================================== MAP VIEW ===================================
    
    var locations: [CLLocationCoordinate2D] = []
    var path: MKPolyline?
    var marker: MKAnnotation?
    var pinLocations: [MKAnnotation] = []
    
    var latestLat: Double = 0.0
    var latestLong: Double = 0.0
    var latestAlt: Double = 0.0

    func onLocationUpdate(newLoc: CLLocationCoordinate2D) {
        
        debugPrint("in onLocationUpdate")
        
        self.locations.append(newLoc)
        
        drawMarker(newLoc)
        
        // Center curent location in map view. May be annoying when user is trying to
        // scroll to a different part of the map (TODO).
        //let region = MKCoordinateRegionMake(newLoc, MKCoordinateSpanMake(0.01, 0.01))
        //self.mapView.setRegion(region, animated: true)
        
        drawPath()
    }
    
    @IBAction func dropPinButtonClicked(sender: AnyObject) {
        if let loc = self.locations.last {
            if (self.pinLocations.count > 0 &&
                loc.latitude == self.pinLocations.last!.coordinate.latitude &&
                loc.longitude == self.pinLocations.last!.coordinate.longitude
                ) {
                return;
            }
            let pin = MKPointAnnotation()
            pin.coordinate = loc
            self.pinLocations.append(pin)
            self.mapView.addAnnotation(pin)
        }
    }
    
    func dropWaypoint(gestureRecognizer: UILongPressGestureRecognizer) {
        if (gestureRecognizer.state != UIGestureRecognizerState.Began) {
            return;
        }
        
        let touchPoint = gestureRecognizer.locationInView(self.mapView)
        let loc = self.mapView.convertPoint(touchPoint, toCoordinateFromView: self.mapView)
        let waypoint = WaypointAnnotation(coordinate: loc)
        self.mapView.addAnnotation(waypoint)
        let waypointArgs = [
            "lat": loc.latitude,
            "lon": loc.longitude
        ]
        socket.emit("waypoint_cmd", waypointArgs)
    }

    func drawMarker(coordinate: CLLocationCoordinate2D) {
        if (marker != nil) {
            mapView.removeAnnotation(marker!)
        }
        marker = CurrentLocationAnnotation(coordinate: coordinate)
        mapView.addAnnotation(marker!)
    }

    func drawPath() {
        if (locations.count == 1) {
            return
        }
        if (self.path != nil) {
            self.mapView.removeOverlay(path!)
        }
        self.path = MKPolyline(coordinates: &locations, count: locations.count)
        self.mapView.addOverlay(self.path!)
    }
    
    func mapView(mapView: MKMapView, viewForAnnotation annotation: MKAnnotation) -> MKAnnotationView? {
        if (annotation is CurrentLocationAnnotation) {
            if let currentLocIcon = self.mapView.dequeueReusableAnnotationViewWithIdentifier("currentLocIcon") {
                return currentLocIcon
            } else {
                let currentLocIcon = MKAnnotationView(annotation: annotation, reuseIdentifier: "currentLocIcon")
                currentLocIcon.image = UIImage(named: "current_location_icon")
                return currentLocIcon
            }
        }
        else if (annotation is WaypointAnnotation) {
            if let waypointIcon = self.mapView.dequeueReusableAnnotationViewWithIdentifier("waypointIcon") {
                return waypointIcon
            } else {
                let waypointIcon = MKAnnotationView(annotation: annotation, reuseIdentifier: "waypointIcon")
                waypointIcon.image = UIImage(named: "waypoint_icon")
                return waypointIcon
            }
        }
        else if (annotation is MKPointAnnotation) {
            if let pinIcon = self.mapView.dequeueReusableAnnotationViewWithIdentifier("pinIcon") {
                return pinIcon
            } else {
                let pinIcon = MKAnnotationView(annotation: annotation, reuseIdentifier: "pinIcon")
                pinIcon.image = UIImage(named: "pin_icon")
                return pinIcon
            }
        }
        return MKAnnotationView()
    }
    
    func mapView(mapView: MKMapView, rendererForOverlay overlay: MKOverlay) -> MKOverlayRenderer {
        if (overlay is MKPolyline) {
            let pathRenderer = MKPolylineRenderer(overlay: overlay)
            pathRenderer.strokeColor = UIColor.blueColor()
            pathRenderer.lineWidth = 2.0
            return pathRenderer
        } else if (overlay is MKCircle) {
            let circleRenderer = MKCircleRenderer(overlay: overlay)
            circleRenderer.fillColor = UIColor.blueColor()
            return circleRenderer
        }
        return MKPolylineRenderer();
    }
    
    func mapView(mapView: MKMapView, didAddAnnotationViews views: [MKAnnotationView]) {
        for view in views {
            if (view.reuseIdentifier == "pinIcon") {
                let endFrame = view.frame
                view.frame = CGRectMake(view.frame.origin.x, view.frame.origin.y - self.view.frame.size.height, view.frame.size.width, view.frame.size.height);
                UIView.animateWithDuration(
                    0.5, delay: 0.0, options: UIViewAnimationOptions.CurveEaseIn,
                    animations:{() in
                        view.frame = endFrame
                    },
                    completion:{(Bool) in
                        UIView.animateWithDuration(0.05, delay: 0.0, options: UIViewAnimationOptions.CurveEaseInOut, animations:{() in
                            view.transform = CGAffineTransformMakeScale(1.0, 0.8)
                            },
                            completion: {(Bool) in
                                UIView.animateWithDuration(0.3, delay: 0.0, options: UIViewAnimationOptions.CurveEaseInOut, animations:{() in
                                    view.transform = CGAffineTransformIdentity
                                    },
                                    completion: nil)
                        })
                    }
                )
            }
            else if (view.reuseIdentifier == "waypointIcon") {
                view.transform = CGAffineTransformMakeScale(1.5, 1.5)
                UIView.animateWithDuration(0.3, animations: {
                    view.transform = CGAffineTransformIdentity
                })
            }
        }
    }

    
    
// =================================== SAR PATHS ===================================
    
    @IBAction func sarPathButtonClicked(sender: AnyObject) {
        print("SAR PATH BUTTON CLICKED");
    }
    
    func adaptivePresentationStyleForPresentationController(controller: UIPresentationController) -> UIModalPresentationStyle {
        return UIModalPresentationStyle.FullScreen
        //        return UIModalPresentationStyle.None
    }
    
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        if segue.identifier == "popoverSegue" {
            let popoverViewController = segue.destinationViewController as UIViewController
            popoverViewController.modalPresentationStyle = UIModalPresentationStyle.Popover
            popoverViewController.popoverPresentationController!.delegate = self
            let popoverTableViewController = segue.destinationViewController as! SARMenuViewController
            popoverTableViewController.delegate = self
            segue.destinationViewController.popoverPresentationController!.sourceView = sender as! UIButton
            segue.destinationViewController.popoverPresentationController!.sourceRect = (sender as! UIButton).bounds
        }
    }
    
}
