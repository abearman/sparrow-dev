// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
#pragma once

#include <AP_Common/AP_Common.h>
#include <AP_Math/AP_Math.h>        // ArduPilot Mega Vector/Matrix math Library
#include <AP_Notify/AP_Notify.h>      // Notify library
#include <RC_Channel/RC_Channel.h>     // RC Channel Library
#include <Filter/Filter.h>         // filter library

// offsets for motors in motor_out and _motor_filtered arrays
#define AP_MOTORS_MOT_1 0U
#define AP_MOTORS_MOT_2 1U
#define AP_MOTORS_MOT_3 2U
#define AP_MOTORS_MOT_4 3U
#define AP_MOTORS_MOT_5 4U
#define AP_MOTORS_MOT_6 5U
#define AP_MOTORS_MOT_7 6U
#define AP_MOTORS_MOT_8 7U

#define AP_MOTORS_MAX_NUM_MOTORS 8

// frame definitions
#define AP_MOTORS_PLUS_FRAME        0
#define AP_MOTORS_X_FRAME           1
#define AP_MOTORS_V_FRAME           2
#define AP_MOTORS_H_FRAME           3   // same as X frame but motors spin in opposite direction
#define AP_MOTORS_VTAIL_FRAME       4   // Lynxmotion Hunter VTail 400/500
#define AP_MOTORS_ATAIL_FRAME       5   // A-Shaped VTail Quads
#define AP_MOTORS_NEW_PLUS_FRAME    10  // NEW frames are same as original 4 but with motor orders changed to be clockwise from the front
#define AP_MOTORS_NEW_X_FRAME       11
#define AP_MOTORS_NEW_V_FRAME       12
#define AP_MOTORS_NEW_H_FRAME       13   // same as X frame but motors spin in opposite direction
#define AP_MOTORS_QUADPLANE         14   // motors on 5..8

// motor update rate
#define AP_MOTORS_SPEED_DEFAULT     490 // default output rate to the motors

/// @class      AP_Motors
class AP_Motors {
public:

    // Constructor
    AP_Motors(uint16_t loop_rate, uint16_t speed_hz = AP_MOTORS_SPEED_DEFAULT);

    // set update rate to motors - a value in hertz
    virtual void        set_update_rate( uint16_t speed_hz ) { _speed_hz = speed_hz; };

    // set frame orientation (normally + or X)
    virtual void        set_frame_orientation( uint8_t new_orientation ) { _flags.frame_orientation = new_orientation; };

    // arm, disarm or check status status of motors
    bool                armed() const { return _flags.armed; };
    void                armed(bool arm);

    // set motor interlock status
    void                set_interlock(bool set) { _flags.interlock = set;}

    // get motor interlock status.  true means motors run, false motors don't run
    bool                get_interlock() const { return _flags.interlock; };

    // set_roll, set_pitch, set_yaw, set_throttle
    void                set_roll(float roll_in) { _roll_in = roll_in; };        // range -1 ~ +1
    void                set_pitch(float pitch_in) { _pitch_in = pitch_in; };    // range -1 ~ +1
    void                set_yaw(float yaw_in) { _yaw_in = yaw_in; };            // range -1 ~ +1
    void                set_throttle(float throttle_in) { _throttle_in = throttle_in; };   // range 0 ~ 1
    void                set_throttle_filter_cutoff(float filt_hz) { _throttle_filter.set_cutoff_frequency(filt_hz); }

    void                set_stabilizing(bool stabilizing) { _flags.stabilizing = stabilizing; }

    // accessors for roll, pitch, yaw and throttle inputs to motors
    float               get_roll() const { return _roll_in; }
    float               get_pitch() const { return _pitch_in; }
    float               get_yaw() const { return _yaw_in; }
    float               get_throttle() const { return constrain_float(_throttle_filter.get(),0.0f,1.0f); }

    // spool up states
    enum spool_up_down_desired {
        DESIRED_SHUT_DOWN = 0,              // all motors stop
        DESIRED_SPIN_WHEN_ARMED = 1,        // all motors at spin when armed
        DESIRED_THROTTLE_UNLIMITED = 2,     // motors are no longer constrained by start up procedure
    };

    virtual void set_desired_spool_state(enum spool_up_down_desired spool) { _spool_desired = spool; };

    //
    // voltage, current and air pressure compensation or limiting features - multicopters only
    //
    // set_voltage - set voltage to be used for output scaling
    void                set_voltage(float volts){ _batt_voltage = volts; }

    // set_current - set current to be used for output scaling
    void                set_current(float current){ _batt_current = current; }

