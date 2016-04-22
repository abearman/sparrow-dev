//
//  JSONHelper.swift
//  Sparrow
//
//  Created by Andrew Lim on 4/21/16.
//  Copyright © 2016 Andrew Lim. All rights reserved.
//

import Foundation

func JSONStringify(value: AnyObject,prettyPrinted:Bool = false) -> String{
    
    let options = prettyPrinted ? NSJSONWritingOptions.PrettyPrinted : NSJSONWritingOptions(rawValue: 0)
    
    
    if NSJSONSerialization.isValidJSONObject(value) {
        
        do{
            let data = try NSJSONSerialization.dataWithJSONObject(value, options: options)
            if let string = NSString(data: data, encoding: NSUTF8StringEncoding) {
                return string as String
            }
        }catch {
            
            print("error")
            //Access error here
        }
        
    }
    return ""
    
}