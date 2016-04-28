// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

/// @file	AC_HELI_PID.cpp
/// @brief	Generic PID algorithm

#include <AP_Math/AP_Math.h>
#include "AC_HELI_PID.h"

const AP_Param::GroupInfo AC_HELI_PID::var_info[] = {
    // @Param: P
    // @DisplayName: PID Proportional Gain
    // @Description: P Gain which produces an output value that is proportional to the current error value
    AP_GROUPINFO("P",    0, AC_HELI_PID, _kp, 0),

    // @Param: I
    // @DisplayName: PID Integral Gain
    // @Description: I Gain which produces an output that is proportional to both the magnitude and the duration of the error
    AP_GROUPINFO("I",    1, AC_HELI_PID, _ki, 0),

    // @Param: D
    // @DisplayName: PID Derivative Gain
    // @Description: D Gain which produces an output that is proportional to the rate of change of the error
    AP_GROUPINFO("D",    2, AC_HELI_PID, _kd, 0),

    // @Param: VFF
    // @DisplayName: Velocity FF FeedForward Gain
    // @Description: Velocity FF Gain which produces an output value that is proportional to the demanded input
    AP_GROUPINFO("VFF",    4, AC_HELI_PID, _vff, 0),

    // @Param: IMAX
    // @DisplayName: PID Integral Maximum
    // @Description: The maximum/minimum value that the I term can output
    AP_GROUPINFO("IMAX", 5, AC_HELI_PID, _imax, 0),

    // @Param: FILT
    // @DisplayName: PID Input filter frequency in Hz
    // @Description: PID Input filter frequency in Hz
    AP_GROUPINFO("FILT", 6, AC_HELI_PID, _filt_hz, AC_PID_FILT_HZ_DEFAULT),

    // @Param: ILMI
    // @DisplayName: I-term Leak Minimum
    // @Description: Point below which I-term will not leak down
    // @Range: 0 1
    // @User: Advanced
    AP_GROUPINFO("ILMI", 7, AC_HELI_PID, _leak_min, AC_PID_LEAK_MIN),

    // @Param: AFF
    // @DisplayName: Acceleration FF FeedForward Gain
    // @Description: Acceleration FF Gain which produces an output value that is proportional to the change in demanded input
    AP_GROUPINFO("AFF",    8, AC_HELI_PID, _aff, 0),

    AP_GROUPEND
};

/// Constructor for PID
AC_HELI_PID::AC_HELI_PID(float initial_p, float initial_i, float initial_d, float initial_imax, float initial_filt_hz, float dt, float initial_vff) :
    AC_PID(initial_p, initial_i, initial_d, initial_imax, initial_filt_hz, dt)
{
    _vff = initial_vff;
    _aff = 0;
    _last_requested_rate = 0;
}

float AC_HELI_PID::get_vff(float requested_rate)
{
    _pid_info.FF = (float)requested_rate * _vff;
    return _pid_info.FF;
}

float AC_HELI_PID::get_aff(float requested_rate)
{
    float derivative;

    // calculate derivative
    if (_dt > 0.0f) {
        derivative = (requested_rate - _last_requested_rate) / _dt;
    } else {
        derivative = 0;
    }

    _pid_info.AFF = derivative * _aff;
    _last_requested_rate = requested_rate;
    return _pid_info.AFF;
}

// This is an integrator which tends to decay to zero naturally
// if the error is zero.

float AC_HELI_PID::get_leaky_i(float leak_rate)
{
    if(!is_zero(_ki) && !is_zero(_dt)){

        // integrator does not leak down below Leak Min
        if (_integrator > _leak_min){
            _integrator -= (float)(_integrator - _leak_min) * leak_rate;
        } else if (_integrator < -_leak_min) {
            _integrator -= (float)(_integrator + _leak_min) * leak_rate;
        }

        _integrator += ((float)_input * _ki) * _dt;
        if (_integrator < -_imax) {
            _integrator = -_imax;
            } else if (_integrator > _imax) {
            _integrator = _imax;
        }

        _pid_info.I = _integrator;
        return _integrator;
    }
    return 0;
}