    // set_density_ratio - sets air density as a proportion of sea level density
    void                set_air_density_ratio(float ratio) { _air_density_ratio = ratio; }

    // structure for holding motor limit flags
    struct AP_Motors_limit {
        uint8_t roll_pitch      : 1; // we have reached roll or pitch limit
        uint8_t yaw             : 1; // we have reached yaw limit
        uint8_t throttle_lower  : 1; // we have reached throttle's lower limit
        uint8_t throttle_upper  : 1; // we have reached throttle's upper limit
    } limit;

    //
    // virtual functions that should be implemented by child classes
    //

    // init
    virtual void        Init() = 0;

    // enable - starts allowing signals to be sent to motors
    virtual void        enable() = 0;

    // output - sends commands to the motors
    virtual void        output() = 0;

    // output_min - sends minimum values out to the motors
    virtual void        output_min() = 0;

    // output_test - spin a motor at the pwm value specified
    //  motor_seq is the motor's sequence number from 1 to the number of motors on the frame
    //  pwm value is an actual pwm value that will be output, normally in the range of 1000 ~ 2000
    virtual void        output_test(uint8_t motor_seq, int16_t pwm) = 0;

    // get_motor_mask - returns a bitmask of which outputs are being used for motors (1 means being used)
    //  this can be used to ensure other pwm outputs (i.e. for servos) do not conflict
    virtual uint16_t    get_motor_mask() = 0;

    // pilot input in the -1 ~ +1 range for roll, pitch and yaw. 0~1 range for throttle
    void                set_radio_passthrough(float roll_input, float pitch_input, float throttle_input, float yaw_input);

protected:
    // output functions that should be overloaded by child classes
    virtual void        output_armed_stabilizing()=0;
    virtual void        rc_write(uint8_t chan, uint16_t pwm);
    virtual void        rc_set_freq(uint32_t mask, uint16_t freq_hz);
    virtual void        rc_enable_ch(uint8_t chan);
    virtual uint32_t    rc_map_mask(uint32_t mask) const;
    
    // update the throttle input filter
    virtual void        update_throttle_filter() = 0;

    // convert input in -1 to +1 range to pwm output
    int16_t calc_pwm_output_1to1(float input, const RC_Channel& servo);

    // convert input in 0 to +1 range to pwm output
    int16_t calc_pwm_output_0to1(float input, const RC_Channel& servo);

    // flag bitmask
    struct AP_Motors_flags {
        uint8_t armed              : 1;    // 0 if disarmed, 1 if armed
        uint8_t stabilizing        : 1;    // 0 if not controlling attitude, 1 if controlling attitude
        uint8_t frame_orientation  : 4;    // PLUS_FRAME 0, X_FRAME 1, V_FRAME 2, H_FRAME 3, NEW_PLUS_FRAME 10, NEW_X_FRAME, NEW_V_FRAME, NEW_H_FRAME
        uint8_t interlock          : 1;    // 1 if the motor interlock is enabled (i.e. motors run), 0 if disabled (motors don't run)
    } _flags;

    // internal variables
    uint16_t            _loop_rate;                 // rate at which output() function is called (normally 400hz)
    uint16_t            _speed_hz;                  // speed in hz to send updates to motors
    float               _roll_in;                   // desired roll control from attitude controllers, -1 ~ +1
    float               _pitch_in;                  // desired pitch control from attitude controller, -1 ~ +1
    float               _yaw_in;                    // desired yaw control from attitude controller, -1 ~ +1
    float               _throttle_in;               // last throttle input from set_throttle caller
    LowPassFilterFloat  _throttle_filter;           // throttle input filter
    spool_up_down_desired _spool_desired;           // desired spool state

    // battery voltage, current and air pressure compensation variables
    float               _batt_voltage;          // latest battery voltage reading
    float               _batt_current;          // latest battery current reading
    float               _air_density_ratio;     // air density / sea level density - decreases in altitude

    // mapping to output channels
    uint8_t             _motor_map[AP_MOTORS_MAX_NUM_MOTORS];
    uint16_t            _motor_map_mask;

    // pass through variables
    float _roll_radio_passthrough = 0.0f;     // roll input from pilot in -1 ~ +1 range.  used for setup and providing servo feedback while landed
    float _pitch_radio_passthrough = 0.0f;    // pitch input from pilot in -1 ~ +1 range.  used for setup and providing servo feedback while landed
    float _throttle_radio_passthrough = 0.0f; // throttle/collective input from pilot in 0 ~ 1 range.  used for setup and providing servo feedback while landed
    float _yaw_radio_passthrough = 0.0f;      // yaw input from pilot in -1 ~ +1 range.  used for setup and providing servo feedback while landed
};
