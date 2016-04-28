// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "Rover.h"

// default sensors are present and healthy: gyro, accelerometer, rate_control, attitude_stabilization, yaw_position, altitude control, x/y position control, motor_control
#define MAVLINK_SENSOR_PRESENT_DEFAULT (MAV_SYS_STATUS_SENSOR_3D_GYRO | MAV_SYS_STATUS_SENSOR_3D_ACCEL | MAV_SYS_STATUS_SENSOR_ANGULAR_RATE_CONTROL | MAV_SYS_STATUS_SENSOR_ATTITUDE_STABILIZATION | MAV_SYS_STATUS_SENSOR_YAW_POSITION | MAV_SYS_STATUS_SENSOR_XY_POSITION_CONTROL | MAV_SYS_STATUS_SENSOR_MOTOR_OUTPUTS | MAV_SYS_STATUS_AHRS)

void Rover::send_heartbeat(mavlink_channel_t chan)
{
    uint8_t base_mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;
    uint8_t system_status = MAV_STATE_ACTIVE;
    uint32_t custom_mode = control_mode;
    
    if (failsafe.triggered != 0) {
        system_status = MAV_STATE_CRITICAL;
    }

    // work out the base_mode. This value is not very useful
    // for APM, but we calculate it as best we can so a generic
    // MAVLink enabled ground station can work out something about
    // what the MAV is up to. The actual bit values are highly
    // ambiguous for most of the APM flight modes. In practice, you
    // only get useful information from the custom_mode, which maps to
    // the APM flight mode and has a well defined meaning in the
    // ArduPlane documentation
    switch (control_mode) {
    case MANUAL:
    case LEARNING:
    case STEERING:
        base_mode = MAV_MODE_FLAG_MANUAL_INPUT_ENABLED;
        break;
    case AUTO:
    case RTL:
    case GUIDED:
        base_mode = MAV_MODE_FLAG_GUIDED_ENABLED;
        // note that MAV_MODE_FLAG_AUTO_ENABLED does not match what
        // APM does in any mode, as that is defined as "system finds its own goal
        // positions", which APM does not currently do
        break;
    case INITIALISING:
        system_status = MAV_STATE_CALIBRATING;
        break;
    case HOLD:
        system_status = 0;
        break;
    }

#if defined(ENABLE_STICK_MIXING) && (ENABLE_STICK_MIXING==ENABLED)
    if (control_mode != INITIALISING) {
        // all modes except INITIALISING have some form of manual
        // override if stick mixing is enabled
        base_mode |= MAV_MODE_FLAG_MANUAL_INPUT_ENABLED;
    }
#endif

#if HIL_MODE != HIL_MODE_DISABLED
    base_mode |= MAV_MODE_FLAG_HIL_ENABLED;
#endif

    // we are armed if we are not initialising
    if (control_mode != INITIALISING && hal.util->get_soft_armed()) {
        base_mode |= MAV_MODE_FLAG_SAFETY_ARMED;
    }

    // indicate we have set a custom mode
    base_mode |= MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;

    mavlink_msg_heartbeat_send(
        chan,
        MAV_TYPE_GROUND_ROVER,
        MAV_AUTOPILOT_ARDUPILOTMEGA,
        base_mode,
        custom_mode,
        system_status);
}

void Rover::send_attitude(mavlink_channel_t chan)
{
    Vector3f omega = ahrs.get_gyro();
    mavlink_msg_attitude_send(
        chan,
        millis(),
        ahrs.roll,
        ahrs.pitch,
        ahrs.yaw,
        omega.x,
        omega.y,
        omega.z);
}

void Rover::send_extended_status1(mavlink_channel_t chan)
{
    uint32_t control_sensors_present;
    uint32_t control_sensors_enabled;
    uint32_t control_sensors_health;

    // default sensors present
    control_sensors_present = MAVLINK_SENSOR_PRESENT_DEFAULT;

    // first what sensors/controllers we have
    if (g.compass_enabled) {
        control_sensors_present |= MAV_SYS_STATUS_SENSOR_3D_MAG; // compass present
    }
    if (gps.status() > AP_GPS::NO_GPS) {
        control_sensors_present |= MAV_SYS_STATUS_SENSOR_GPS;
    }

    // all present sensors enabled by default except rate control, attitude stabilization, yaw, altitude, position control and motor output which we will set individually
    control_sensors_enabled = control_sensors_present & (~MAV_SYS_STATUS_SENSOR_ANGULAR_RATE_CONTROL & ~MAV_SYS_STATUS_SENSOR_ATTITUDE_STABILIZATION & ~MAV_SYS_STATUS_SENSOR_YAW_POSITION & ~MAV_SYS_STATUS_SENSOR_XY_POSITION_CONTROL & ~MAV_SYS_STATUS_SENSOR_MOTOR_OUTPUTS);

    switch (control_mode) {
    case MANUAL:
    case HOLD:
        break;

    case LEARNING:
    case STEERING:
        control_sensors_enabled |= MAV_SYS_STATUS_SENSOR_ANGULAR_RATE_CONTROL; // 3D angular rate control
        control_sensors_enabled |= MAV_SYS_STATUS_SENSOR_ATTITUDE_STABILIZATION; // attitude stabilisation
        break;

    case AUTO:
    case RTL:
    case GUIDED:
        control_sensors_enabled |= MAV_SYS_STATUS_SENSOR_ANGULAR_RATE_CONTROL; // 3D angular rate control
        control_sensors_enabled |= MAV_SYS_STATUS_SENSOR_ATTITUDE_STABILIZATION; // attitude stabilisation
        control_sensors_enabled |= MAV_SYS_STATUS_SENSOR_YAW_POSITION; // yaw position
        control_sensors_enabled |= MAV_SYS_STATUS_SENSOR_XY_POSITION_CONTROL; // X/Y position control
        break;

    case INITIALISING:
        break;
    }

    // set motors outputs as enabled if safety switch is not disarmed (i.e. either NONE or ARMED)
    if (hal.util->safety_switch_state() != AP_HAL::Util::SAFETY_DISARMED) {
        control_sensors_enabled |= MAV_SYS_STATUS_SENSOR_MOTOR_OUTPUTS;
    }

    // default to all healthy except compass and gps which we set individually
    control_sensors_health = control_sensors_present & (~MAV_SYS_STATUS_SENSOR_3D_MAG & ~MAV_SYS_STATUS_SENSOR_GPS);
    if (g.compass_enabled && compass.healthy(0) && ahrs.use_compass()) {
        control_sensors_health |= MAV_SYS_STATUS_SENSOR_3D_MAG;
    }
    if (gps.status() >= AP_GPS::GPS_OK_FIX_3D) {
        control_sensors_health |= MAV_SYS_STATUS_SENSOR_GPS;
    }
    if (!ins.get_gyro_health_all() || !ins.gyro_calibrated_ok_all()) {
        control_sensors_health &= ~MAV_SYS_STATUS_SENSOR_3D_GYRO;
    }
    if (!ins.get_accel_health_all()) {
        control_sensors_health &= ~MAV_SYS_STATUS_SENSOR_3D_ACCEL;
    }

    if (ahrs.initialised() && !ahrs.healthy()) {
        // AHRS subsystem is unhealthy
        control_sensors_health &= ~MAV_SYS_STATUS_AHRS;
    }

    int16_t battery_current = -1;
    int8_t battery_remaining = -1;

    if (battery.has_current() && battery.healthy()) {
        battery_remaining = battery.capacity_remaining_pct();
        battery_current = battery.current_amps() * 100;
    }

    if (sonar.num_sensors() > 0) {
        control_sensors_present |= MAV_SYS_STATUS_SENSOR_LASER_POSITION;
        if (g.sonar_trigger_cm > 0) {
            control_sensors_enabled |= MAV_SYS_STATUS_SENSOR_LASER_POSITION;
        }
        if (sonar.has_data()) {
            control_sensors_health |= MAV_SYS_STATUS_SENSOR_LASER_POSITION;
        }
    }

    if (AP_Notify::flags.initialising) {
        // while initialising the gyros and accels are not enabled
        control_sensors_enabled &= ~(MAV_SYS_STATUS_SENSOR_3D_GYRO | MAV_SYS_STATUS_SENSOR_3D_ACCEL);
        control_sensors_health &= ~(MAV_SYS_STATUS_SENSOR_3D_GYRO | MAV_SYS_STATUS_SENSOR_3D_ACCEL);
    }

    mavlink_msg_sys_status_send(
        chan,
        control_sensors_present,
        control_sensors_enabled,
        control_sensors_health,
        (uint16_t)(scheduler.load_average(20000) * 1000),
        battery.voltage() * 1000, // mV
        battery_current,        // in 10mA units
        battery_remaining,      // in %
        0, // comm drops %,
        0, // comm drops in pkts,
        0, 0, 0, 0);

}

