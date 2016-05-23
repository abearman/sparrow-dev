//
//  VideoFeedViewController.swift
//  Sparrow
//
//  Created by Amy Bearman on 5/22/16.
//  Copyright © 2016 Andrew Lim. All rights reserved.
//

import UIKit

class VideoFeedViewController: DroneViewController {
    
    @IBOutlet weak var videoImage: UIImageView!

    override func viewDidLoad() {
        disableWaypointButtons()
        
        debugPrint("Connecting to server control socket...")
        initializeSocket()
        connectToSocket()
    }
    
    // =================================== UPDATE CONTROL BAR ===================================
    
    func disableWaypointButtons() {
        for childVC in self.childViewControllers {
            if let controlBarVC = childVC as? ControlBarViewController {
                disableButton(controlBarVC.waypointButton)
                disableButton(controlBarVC.sarButton)
                disableButton(controlBarVC.dropPinButton)
            }
        }
    }
    
    func disableButton(button: UIButton!) {
        button.enabled = false
        button.alpha = 0.5
    }
    
    // =================================== SERVER ===================================
    
    override func initializeSocket() {
        super.initializeSocket()
        
        // video frame rendering
        socket.on("ios_frame") {[weak self] data, ack in
            debugPrint("received ios_frame event")
            self?.handleFrame(data)
            //            let priority = DISPATCH_QUEUE_PRIORITY_DEFAULT
            //            dispatch_async(dispatch_get_global_queue(priority, 0)) {
            //                self?.handleFrame(data)
            //            }
            return
        }
    }
    
    // =================================== HANDLE IMAGE DATA ===================================
    
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
        //var compressed_image_bytes = Array(UnsafeBufferPointer(start: UnsafePointer<UInt8>(image_data!.bytes), count: image_data!.length))
        
        let decompressed_data : NSData = try! image_data!.gunzippedData()
        var image_bytes = Array(UnsafeBufferPointer(start: UnsafePointer<UInt8>(decompressed_data.bytes), count: decompressed_data.length))
        
        // let count = 921600
        let count = 36864
        
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
        //let width = 256
        //let height = 144
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
        debugPrint("Finished processing image")
    }
    
}
