// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
#pragma once

#include <AP_AHRS/AP_AHRS.h>
#include <AP_Common/AP_Common.h>
#include <AP_Vehicle/AP_Vehicle.h>
#include "AP_AutoTune.h"
#include <DataFlash/DataFlash.h>
#include <AP_Math/AP_Math.h>

class AP_RollController {
public:
	AP_RollController(AP_AHRS &ahrs, const AP_Vehicle::FixedWing &parms, DataFlash_Class &_dataflash) :
		aparm(parms),
        autotune(gains, AP_AutoTune::AUTOTUNE_ROLL, parms, _dataflash),
        _ahrs(ahrs)
    { 
		AP_Param::setup_object_defaults(this, var_info);
	}

	int32_t get_rate_out(float desired_rate, float scaler);
	int32_t get_servo_out(int32_t angle_err, float scaler, bool disable_integrator);

	void reset_I();

    void autotune_start(void) { autotune.start(); }
    void autotune_restore(void) { autotune.stop(); }

    const       DataFlash_Class::PID_Info& get_pid_info(void) const { return _pid_info; }

	static const struct AP_Param::GroupInfo var_info[];

private:
	const AP_Vehicle::FixedWing &aparm;
    AP_AutoTune::ATGains gains;
    AP_AutoTune autotune;
	uint32_t _last_t;
	float _last_out;

    DataFlash_Class::PID_Info _pid_info;

	int32_t _get_rate_out(float desired_rate, float scaler, bool disable_integrator);

	AP_AHRS &_ahrs;

};