void Rover::send_location(mavlink_channel_t chan)
{
    uint32_t fix_time;
    // if we have a GPS fix, take the time as the last fix time. That
    // allows us to correctly calculate velocities and extrapolate
    // positions.
    // If we don't have a GPS fix then we are dead reckoning, and will
    // use the current boot time as the fix time.    
    if (gps.status() >= AP_GPS::GPS_OK_FIX_2D) {
        fix_time = gps.last_fix_time_ms();
    } else {
        fix_time = millis();
    }
    const Vector3f &vel = gps.velocity();
    mavlink_msg_global_position_int_send(
        chan,
        fix_time,
        current_loc.lat,                   // in 1E7 degrees
        current_loc.lng,                   // in 1E7 degrees
        current_loc.alt * 10UL,            // millimeters above sea level
        (current_loc.alt - home.alt) * 10, // millimeters above ground
        vel.x * 100,  // X speed cm/s (+ve North)
        vel.y * 100,  // Y speed cm/s (+ve East)
        vel.z * -100, // Z speed cm/s (+ve up)
        ahrs.yaw_sensor);
}

void Rover::send_nav_controller_output(mavlink_channel_t chan)
{
    mavlink_msg_nav_controller_output_send(
        chan,
        lateral_acceleration, // use nav_roll to hold demanded Y accel
        gps.ground_speed() * ins.get_gyro().z, // use nav_pitch to hold actual Y accel
        nav_controller->nav_bearing_cd() * 0.01f,
        nav_controller->target_bearing_cd() * 0.01f,
        wp_distance,
        0,
        groundspeed_error,
        nav_controller->crosstrack_error());
}

void Rover::send_servo_out(mavlink_channel_t chan)
{
#if HIL_MODE != HIL_MODE_DISABLED
    // normalized values scaled to -10000 to 10000
    // This is used for HIL.  Do not change without discussing with
    // HIL maintainers
    mavlink_msg_rc_channels_scaled_send(
        chan,
        millis(),
        0, // port 0
        10000 * channel_steer->norm_output(),
        0,
        10000 * channel_throttle->norm_output(),
        0,
        0,
        0,
        0,
        0,
        receiver_rssi);
#endif
}

void Rover::send_radio_out(mavlink_channel_t chan)
{
#if HIL_MODE == HIL_MODE_DISABLED || HIL_SERVOS
    mavlink_msg_servo_output_raw_send(
        chan,
        micros(),
        0,     // port
        hal.rcout->read(0),
        hal.rcout->read(1),
        hal.rcout->read(2),
        hal.rcout->read(3),
        hal.rcout->read(4),
        hal.rcout->read(5),
        hal.rcout->read(6),
        hal.rcout->read(7));
#else
    mavlink_msg_servo_output_raw_send(
        chan,
        micros(),
        0,     // port
        RC_Channel::rc_channel(0)->radio_out,
        RC_Channel::rc_channel(1)->radio_out,
        RC_Channel::rc_channel(2)->radio_out,
        RC_Channel::rc_channel(3)->radio_out,
        RC_Channel::rc_channel(4)->radio_out,
        RC_Channel::rc_channel(5)->radio_out,
        RC_Channel::rc_channel(6)->radio_out,
        RC_Channel::rc_channel(7)->radio_out);
#endif
}

void Rover::send_vfr_hud(mavlink_channel_t chan)
{
    mavlink_msg_vfr_hud_send(
        chan,
        gps.ground_speed(),
        gps.ground_speed(),
        (ahrs.yaw_sensor / 100) % 360,
        (uint16_t)(100 * fabsf(channel_throttle->norm_output())),
        current_loc.alt / 100.0,
        0);
}

// report simulator state
void Rover::send_simstate(mavlink_channel_t chan)
{
#if CONFIG_HAL_BOARD == HAL_BOARD_SITL
    sitl.simstate_send(chan);
#endif
}

void Rover::send_hwstatus(mavlink_channel_t chan)
{
    mavlink_msg_hwstatus_send(
        chan,
        hal.analogin->board_voltage()*1000,
        hal.i2c->lockup_count());
}

