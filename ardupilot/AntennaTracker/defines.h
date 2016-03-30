// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
#pragma once

// Command/Waypoint/Location Options Bitmask
//--------------------
#define MASK_OPTIONS_RELATIVE_ALT       (1<<0)          // 1 = Relative
                                                        // altitude

// Controller modes
// ----------------

enum ControlMode {
    MANUAL=0,
    STOP=1,
    SCAN=2,
    SERVO_TEST=3,
    AUTO=10,
    INITIALISING=16
};

enum ServoType {
    SERVO_TYPE_POSITION=0,
    SERVO_TYPE_ONOFF=1,
    SERVO_TYPE_CR=2
};

//  Logging parameters
#define MASK_LOG_ATTITUDE               (1<<0)
#define MASK_LOG_GPS                    (1<<1)
#define MASK_LOG_RCIN                   (1<<2)
#define MASK_LOG_IMU                    (1<<3)
#define MASK_LOG_RCOUT                  (1<<4)
#define MASK_LOG_COMPASS                (1<<5)
#define MASK_LOG_ANY                    0xFFFF