void Rover::send_rangefinder(mavlink_channel_t chan)
{
    if (!sonar.has_data(0) && !sonar.has_data(1)) {
        // no sonar to report
        return;
    }

    float distance_cm = 0.0f;
    float voltage = 0.0f;

    /*
      report smaller distance of two sonars
     */
    if (sonar.has_data(0) && sonar.has_data(1)) {
        if (sonar.distance_cm(0) <= sonar.distance_cm(1)) {
            distance_cm = sonar.distance_cm(0);
            voltage = sonar.voltage_mv(0);
        } else {
            distance_cm = sonar.distance_cm(1);
            voltage = sonar.voltage_mv(1);
        }
    } else {
        // only sonar 0 or sonar 1 has data
        if (sonar.has_data(0)) {
            distance_cm = sonar.distance_cm(0);
            voltage = sonar.voltage_mv(0) * 0.001f;
        }
        if (sonar.has_data(1)) {
            distance_cm = sonar.distance_cm(1);
            voltage = sonar.voltage_mv(1) * 0.001f;
        }
    }

    mavlink_msg_rangefinder_send(
        chan,
        distance_cm * 0.01f,
        voltage);
}

/*
  send PID tuning message
 */
void Rover::send_pid_tuning(mavlink_channel_t chan)
{
    const Vector3f &gyro = ahrs.get_gyro();
    if (g.gcs_pid_mask & 1) {
        const DataFlash_Class::PID_Info &pid_info = steerController.get_pid_info();
        mavlink_msg_pid_tuning_send(chan, PID_TUNING_STEER, 
                                    pid_info.desired,
                                    degrees(gyro.z),
                                    pid_info.FF,
                                    pid_info.P,
                                    pid_info.I,
                                    pid_info.D);
        if (!HAVE_PAYLOAD_SPACE(chan, PID_TUNING)) {
            return;
        }
    }
}

void Rover::send_current_waypoint(mavlink_channel_t chan)
{
    mavlink_msg_mission_current_send(chan, mission.get_current_nav_index());
}

// are we still delaying telemetry to try to avoid Xbee bricking?
bool Rover::telemetry_delayed(mavlink_channel_t chan)
{
    uint32_t tnow = millis() >> 10;
    if (tnow > (uint32_t)g.telem_delay) {
        return false;
    }
    if (chan == MAVLINK_COMM_0 && hal.gpio->usb_connected()) {
        // this is USB telemetry, so won't be an Xbee
        return false;
    }
    // we're either on the 2nd UART, or no USB cable is connected
    // we need to delay telemetry by the TELEM_DELAY time
    return true;
}


// try to send a message, return false if it won't fit in the serial tx buffer
bool GCS_MAVLINK::try_send_message(enum ap_message id)
{
    if (rover.telemetry_delayed(chan)) {
        return false;
    }

    // if we don't have at least 1ms remaining before the main loop
    // wants to fire then don't send a mavlink message. We want to
    // prioritise the main flight control loop over communications
    if (!rover.in_mavlink_delay && rover.scheduler.time_available_usec() < 1200) {
        rover.gcs_out_of_time = true;
        return false;
    }

    switch (id) {
    case MSG_HEARTBEAT:
        CHECK_PAYLOAD_SIZE(HEARTBEAT);
        rover.gcs[chan-MAVLINK_COMM_0].last_heartbeat_time = AP_HAL::millis();        
        rover.send_heartbeat(chan);
        return true;

    case MSG_EXTENDED_STATUS1:
        CHECK_PAYLOAD_SIZE(SYS_STATUS);
        rover.send_extended_status1(chan);
        CHECK_PAYLOAD_SIZE(POWER_STATUS);
        rover.gcs[chan-MAVLINK_COMM_0].send_power_status();
        break;

    case MSG_EXTENDED_STATUS2:
        CHECK_PAYLOAD_SIZE(MEMINFO);
        rover.gcs[chan-MAVLINK_COMM_0].send_meminfo();
        break;

    case MSG_ATTITUDE:
        CHECK_PAYLOAD_SIZE(ATTITUDE);
        rover.send_attitude(chan);
        break;

    case MSG_LOCATION:
        CHECK_PAYLOAD_SIZE(GLOBAL_POSITION_INT);
        rover.send_location(chan);
        break;

    case MSG_LOCAL_POSITION:
        CHECK_PAYLOAD_SIZE(LOCAL_POSITION_NED);
        send_local_position(rover.ahrs);
        break;

    case MSG_NAV_CONTROLLER_OUTPUT:
        if (rover.control_mode != MANUAL) {
            CHECK_PAYLOAD_SIZE(NAV_CONTROLLER_OUTPUT);
            rover.send_nav_controller_output(chan);
        }
        break;

    case MSG_GPS_RAW:
        CHECK_PAYLOAD_SIZE(GPS_RAW_INT);
        rover.gcs[chan-MAVLINK_COMM_0].send_gps_raw(rover.gps);
        break;

    case MSG_SYSTEM_TIME:
        CHECK_PAYLOAD_SIZE(SYSTEM_TIME);
        rover.gcs[chan-MAVLINK_COMM_0].send_system_time(rover.gps);
        break;

    case MSG_SERVO_OUT:
        CHECK_PAYLOAD_SIZE(RC_CHANNELS_SCALED);
        rover.send_servo_out(chan);
        break;

    case MSG_RADIO_IN:
        CHECK_PAYLOAD_SIZE(RC_CHANNELS_RAW);
        rover.gcs[chan-MAVLINK_COMM_0].send_radio_in(rover.receiver_rssi);
        break;

    case MSG_RADIO_OUT:
        CHECK_PAYLOAD_SIZE(SERVO_OUTPUT_RAW);
        rover.send_radio_out(chan);
        break;

    case MSG_VFR_HUD:
        CHECK_PAYLOAD_SIZE(VFR_HUD);
        rover.send_vfr_hud(chan);
        break;

    case MSG_RAW_IMU1:
        CHECK_PAYLOAD_SIZE(RAW_IMU);
        rover.gcs[chan-MAVLINK_COMM_0].send_raw_imu(rover.ins, rover.compass);
        break;

    case MSG_RAW_IMU3:
        CHECK_PAYLOAD_SIZE(SENSOR_OFFSETS);
        rover.gcs[chan-MAVLINK_COMM_0].send_sensor_offsets(rover.ins, rover.compass, rover.barometer);
        break;

    case MSG_CURRENT_WAYPOINT:
        CHECK_PAYLOAD_SIZE(MISSION_CURRENT);
        rover.send_current_waypoint(chan);
        break;

    case MSG_NEXT_PARAM:
        CHECK_PAYLOAD_SIZE(PARAM_VALUE);
        rover.gcs[chan-MAVLINK_COMM_0].queued_param_send();
        break;

    case MSG_NEXT_WAYPOINT:
        CHECK_PAYLOAD_SIZE(MISSION_REQUEST);
        rover.gcs[chan-MAVLINK_COMM_0].queued_waypoint_send();
        break;

    case MSG_STATUSTEXT:
        // depreciated, use GCS_MAVLINK::send_statustext*
        return false;

    case MSG_AHRS:
        CHECK_PAYLOAD_SIZE(AHRS);
        rover.gcs[chan-MAVLINK_COMM_0].send_ahrs(rover.ahrs);
        break;

    case MSG_SIMSTATE:
        CHECK_PAYLOAD_SIZE(SIMSTATE);
        rover.send_simstate(chan);
        break;

    case MSG_HWSTATUS:
        CHECK_PAYLOAD_SIZE(HWSTATUS);
        rover.send_hwstatus(chan);
        break;

    case MSG_RANGEFINDER:
        CHECK_PAYLOAD_SIZE(RANGEFINDER);
        rover.send_rangefinder(chan);
        break;

    case MSG_MOUNT_STATUS:
#if MOUNT == ENABLED
        CHECK_PAYLOAD_SIZE(MOUNT_STATUS);
        rover.camera_mount.status_msg(chan);
#endif // MOUNT == ENABLED
        break;

    case MSG_RAW_IMU2:
    case MSG_LIMITS_STATUS:
    case MSG_FENCE_STATUS:
    case MSG_WIND:
    case MSG_VIBRATION:
        // unused
        break;

    case MSG_BATTERY2:
        CHECK_PAYLOAD_SIZE(BATTERY2);
        rover.gcs[chan-MAVLINK_COMM_0].send_battery2(rover.battery);
        break;

    case MSG_CAMERA_FEEDBACK:
#if CAMERA == ENABLED
        CHECK_PAYLOAD_SIZE(CAMERA_FEEDBACK);
        rover.camera.send_feedback(chan, rover.gps, rover.ahrs, rover.current_loc);
#endif
        break;

    case MSG_EKF_STATUS_REPORT:
#if AP_AHRS_NAVEKF_AVAILABLE
        CHECK_PAYLOAD_SIZE(EKF_STATUS_REPORT);
        rover.ahrs.send_ekf_status_report(chan);
#endif
        break;

    case MSG_PID_TUNING:
        CHECK_PAYLOAD_SIZE(PID_TUNING);
        rover.send_pid_tuning(chan);
        break;

    case MSG_MISSION_ITEM_REACHED:
        CHECK_PAYLOAD_SIZE(MISSION_ITEM_REACHED);
        mavlink_msg_mission_item_reached_send(chan, mission_item_reached_index);
        break;

    case MSG_MAG_CAL_PROGRESS:
        CHECK_PAYLOAD_SIZE(MAG_CAL_PROGRESS);
        rover.compass.send_mag_cal_progress(chan);
        break;

    case MSG_MAG_CAL_REPORT:
        CHECK_PAYLOAD_SIZE(MAG_CAL_REPORT);
        rover.compass.send_mag_cal_report(chan);
        break;

    case MSG_RETRY_DEFERRED:
    case MSG_TERRAIN:
    case MSG_OPTICAL_FLOW:
    case MSG_GIMBAL_REPORT:
    case MSG_RPM:
        break; // just here to prevent a warning

    }

    
    return true;
}

/*
  default stream rates to 1Hz
 */
const AP_Param::GroupInfo GCS_MAVLINK::var_info[] = {
    // @Param: RAW_SENS
    // @DisplayName: Raw sensor stream rate
    // @Description: Raw sensor stream rate to ground station
    // @Units: Hz
    // @Range: 0 10
    // @Increment: 1
    // @User: Advanced
    AP_GROUPINFO("RAW_SENS", 0, GCS_MAVLINK, streamRates[0],  1),

    // @Param: EXT_STAT
    // @DisplayName: Extended status stream rate to ground station
    // @Description: Extended status stream rate to ground station
    // @Units: Hz
    // @Range: 0 10
    // @Increment: 1
    // @User: Advanced
    AP_GROUPINFO("EXT_STAT", 1, GCS_MAVLINK, streamRates[1],  1),

    // @Param: RC_CHAN
    // @DisplayName: RC Channel stream rate to ground station
    // @Description: RC Channel stream rate to ground station
    // @Units: Hz
    // @Range: 0 10
    // @Increment: 1
    // @User: Advanced
    AP_GROUPINFO("RC_CHAN",  2, GCS_MAVLINK, streamRates[2],  1),

    // @Param: RAW_CTRL
    // @DisplayName: Raw Control stream rate to ground station
    // @Description: Raw Control stream rate to ground station
    // @Units: Hz
    // @Range: 0 10
    // @Increment: 1
    // @User: Advanced
    AP_GROUPINFO("RAW_CTRL", 3, GCS_MAVLINK, streamRates[3],  1),

    // @Param: POSITION
    // @DisplayName: Position stream rate to ground station
    // @Description: Position stream rate to ground station
    // @Units: Hz
    // @Range: 0 10
    // @Increment: 1
    // @User: Advanced
    AP_GROUPINFO("POSITION", 4, GCS_MAVLINK, streamRates[4],  1),

    // @Param: EXTRA1
    // @DisplayName: Extra data type 1 stream rate to ground station
    // @Description: Extra data type 1 stream rate to ground station
    // @Units: Hz
    // @Range: 0 10
    // @Increment: 1
    // @User: Advanced
    AP_GROUPINFO("EXTRA1",   5, GCS_MAVLINK, streamRates[5],  1),

    // @Param: EXTRA2
    // @DisplayName: Extra data type 2 stream rate to ground station
    // @Description: Extra data type 2 stream rate to ground station
    // @Units: Hz
    // @Range: 0 10
    // @Increment: 1
    // @User: Advanced
    AP_GROUPINFO("EXTRA2",   6, GCS_MAVLINK, streamRates[6],  1),

    // @Param: EXTRA3
    // @DisplayName: Extra data type 3 stream rate to ground station
    // @Description: Extra data type 3 stream rate to ground station
    // @Units: Hz
    // @Range: 0 10
    // @Increment: 1
    // @User: Advanced
    AP_GROUPINFO("EXTRA3",   7, GCS_MAVLINK, streamRates[7],  1),

    // @Param: PARAMS
    // @DisplayName: Parameter stream rate to ground station
    // @Description: Parameter stream rate to ground station
    // @Units: Hz
    // @Range: 0 10
    // @Increment: 1
    // @User: Advanced
    AP_GROUPINFO("PARAMS",   8, GCS_MAVLINK, streamRates[8],  10),
    AP_GROUPEND
};


// see if we should send a stream now. Called at 50Hz
bool GCS_MAVLINK::stream_trigger(enum streams stream_num)
{
    if (stream_num >= NUM_STREAMS) {
        return false;
    }
    float rate = (uint8_t)streamRates[stream_num].get();

    // send at a much lower rate while handling waypoints and
    // parameter sends
    if ((stream_num != STREAM_PARAMS) && 
        (waypoint_receiving || _queued_parameter != NULL)) {
        rate *= 0.25f;
    }

    if (rate <= 0) {
        return false;
    }

    if (stream_ticks[stream_num] == 0) {
        // we're triggering now, setup the next trigger point
        if (rate > 50) {
            rate = 50;
        }
        stream_ticks[stream_num] = (50 / rate) - 1 + stream_slowdown;
        return true;
    }

    // count down at 50Hz
    stream_ticks[stream_num]--;
    return false;
}

void
GCS_MAVLINK::data_stream_send(void)
{
    rover.gcs_out_of_time = false;

    if (!rover.in_mavlink_delay) {
        handle_log_send(rover.DataFlash);
    }

    if (_queued_parameter != NULL) {
        if (streamRates[STREAM_PARAMS].get() <= 0) {
            streamRates[STREAM_PARAMS].set(10);
        }
        if (stream_trigger(STREAM_PARAMS)) {
            send_message(MSG_NEXT_PARAM);
        }
    }

    if (rover.gcs_out_of_time) return;

    if (rover.in_mavlink_delay) {
#if HIL_MODE != HIL_MODE_DISABLED
        // in HIL we need to keep sending servo values to ensure
        // the simulator doesn't pause, otherwise our sensor
        // calibration could stall
        if (stream_trigger(STREAM_RAW_CONTROLLER)) {
            send_message(MSG_SERVO_OUT);
        }
        if (stream_trigger(STREAM_RC_CHANNELS)) {
            send_message(MSG_RADIO_OUT);
        }
#endif
        // don't send any other stream types while in the delay callback
        return;
    }

    if (rover.gcs_out_of_time) return;

    if (stream_trigger(STREAM_RAW_SENSORS)) {
        send_message(MSG_RAW_IMU1);
        send_message(MSG_RAW_IMU3);
    }

    if (rover.gcs_out_of_time) return;

    if (stream_trigger(STREAM_EXTENDED_STATUS)) {
        send_message(MSG_EXTENDED_STATUS1);
        send_message(MSG_EXTENDED_STATUS2);
        send_message(MSG_CURRENT_WAYPOINT);
        send_message(MSG_GPS_RAW);            // TODO - remove this message after location message is working
        send_message(MSG_NAV_CONTROLLER_OUTPUT);
    }

    if (rover.gcs_out_of_time) return;

    if (stream_trigger(STREAM_POSITION)) {
        // sent with GPS read
        send_message(MSG_LOCATION);
        send_message(MSG_LOCAL_POSITION);
    }

    if (rover.gcs_out_of_time) return;

    if (stream_trigger(STREAM_RAW_CONTROLLER)) {
        send_message(MSG_SERVO_OUT);
    }

    if (rover.gcs_out_of_time) return;

    if (stream_trigger(STREAM_RC_CHANNELS)) {
        send_message(MSG_RADIO_OUT);
        send_message(MSG_RADIO_IN);
    }

    if (rover.gcs_out_of_time) return;

    if (stream_trigger(STREAM_EXTRA1)) {
        send_message(MSG_ATTITUDE);
        send_message(MSG_SIMSTATE);
        if (rover.control_mode != MANUAL) {
            send_message(MSG_PID_TUNING);
        }
    }
    
    if (rover.gcs_out_of_time) return;

    if (stream_trigger(STREAM_EXTRA2)) {
        send_message(MSG_VFR_HUD);
    }

    if (rover.gcs_out_of_time) return;

    if (stream_trigger(STREAM_EXTRA3)) {
        send_message(MSG_AHRS);
        send_message(MSG_HWSTATUS);
        send_message(MSG_RANGEFINDER);
        send_message(MSG_SYSTEM_TIME);
        send_message(MSG_BATTERY2);
        send_message(MSG_MAG_CAL_REPORT);
        send_message(MSG_MAG_CAL_PROGRESS);
        send_message(MSG_MOUNT_STATUS);
        send_message(MSG_EKF_STATUS_REPORT);
    }
}



void GCS_MAVLINK::handle_guided_request(AP_Mission::Mission_Command &cmd)
{
    if (rover.control_mode != GUIDED) {
        // only accept position updates when in GUIDED mode
        return;
    }
        
    rover.guided_WP = cmd.content.location;

    // make any new wp uploaded instant (in case we are already in Guided mode)
    rover.rtl_complete = false;
    rover.set_guided_WP();
}

void GCS_MAVLINK::handle_change_alt_request(AP_Mission::Mission_Command &cmd)
{
    // nothing to do
}

void GCS_MAVLINK::handleMessage(mavlink_message_t* msg)
{
    switch (msg->msgid) {

    case MAVLINK_MSG_ID_REQUEST_DATA_STREAM:
        {
            handle_request_data_stream(msg, true);
            break;
        }

    case MAVLINK_MSG_ID_COMMAND_LONG:
        {
            // decode
            mavlink_command_long_t packet;
            mavlink_msg_command_long_decode(msg, &packet);

            uint8_t result = MAV_RESULT_UNSUPPORTED;

            // do command

            switch(packet.command) {

            case MAV_CMD_START_RX_PAIR:
                // initiate bind procedure
                if (!hal.rcin->rc_bind(packet.param1)) {
                    result = MAV_RESULT_FAILED;
                } else {
                    result = MAV_RESULT_ACCEPTED;
                }
                break;

            case MAV_CMD_NAV_RETURN_TO_LAUNCH:
                rover.set_mode(RTL);
                result = MAV_RESULT_ACCEPTED;
                break;

#if MOUNT == ENABLED
            // Sets the region of interest (ROI) for the camera
            case MAV_CMD_DO_SET_ROI:
                // sanity check location
                if (fabsf(packet.param5) > 90.0f || fabsf(packet.param6) > 180.0f) {
                    break;
                }
                Location roi_loc;
                roi_loc.lat = (int32_t)(packet.param5 * 1.0e7f);
                roi_loc.lng = (int32_t)(packet.param6 * 1.0e7f);
                roi_loc.alt = (int32_t)(packet.param7 * 100.0f);
                if (roi_loc.lat == 0 && roi_loc.lng == 0 && roi_loc.alt == 0) {
                    // switch off the camera tracking if enabled
                    if (rover.camera_mount.get_mode() == MAV_MOUNT_MODE_GPS_POINT) {
                        rover.camera_mount.set_mode_to_default();
                    }
                } else {
                    // send the command to the camera mount
                    rover.camera_mount.set_roi_target(roi_loc);
                }
                result = MAV_RESULT_ACCEPTED;
                break;
#endif

#if CAMERA == ENABLED
        case MAV_CMD_DO_DIGICAM_CONFIGURE:
            rover.camera.configure(packet.param1,
                                   packet.param2,
                                   packet.param3,
                                   packet.param4,
                                   packet.param5,
                                   packet.param6,
                                   packet.param7);

            result = MAV_RESULT_ACCEPTED;
            break;

        case MAV_CMD_DO_DIGICAM_CONTROL:
            if (rover.camera.control(packet.param1,
                                     packet.param2,
                                     packet.param3,
                                     packet.param4,
                                     packet.param5,
                                     packet.param6)) {
                rover.log_picture();
            }
            result = MAV_RESULT_ACCEPTED;
            break;
#endif // CAMERA == ENABLED

            case MAV_CMD_DO_MOUNT_CONTROL:
#if MOUNT == ENABLED
                rover.camera_mount.control(packet.param1, packet.param2, packet.param3, (MAV_MOUNT_MODE) packet.param7);
#endif
                break;

            case MAV_CMD_MISSION_START:
                rover.set_mode(AUTO);
                result = MAV_RESULT_ACCEPTED;
                break;

            case MAV_CMD_PREFLIGHT_CALIBRATION:
                if(hal.util->get_soft_armed()) {
                    result = MAV_RESULT_FAILED;
                    break;
                }
                if (is_equal(packet.param1,1.0f)) {
                    rover.ins.init_gyro();
                    if (rover.ins.gyro_calibrated_ok_all()) {
                        rover.ahrs.reset_gyro_drift();
                        result = MAV_RESULT_ACCEPTED;
                    } else {
                        result = MAV_RESULT_FAILED;
                    }
                } else if (is_equal(packet.param3,1.0f)) {
                    rover.init_barometer();
                    result = MAV_RESULT_ACCEPTED;
                } else if (is_equal(packet.param4,1.0f)) {
                    rover.trim_radio();
                    result = MAV_RESULT_ACCEPTED;
                } else if (is_equal(packet.param5,1.0f)) {
                    result = MAV_RESULT_ACCEPTED;
                    // start with gyro calibration
                    rover.ins.init_gyro();
                    // reset ahrs gyro bias
                    if (rover.ins.gyro_calibrated_ok_all()) {
                        rover.ahrs.reset_gyro_drift();
                    } else {
                        result = MAV_RESULT_FAILED;
                    }
                    rover.ins.acal_init();
                    rover.ins.get_acal()->start(this);

                } else if (is_equal(packet.param5,2.0f)) {
                    // start with gyro calibration
                    rover.ins.init_gyro();
                    // accel trim
                    float trim_roll, trim_pitch;
                    if(rover.ins.calibrate_trim(trim_roll, trim_pitch)) {
                        // reset ahrs's trim to suggested values from calibration routine
                        rover.ahrs.set_trim(Vector3f(trim_roll, trim_pitch, 0));
                        result = MAV_RESULT_ACCEPTED;
                    } else {
                        result = MAV_RESULT_FAILED;
                    }
                }
                else {
                    send_text(MAV_SEVERITY_WARNING, "Unsupported preflight calibration");
                }
                break;

            case MAV_CMD_PREFLIGHT_SET_SENSOR_OFFSETS:
                if (is_equal(packet.param1,2.0f)) {
                    // save first compass's offsets
                    rover.compass.set_and_save_offsets(0, packet.param2, packet.param3, packet.param4);
                    result = MAV_RESULT_ACCEPTED;
                }
                if (is_equal(packet.param1,5.0f)) {
                    // save secondary compass's offsets
                    rover.compass.set_and_save_offsets(1, packet.param2, packet.param3, packet.param4);
                    result = MAV_RESULT_ACCEPTED;
                }
                break;

        case MAV_CMD_DO_SET_MODE:
            switch ((uint16_t)packet.param1) {
            case MAV_MODE_MANUAL_ARMED:
            case MAV_MODE_MANUAL_DISARMED:
                rover.set_mode(MANUAL);
                result = MAV_RESULT_ACCEPTED;
                break;

            case MAV_MODE_AUTO_ARMED:
            case MAV_MODE_AUTO_DISARMED:
                rover.set_mode(AUTO);
                result = MAV_RESULT_ACCEPTED;
                break;

            case MAV_MODE_STABILIZE_DISARMED:
            case MAV_MODE_STABILIZE_ARMED:
                rover.set_mode(LEARNING);
                result = MAV_RESULT_ACCEPTED;
                break;

            default:
                result = MAV_RESULT_UNSUPPORTED;
            }
            break;

        case MAV_CMD_DO_SET_SERVO:
            if (rover.ServoRelayEvents.do_set_servo(packet.param1, packet.param2)) {
                result = MAV_RESULT_ACCEPTED;
            }
            break;

        case MAV_CMD_DO_REPEAT_SERVO:
            if (rover.ServoRelayEvents.do_repeat_servo(packet.param1, packet.param2, packet.param3, packet.param4*1000)) {
                result = MAV_RESULT_ACCEPTED;
            }
            break;

        case MAV_CMD_DO_SET_RELAY:
            if (rover.ServoRelayEvents.do_set_relay(packet.param1, packet.param2)) {
                result = MAV_RESULT_ACCEPTED;
            }
            break;

        case MAV_CMD_DO_REPEAT_RELAY:
            if (rover.ServoRelayEvents.do_repeat_relay(packet.param1, packet.param2, packet.param3*1000)) {
                result = MAV_RESULT_ACCEPTED;
            }
            break;

        case MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN:
            if (is_equal(packet.param1,1.0f) || is_equal(packet.param1,3.0f)) {
                // when packet.param1 == 3 we reboot to hold in bootloader
                hal.scheduler->reboot(is_equal(packet.param1,3.0f));
                result = MAV_RESULT_ACCEPTED;
            }
            break;

        case MAV_CMD_COMPONENT_ARM_DISARM:
            if (is_equal(packet.param1,1.0f)) {
                // run pre_arm_checks and arm_checks and display failures
                if (rover.arm_motors(AP_Arming::MAVLINK)) {
                    result = MAV_RESULT_ACCEPTED;
                } else {
                    result = MAV_RESULT_FAILED;
                }
            } else if (is_zero(packet.param1))  {
                if (rover.disarm_motors()) {
                    result = MAV_RESULT_ACCEPTED;
                } else {
                    result = MAV_RESULT_FAILED;
                }
            } else {
                result = MAV_RESULT_UNSUPPORTED;
            }
            break;

        case MAV_CMD_GET_HOME_POSITION:
            if (rover.home_is_set != HOME_UNSET) {
                send_home(rover.ahrs.get_home());
                result = MAV_RESULT_ACCEPTED;
            }
            break;

        case MAV_CMD_REQUEST_AUTOPILOT_CAPABILITIES: {
            if (is_equal(packet.param1,1.0f)) {
                rover.gcs[chan-MAVLINK_COMM_0].send_autopilot_version(FIRMWARE_VERSION);
                result = MAV_RESULT_ACCEPTED;
            }
            break;
        }

        case MAV_CMD_DO_START_MAG_CAL:
        case MAV_CMD_DO_ACCEPT_MAG_CAL:
        case MAV_CMD_DO_CANCEL_MAG_CAL:
            result = rover.compass.handle_mag_cal_command(packet);
            break;

        default:
                break;
            }

            mavlink_msg_command_ack_send_buf(
                msg,
                chan,
                packet.command,
                result);

            break;
        }

    case MAVLINK_MSG_ID_SET_MODE:
		{
            handle_set_mode(msg, FUNCTOR_BIND(&rover, &Rover::mavlink_set_mode, bool, uint8_t));
            break;
        }

    case MAVLINK_MSG_ID_MISSION_REQUEST_LIST:
        {
            handle_mission_request_list(rover.mission, msg);
            break;
        }


	// XXX read a WP from EEPROM and send it to the GCS
    case MAVLINK_MSG_ID_MISSION_REQUEST:
    {
        handle_mission_request(rover.mission, msg);
        break;
    }


    case MAVLINK_MSG_ID_MISSION_ACK:
        {
            // not used
            break;
        }

    case MAVLINK_MSG_ID_PARAM_REQUEST_LIST:
        {
            // mark the firmware version in the tlog
            send_text(MAV_SEVERITY_INFO, FIRMWARE_STRING);

#if defined(PX4_GIT_VERSION) && defined(NUTTX_GIT_VERSION)
            send_text(MAV_SEVERITY_INFO, "PX4: " PX4_GIT_VERSION " NuttX: " NUTTX_GIT_VERSION);
#endif
            handle_param_request_list(msg);
            break;
        }

    case MAVLINK_MSG_ID_PARAM_REQUEST_READ:
    {
        handle_param_request_read(msg);
        break;
    }

    case MAVLINK_MSG_ID_MISSION_CLEAR_ALL:
        {
            handle_mission_clear_all(rover.mission, msg);
            break;
        }

    case MAVLINK_MSG_ID_MISSION_SET_CURRENT:
        {
            handle_mission_set_current(rover.mission, msg);
            break;
        }

    case MAVLINK_MSG_ID_MISSION_COUNT:
        {
            handle_mission_count(rover.mission, msg);
            break;
        }

    case MAVLINK_MSG_ID_MISSION_WRITE_PARTIAL_LIST:
    {
        handle_mission_write_partial_list(rover.mission, msg);
        break;
    }

    // GCS has sent us a mission item, store to EEPROM
    case MAVLINK_MSG_ID_MISSION_ITEM:
        {
            if (handle_mission_item(msg, rover.mission)) {
                rover.DataFlash.Log_Write_EntireMission(rover.mission);
            }
            break;
        }


    case MAVLINK_MSG_ID_PARAM_SET:
        {
            handle_param_set(msg, &rover.DataFlash);
            break;
        }

    case MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE:
    {
        // allow override of RC channel values for HIL
        // or for complete GCS control of switch position
        // and RC PWM values.
        if(msg->sysid != rover.g.sysid_my_gcs) break;                         // Only accept control from our gcs
        mavlink_rc_channels_override_t packet;
        int16_t v[8];
        mavlink_msg_rc_channels_override_decode(msg, &packet);

        v[0] = packet.chan1_raw;
        v[1] = packet.chan2_raw;
        v[2] = packet.chan3_raw;
        v[3] = packet.chan4_raw;
        v[4] = packet.chan5_raw;
        v[5] = packet.chan6_raw;
        v[6] = packet.chan7_raw;
        v[7] = packet.chan8_raw;

        hal.rcin->set_overrides(v, 8);

        rover.failsafe.rc_override_timer = AP_HAL::millis();
        rover.failsafe_trigger(FAILSAFE_EVENT_RC, false);
        break;
    }

    case MAVLINK_MSG_ID_HEARTBEAT:
        {
            // We keep track of the last time we received a heartbeat from our GCS for failsafe purposes
			if(msg->sysid != rover.g.sysid_my_gcs) break;
            rover.last_heartbeat_ms = rover.failsafe.rc_override_timer = AP_HAL::millis();
            rover.failsafe_trigger(FAILSAFE_EVENT_GCS, false);
            break;
        }

#if HIL_MODE != HIL_MODE_DISABLED
	case MAVLINK_MSG_ID_HIL_STATE:
		{
			mavlink_hil_state_t packet;
			mavlink_msg_hil_state_decode(msg, &packet);
			
            // set gps hil sensor
            Location loc;
            loc.lat = packet.lat;
            loc.lng = packet.lon;
            loc.alt = packet.alt/10;
            Vector3f vel(packet.vx, packet.vy, packet.vz);
            vel *= 0.01f;
            
            gps.setHIL(0, AP_GPS::GPS_OK_FIX_3D,
                       packet.time_usec/1000,
                       loc, vel, 10, 0, true);
			
			// rad/sec
            Vector3f gyros;
            gyros.x = packet.rollspeed;
            gyros.y = packet.pitchspeed;
            gyros.z = packet.yawspeed;

            // m/s/s
            Vector3f accels;
            accels.x = packet.xacc * (GRAVITY_MSS/1000.0f);
            accels.y = packet.yacc * (GRAVITY_MSS/1000.0f);
            accels.z = packet.zacc * (GRAVITY_MSS/1000.0f);
            
            ins.set_gyro(0, gyros);

            ins.set_accel(0, accels);
            compass.setHIL(0, packet.roll, packet.pitch, packet.yaw);
            compass.setHIL(1, packet.roll, packet.pitch, packet.yaw);
            break;
		}
#endif // HIL_MODE

#if CAMERA == ENABLED
    //deprecated. Use MAV_CMD_DO_DIGICAM_CONFIGURE
    case MAVLINK_MSG_ID_DIGICAM_CONFIGURE:
    {
        break;
    }

    //deprecated. Use MAV_CMD_DO_DIGICAM_CONFIGURE
    case MAVLINK_MSG_ID_DIGICAM_CONTROL:
    {
        rover.camera.control_msg(msg);
        rover.log_picture();
        break;
    }
#endif // CAMERA == ENABLED

#if MOUNT == ENABLED
    //deprecated. Use MAV_CMD_DO_MOUNT_CONFIGURE
    case MAVLINK_MSG_ID_MOUNT_CONFIGURE:
		{
			rover.camera_mount.configure_msg(msg);
			break;
		}

    //deprecated. Use MAV_CMD_DO_MOUNT_CONTROL
    case MAVLINK_MSG_ID_MOUNT_CONTROL:
		{
			rover.camera_mount.control_msg(msg);
			break;
		}
#endif // MOUNT == ENABLED

    case MAVLINK_MSG_ID_RADIO:
    case MAVLINK_MSG_ID_RADIO_STATUS:
        {
            handle_radio_status(msg, rover.DataFlash, rover.should_log(MASK_LOG_PM));
            break;
        }

    case MAVLINK_MSG_ID_LOG_REQUEST_DATA:
    case MAVLINK_MSG_ID_LOG_ERASE:
        rover.in_log_download = true;
        /* no break */
    case MAVLINK_MSG_ID_LOG_REQUEST_LIST:
        if (!rover.in_mavlink_delay) {
            handle_log_message(msg, rover.DataFlash);
        }
        break;
    case MAVLINK_MSG_ID_LOG_REQUEST_END:
        rover.in_log_download = false;
        if (!rover.in_mavlink_delay) {
            handle_log_message(msg, rover.DataFlash);
        }
        break;

    case MAVLINK_MSG_ID_SERIAL_CONTROL:
        handle_serial_control(msg, rover.gps);
        break;

    case MAVLINK_MSG_ID_GPS_INJECT_DATA:
        handle_gps_inject(msg, rover.gps);
        break;

    case MAVLINK_MSG_ID_REMOTE_LOG_BLOCK_STATUS:
        rover.DataFlash.remote_log_block_status_msg(chan, msg);
        break;

    case MAVLINK_MSG_ID_AUTOPILOT_VERSION_REQUEST:
        rover.gcs[chan-MAVLINK_COMM_0].send_autopilot_version(FIRMWARE_VERSION);
        break;

    } // end switch
} // end handle mavlink

/*
 *  a delay() callback that processes MAVLink packets. We set this as the
 *  callback in long running library initialisation routines to allow
 *  MAVLink to process packets while waiting for the initialisation to
 *  complete
 */
void Rover::mavlink_delay_cb()
{
    static uint32_t last_1hz, last_50hz, last_5s;
    if (!gcs[0].initialised || in_mavlink_delay) return;

    in_mavlink_delay = true;

    uint32_t tnow = millis();
    if (tnow - last_1hz > 1000) {
        last_1hz = tnow;
        gcs_send_message(MSG_HEARTBEAT);
        gcs_send_message(MSG_EXTENDED_STATUS1);
    }
    if (tnow - last_50hz > 20) {
        last_50hz = tnow;
        gcs_update();
        gcs_data_stream_send();
        notify.update();
    }
    if (tnow - last_5s > 5000) {
        last_5s = tnow;
        gcs_send_text(MAV_SEVERITY_INFO, "Initialising APM");
    }
    check_usb_mux();

    in_mavlink_delay = false;
}

/*
 *  send a message on both GCS links
 */
void Rover::gcs_send_message(enum ap_message id)
{
    for (uint8_t i=0; i<num_gcs; i++) {
        if (gcs[i].initialised) {
            gcs[i].send_message(id);
        }
    }
}

/*
 *  send a mission item reached message and load the index before the send attempt in case it may get delayed
 */
void Rover::gcs_send_mission_item_reached_message(uint16_t mission_index)
{
    for (uint8_t i=0; i<num_gcs; i++) {
        if (gcs[i].initialised) {
            gcs[i].mission_item_reached_index = mission_index;
            gcs[i].send_message(MSG_MISSION_ITEM_REACHED);
        }
    }
}

/*
 *  send data streams in the given rate range on both links
 */
void Rover::gcs_data_stream_send(void)
{
    for (uint8_t i=0; i<num_gcs; i++) {
        if (gcs[i].initialised) {
            gcs[i].data_stream_send();
        }
    }
}

/*
 *  look for incoming commands on the GCS links
 */
void Rover::gcs_update(void)
{
    for (uint8_t i=0; i<num_gcs; i++) {
        if (gcs[i].initialised) {
#if CLI_ENABLED == ENABLED
            gcs[i].update(g.cli_enabled == 1 ? FUNCTOR_BIND_MEMBER(&Rover::run_cli, void, AP_HAL::UARTDriver *) : NULL);
#else
            gcs[i].update(NULL);
#endif
        }
    }
}

void Rover::gcs_send_text(MAV_SEVERITY severity, const char *str)
{
    GCS_MAVLINK::send_statustext(severity, 0xFF, str);
}

/*
 *  send a low priority formatted message to the GCS
 *  only one fits in the queue, so if you send more than one before the
 *  last one gets into the serial buffer then the old one will be lost
 */
void Rover::gcs_send_text_fmt(MAV_SEVERITY severity, const char *fmt, ...)
{
    char str[MAVLINK_MSG_STATUSTEXT_FIELD_TEXT_LEN] {};
    va_list arg_list;
    va_start(arg_list, fmt);
    hal.util->vsnprintf((char *)str, sizeof(str), fmt, arg_list);
    va_end(arg_list);
    GCS_MAVLINK::send_statustext(severity, 0xFF, str);
}


/**
   retry any deferred messages
 */
void Rover::gcs_retry_deferred(void)
{
    gcs_send_message(MSG_RETRY_DEFERRED);
    GCS_MAVLINK::service_statustext();
}
