// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "Plane.h"

const AP_Param::GroupInfo QuadPlane::var_info[] = {

    // @Param: ENABLE
    // @DisplayName: Enable QuadPlane
    // @Description: This enables QuadPlane functionality, assuming quad motors on outputs 5 to 8
    // @Values: 0:Disable,1:Enable
    // @User: Standard
    AP_GROUPINFO_FLAGS("ENABLE", 1, QuadPlane, enable, 0, AP_PARAM_FLAG_ENABLE),

    // @Group: M_
    // @Path: ../libraries/AP_Motors/AP_MotorsMulticopter.cpp
    AP_SUBGROUPPTR(motors, "M_", 2, QuadPlane, AP_MotorsMulticopter),

    // 3 ~ 8 were used by quadplane attitude control PIDs

    // @Param: ANGLE_MAX
    // @DisplayName: Angle Max
    // @Description: Maximum lean angle in all flight modes
    // @Units: Centi-degrees
    // @Range: 1000 8000
    // @User: Advanced
    AP_GROUPINFO("ANGLE_MAX", 10, QuadPlane, aparm.angle_max, 4500),

    // @Param: TRANSITION_MS
    // @DisplayName: Transition time
    // @Description: Transition time in milliseconds after minimum airspeed is reached
    // @Units: milli-seconds
    // @Range: 0 30000
    // @User: Advanced
    AP_GROUPINFO("TRANSITION_MS", 11, QuadPlane, transition_time_ms, 5000),

    // @Param: PZ_P
    // @DisplayName: Position (vertical) controller P gain
    // @Description: Position (vertical) controller P gain.  Converts the difference between the desired altitude and actual altitude into a climb or descent rate which is passed to the throttle rate controller
    // @Range: 1.000 3.000
    // @User: Standard
    AP_SUBGROUPINFO(p_alt_hold, "PZ_", 12, QuadPlane, AC_P),

    // @Param: PXY_P
    // @DisplayName: Position (horizonal) controller P gain
    // @Description: Loiter position controller P gain.  Converts the distance (in the latitude direction) to the target location into a desired speed which is then passed to the loiter latitude rate controller
    // @Range: 0.500 2.000
    // @User: Standard
    AP_SUBGROUPINFO(p_pos_xy,   "PXY_", 13, QuadPlane, AC_P),

    // @Param: VXY_P
    // @DisplayName: Velocity (horizontal) P gain
    // @Description: Velocity (horizontal) P gain.  Converts the difference between desired velocity to a target acceleration
    // @Range: 0.1 6.0
    // @Increment: 0.1
    // @User: Advanced

    // @Param: VXY_I
    // @DisplayName: Velocity (horizontal) I gain
    // @Description: Velocity (horizontal) I gain.  Corrects long-term difference in desired velocity to a target acceleration
    // @Range: 0.02 1.00
    // @Increment: 0.01
    // @User: Advanced

    // @Param: VXY_IMAX
    // @DisplayName: Velocity (horizontal) integrator maximum
    // @Description: Velocity (horizontal) integrator maximum.  Constrains the target acceleration that the I gain will output
    // @Range: 0 4500
    // @Increment: 10
    // @Units: cm/s/s
    // @User: Advanced
    AP_SUBGROUPINFO(pi_vel_xy, "VXY_",  14, QuadPlane, AC_PI_2D),

    // @Param: VZ_P
    // @DisplayName: Velocity (vertical) P gain
    // @Description: Velocity (vertical) P gain.  Converts the difference between desired vertical speed and actual speed into a desired acceleration that is passed to the throttle acceleration controller
    // @Range: 1.000 8.000
    // @User: Standard
    AP_SUBGROUPINFO(p_vel_z,   "VZ_", 15, QuadPlane, AC_P),

    // @Param: AZ_P
    // @DisplayName: Throttle acceleration controller P gain
    // @Description: Throttle acceleration controller P gain.  Converts the difference between desired vertical acceleration and actual acceleration into a motor output
    // @Range: 0.500 1.500
    // @User: Standard

    // @Param: AZ_I
    // @DisplayName: Throttle acceleration controller I gain
    // @Description: Throttle acceleration controller I gain.  Corrects long-term difference in desired vertical acceleration and actual acceleration
    // @Range: 0.000 3.000
    // @User: Standard

    // @Param: AZ_IMAX
    // @DisplayName: Throttle acceleration controller I gain maximum
    // @Description: Throttle acceleration controller I gain maximum.  Constrains the maximum pwm that the I term will generate
    // @Range: 0 1000
    // @Units: Percent*10
    // @User: Standard

    // @Param: AZ_D
    // @DisplayName: Throttle acceleration controller D gain
    // @Description: Throttle acceleration controller D gain.  Compensates for short-term change in desired vertical acceleration vs actual acceleration
    // @Range: 0.000 0.400
    // @User: Standard

    // @Param: AZ_FILT_HZ
    // @DisplayName: Throttle acceleration filter
    // @Description: Filter applied to acceleration to reduce noise.  Lower values reduce noise but add delay.
    // @Range: 1.000 100.000
    // @Units: Hz
    // @User: Standard
    AP_SUBGROUPINFO(pid_accel_z, "AZ_", 16, QuadPlane, AC_PID),

    // @Group: P_
    // @Path: ../libraries/AC_AttitudeControl/AC_PosControl.cpp
    AP_SUBGROUPPTR(pos_control, "P", 17, QuadPlane, AC_PosControl),

    // @Param: VELZ_MAX
    // @DisplayName: Pilot maximum vertical speed
    // @Description: The maximum vertical velocity the pilot may request in cm/s
    // @Units: Centimeters/Second
    // @Range: 50 500
    // @Increment: 10
    // @User: Standard
    AP_GROUPINFO("VELZ_MAX", 18, QuadPlane, pilot_velocity_z_max, 250),
    
    // @Param: ACCEL_Z
    // @DisplayName: Pilot vertical acceleration
    // @Description: The vertical acceleration used when pilot is controlling the altitude
    // @Units: cm/s/s
    // @Range: 50 500
    // @Increment: 10
    // @User: Standard
    AP_GROUPINFO("ACCEL_Z",  19, QuadPlane, pilot_accel_z,  250),

    // @Group: WP_
    // @Path: ../libraries/AC_WPNav/AC_WPNav.cpp
    AP_SUBGROUPPTR(wp_nav, "WP_",  20, QuadPlane, AC_WPNav),

    // @Param: RC_SPEED
    // @DisplayName: RC output speed in Hz
    // @Description: This is the PWM refresh rate in Hz for QuadPlane quad motors
    // @Units: Hz
    // @Range: 50 500
    // @Increment: 10
    // @User: Standard
    AP_GROUPINFO("RC_SPEED", 21, QuadPlane, rc_speed, 490),

    // @Param: THR_MIN_PWM
    // @DisplayName: Minimum PWM output
    // @Description: This is the minimum PWM output for the quad motors
    // @Units: Hz
    // @Range: 800 2200
    // @Increment: 1
    // @User: Standard
    AP_GROUPINFO("THR_MIN_PWM", 22, QuadPlane, thr_min_pwm, 1000),

    // @Param: THR_MAX_PWM
    // @DisplayName: Maximum PWM output
    // @Description: This is the maximum PWM output for the quad motors
    // @Units: Hz
    // @Range: 800 2200
    // @Increment: 1
    // @User: Standard
    AP_GROUPINFO("THR_MAX_PWM", 23, QuadPlane, thr_max_pwm, 2000),

    // @Param: ASSIST_SPEED
    // @DisplayName: Quadplane assistance speed
    // @Description: This is the speed below which the quad motors will provide stability and lift assistance in fixed wing modes. Zero means no assistance except during transition
    // @Units: m/s
    // @Range: 0 100
    // @Increment: 0.1
    // @User: Standard
    AP_GROUPINFO("ASSIST_SPEED", 24, QuadPlane, assist_speed, 0),

    // @Param: YAW_RATE_MAX
    // @DisplayName: Maximum yaw rate
    // @Description: This is the maximum yaw rate in degrees/second
    // @Units: degrees/second
    // @Range: 50 500
    // @Increment: 1
    // @User: Standard
    AP_GROUPINFO("YAW_RATE_MAX", 25, QuadPlane, yaw_rate_max, 100),

    // @Param: LAND_SPEED
    // @DisplayName: Land speed
    // @Description: The descent speed for the final stage of landing in cm/s
    // @Units: cm/s
    // @Range: 30 200
    // @Increment: 10
    // @User: Standard
    AP_GROUPINFO("LAND_SPEED", 26, QuadPlane, land_speed_cms, 50),

    // @Param: LAND_FINAL_ALT
    // @DisplayName: Land final altitude
    // @Description: The altitude at which we should switch to Q_LAND_SPEED descent rate
    // @Units: m
    // @Range: 0.5 50
    // @Increment: 0.1
    // @User: Standard
    AP_GROUPINFO("LAND_FINAL_ALT", 27, QuadPlane, land_final_alt, 6),

    // @Param: THR_MID
    // @DisplayName: Throttle Mid Position
    // @Description: The throttle output (0 ~ 1000) when throttle stick is in mid position.  Used to scale the manual throttle so that the mid throttle stick position is close to the throttle required to hover
    // @User: Standard
    // @Range: 300 700
    // @Units: Percent*10
    // @Increment: 10
    AP_GROUPINFO("THR_MID", 28, QuadPlane, throttle_mid, 500),

    // @Param: TRAN_PIT_MAX
    // @DisplayName: Transition max pitch
    // @Description: Maximum pitch during transition to auto fixed wing flight
    // @User: Standard
    // @Range: 0 30
    // @Units: Degrees
    // @Increment: 1
    AP_GROUPINFO("TRAN_PIT_MAX", 29, QuadPlane, transition_pitch_max, 3),

    // @Param: FRAME_CLASS
    // @DisplayName: Frame Class
    // @Description: Controls major frame class for multicopter component
    // @Values: 0:Quad, 1:Hexa, 2:Octa, 3:OctaQuad
    // @User: Standard
    AP_GROUPINFO("FRAME_CLASS", 30, QuadPlane, frame_class, 0),

    // @Param: FRAME_TYPE
    // @DisplayName: Frame Type (+, X or V)
    // @Description: Controls motor mixing for multicopter component
    // @Values: 0:Plus, 1:X, 2:V, 3:H, 4:V-Tail, 5:A-Tail, 10:Y6B
    // @User: Standard
    AP_GROUPINFO("FRAME_TYPE", 31, QuadPlane, frame_type, 1),
    
    AP_GROUPEND
};

static const struct {
    const char *name;
    float value;
} defaults_table[] = {
    { "Q_A_RAT_RLL_P",    0.25 },
    { "Q_A_RAT_RLL_I",    0.25 },
    { "Q_A_RAT_RLL_FILT", 10.0 },
    { "Q_A_RAT_PIT_P",    0.25 },
    { "Q_A_RAT_PIT_I",    0.25 },
    { "Q_A_RAT_PIT_FILT", 10.0 },
};

QuadPlane::QuadPlane(AP_AHRS_NavEKF &_ahrs) :
    ahrs(_ahrs)
{
    AP_Param::setup_object_defaults(this, var_info);
}


// setup default motors for the frame class
void QuadPlane::setup_default_channels(uint8_t num_motors)
{
    for (uint8_t i=0; i<num_motors; i++) {
        RC_Channel_aux::set_aux_channel_default((RC_Channel_aux::Aux_servo_function_t)(RC_Channel_aux::k_motor1+i), CH_5+i);
    }
}
    

bool QuadPlane::setup(void)
{
    uint16_t mask;
    if (initialised) {
        return true;
    }
    if (!enable || hal.util->get_soft_armed()) {
        return false;
    }
    float loop_delta_t = 1.0 / plane.scheduler.get_loop_rate_hz();
    
    if (hal.util->available_memory() <
        4096 + sizeof(*motors) + sizeof(*attitude_control) + sizeof(*pos_control) + sizeof(*wp_nav)) {
        GCS_MAVLINK::send_statustext_all(MAV_SEVERITY_INFO, "Not enough memory for quadplane");
        goto failed;
    }

    /*
      dynamically allocate the key objects for quadplane. This ensures
      that the objects don't affect the vehicle unless enabled and
      also saves memory when not in use
     */
    switch ((enum frame_class)frame_class.get()) {
    case FRAME_CLASS_QUAD:
        setup_default_channels(4);
        motors = new AP_MotorsQuad(plane.scheduler.get_loop_rate_hz());
        break;
    case FRAME_CLASS_HEXA:
        setup_default_channels(6);
        motors = new AP_MotorsHexa(plane.scheduler.get_loop_rate_hz());
        break;
    case FRAME_CLASS_OCTA:
        setup_default_channels(8);
        motors = new AP_MotorsOcta(plane.scheduler.get_loop_rate_hz());
        break;
    case FRAME_CLASS_OCTAQUAD:
        setup_default_channels(8);
         motors = new AP_MotorsOctaQuad(plane.scheduler.get_loop_rate_hz());
        break;
    default:
        hal.console->printf("Unknown frame class %u\n", (unsigned)frame_class.get());
        goto failed;
    }
    if (!motors) {
        hal.console->printf("Unable to allocate motors\n");
        goto failed;
    }
    
    AP_Param::load_object_from_eeprom(motors, motors->var_info);
    attitude_control = new AC_AttitudeControl_Multi(ahrs, aparm, *motors, loop_delta_t);
    if (!attitude_control) {
        hal.console->printf("Unable to allocate attitude_control\n");
        goto failed;
    }
    AP_Param::load_object_from_eeprom(attitude_control, attitude_control->var_info);
    pos_control = new AC_PosControl(ahrs, inertial_nav, *motors, *attitude_control,
                                    p_alt_hold, p_vel_z, pid_accel_z,
                                    p_pos_xy, pi_vel_xy);
    if (!pos_control) {
        hal.console->printf("Unable to allocate pos_control\n");
        goto failed;
    }
    AP_Param::load_object_from_eeprom(pos_control, pos_control->var_info);
    wp_nav = new AC_WPNav(inertial_nav, ahrs, *pos_control, *attitude_control);
    if (!pos_control) {
        hal.console->printf("Unable to allocate wp_nav\n");
        goto failed;
    }
    AP_Param::load_object_from_eeprom(wp_nav, wp_nav->var_info);

    motors->set_frame_orientation(frame_type);
    motors->Init();
    motors->set_throttle_range(100, thr_min_pwm, thr_max_pwm);
    motors->set_hover_throttle(throttle_mid);
    motors->set_update_rate(rc_speed);
    motors->set_interlock(true);
    pid_accel_z.set_dt(loop_delta_t);
    pos_control->set_dt(loop_delta_t);

    // setup the trim of any motors used by AP_Motors so px4io
    // failsafe will disable motors
    mask = motors->get_motor_mask();
    for (uint8_t i=0; i<16; i++) {
        if (mask & 1U<<i) {
            RC_Channel *ch = RC_Channel::rc_channel(i);
            if (ch != nullptr) {
                ch->radio_trim = thr_min_pwm;
            }
        }
    }

#if CONFIG_HAL_BOARD == HAL_BOARD_PX4
    // redo failsafe mixing on px4
    plane.setup_failsafe_mixing();
#endif
    
    transition_state = TRANSITION_DONE;

    setup_defaults();
    
    GCS_MAVLINK::send_statustext_all(MAV_SEVERITY_INFO, "QuadPlane initialised");
    initialised = true;
    return true;
    
failed:
    initialised = false;
    enable.set(0);
    GCS_MAVLINK::send_statustext_all(MAV_SEVERITY_INFO, "QuadPlane setup failed");
    return false;
}

/*
  setup default parameters from defaults_table
 */
void QuadPlane::setup_defaults(void)
{
    for (uint8_t i=0; i<ARRAY_SIZE(defaults_table); i++) {
        if (!AP_Param::set_default_by_name(defaults_table[i].name, defaults_table[i].value)) {
            GCS_MAVLINK::send_statustext_all(MAV_SEVERITY_INFO, "QuadPlane setup failure for %s",
                                             defaults_table[i].name);
            AP_HAL::panic("quadplane bad default %s", defaults_table[i].name);
        }
    }
}

// init quadplane stabilize mode 
void QuadPlane::init_stabilize(void)
{
    throttle_wait = false;
}

// hold in stabilize with given throttle
void QuadPlane::hold_stabilize(float throttle_in)
{    
    // call attitude controller
    attitude_control->input_euler_angle_roll_pitch_euler_rate_yaw_smooth(plane.nav_roll_cd,
                                                                         plane.nav_pitch_cd,
                                                                         get_desired_yaw_rate_cds(),
                                                                         smoothing_gain);

    if (throttle_in <= 0) {
        motors->set_desired_spool_state(AP_Motors::DESIRED_SPIN_WHEN_ARMED);
        attitude_control->set_throttle_out_unstabilized(0, true, 0);
    } else {
        motors->set_desired_spool_state(AP_Motors::DESIRED_THROTTLE_UNLIMITED);
        attitude_control->set_throttle_out(throttle_in, true, 0);
    }
}

// quadplane stabilize mode
void QuadPlane::control_stabilize(void)
{
    float pilot_throttle_scaled = plane.channel_throttle->control_in / 100.0f;
    hold_stabilize(pilot_throttle_scaled);

}

// init quadplane hover mode 
void QuadPlane::init_hover(void)
{
    // initialize vertical speeds and leash lengths
    pos_control->set_speed_z(-pilot_velocity_z_max, pilot_velocity_z_max);
    pos_control->set_accel_z(pilot_accel_z);

    // initialise position and desired velocity
    pos_control->set_alt_target(inertial_nav.get_altitude());
    pos_control->set_desired_velocity_z(inertial_nav.get_velocity_z());

    init_throttle_wait();
}

/*
  hold hover with target climb rate
 */
void QuadPlane::hold_hover(float target_climb_rate)
{
    // motors use full range
    motors->set_desired_spool_state(AP_Motors::DESIRED_THROTTLE_UNLIMITED);

    // initialize vertical speeds and acceleration
    pos_control->set_speed_z(-pilot_velocity_z_max, pilot_velocity_z_max);
    pos_control->set_accel_z(pilot_accel_z);

    // call attitude controller
    attitude_control->input_euler_angle_roll_pitch_euler_rate_yaw_smooth(plane.nav_roll_cd,
                                                                         plane.nav_pitch_cd,
                                                                         get_desired_yaw_rate_cds(),
                                                                         smoothing_gain);

    // call position controller
    pos_control->set_alt_target_from_climb_rate_ff(target_climb_rate, plane.G_Dt, false);
    pos_control->update_z_controller();

}

/*
  control QHOVER mode
 */
void QuadPlane::control_hover(void)
{
    if (throttle_wait) {
        motors->set_desired_spool_state(AP_Motors::DESIRED_SPIN_WHEN_ARMED);
        attitude_control->set_throttle_out_unstabilized(0, true, 0);
        pos_control->relax_alt_hold_controllers(0);
    } else {
        hold_hover(get_pilot_desired_climb_rate_cms());
    }
}

void QuadPlane::init_loiter(void)
{
    // set target to current position
    wp_nav->init_loiter_target();

    // initialize vertical speed and acceleration
    pos_control->set_speed_z(-pilot_velocity_z_max, pilot_velocity_z_max);
    pos_control->set_accel_z(pilot_accel_z);

    // initialise position and desired velocity
    pos_control->set_alt_target(inertial_nav.get_altitude());
    pos_control->set_desired_velocity_z(inertial_nav.get_velocity_z());

    init_throttle_wait();
}

void QuadPlane::init_land(void)
{
    init_loiter();
    throttle_wait = false;
    land_state = QLAND_DESCEND;
    motors_lower_limit_start_ms = 0;
}


// helper for is_flying()
bool QuadPlane::is_flying(void)
{
    if (!available()) {
        return false;
    }
    if (motors->get_throttle() > 0.2 && !motors->limit.throttle_lower) {
        return true;
    }
    return false;
}

// crude landing detector to prevent tipover
bool QuadPlane::should_relax(void)
{
    bool motor_at_lower_limit = motors->limit.throttle_lower && motors->is_throttle_mix_min();
    if (motors->get_throttle() < 0.01) {
        motor_at_lower_limit = true;
    }
    if (!motor_at_lower_limit) {
        motors_lower_limit_start_ms = 0;
    }
    if (motor_at_lower_limit && motors_lower_limit_start_ms == 0) {
        motors_lower_limit_start_ms = millis();
    }
    bool relax_loiter = motors_lower_limit_start_ms != 0 && (millis() - motors_lower_limit_start_ms) > 1000;
    return relax_loiter;
}

/*
  smooth out descent rate for landing to prevent a jerk as we get to
  land_final_alt. 
 */
float QuadPlane::landing_descent_rate_cms(float height_above_ground)
{
    float ret = linear_interpolate(land_speed_cms, wp_nav->get_speed_down(),
                                   height_above_ground,
                                   land_final_alt, land_final_alt+3);
    return ret;
}


// run quadplane loiter controller
void QuadPlane::control_loiter()
{
    if (throttle_wait) {
        motors->set_desired_spool_state(AP_Motors::DESIRED_SPIN_WHEN_ARMED);
        attitude_control->set_throttle_out_unstabilized(0, true, 0);
        pos_control->relax_alt_hold_controllers(0);
        wp_nav->init_loiter_target();
        return;
    }


    if (should_relax()) {
        wp_nav->loiter_soften_for_landing();
    }

    if (millis() - last_loiter_ms > 500) {
        wp_nav->init_loiter_target();
    }
    last_loiter_ms = millis();

    // motors use full range
    motors->set_desired_spool_state(AP_Motors::DESIRED_THROTTLE_UNLIMITED);

    // initialize vertical speed and acceleration
    pos_control->set_speed_z(-pilot_velocity_z_max, pilot_velocity_z_max);
    pos_control->set_accel_z(pilot_accel_z);

    // process pilot's roll and pitch input
    wp_nav->set_pilot_desired_acceleration(plane.channel_roll->control_in,
                                           plane.channel_pitch->control_in);

    // Update EKF speed limit - used to limit speed when we are using optical flow
    float ekfGndSpdLimit, ekfNavVelGainScaler;    
    ahrs.getEkfControlLimits(ekfGndSpdLimit, ekfNavVelGainScaler);
    
    // run loiter controller
    wp_nav->update_loiter(ekfGndSpdLimit, ekfNavVelGainScaler);

    // call attitude controller
    attitude_control->input_euler_angle_roll_pitch_euler_rate_yaw(wp_nav->get_roll(),
                                                                  wp_nav->get_pitch(),
                                                                  get_desired_yaw_rate_cds());

    // nav roll and pitch are controller by loiter controller
    plane.nav_roll_cd = wp_nav->get_roll();
    plane.nav_pitch_cd = wp_nav->get_pitch();

    if (plane.control_mode == QLAND) {
        float height_above_ground;
        if (plane.g.rangefinder_landing && plane.rangefinder_state.in_range) {
            height_above_ground = plane.rangefinder_state.height_estimate;
        } else {
            height_above_ground = plane.adjusted_relative_altitude_cm() * 0.01;
        }
        if (height_above_ground < land_final_alt && land_state < QLAND_FINAL) {
            land_state = QLAND_FINAL;
        }
        float descent_rate = (land_state == QLAND_FINAL)? land_speed_cms:landing_descent_rate_cms(height_above_ground);
        pos_control->set_alt_target_from_climb_rate(-descent_rate, plane.G_Dt, true);
        check_land_complete();
    } else {
        // update altitude target and call position controller
        pos_control->set_alt_target_from_climb_rate_ff(get_pilot_desired_climb_rate_cms(), plane.G_Dt, false);
    }
    pos_control->update_z_controller();
}

/*
  get pilot input yaw rate in cd/s
 */
float QuadPlane::get_pilot_input_yaw_rate_cds(void)
{
    if (plane.channel_throttle->control_in <= 0 && !plane.auto_throttle_mode) {
        // the user may be trying to disarm
        return 0;
    }

    // add in rudder input
    return plane.channel_rudder->norm_input() * 100 * yaw_rate_max;
}

/*
  get overall desired yaw rate in cd/s
 */
float QuadPlane::get_desired_yaw_rate_cds(void)
{
    float yaw_cds = 0;
    if (assisted_flight) {
        // use bank angle to get desired yaw rate
        yaw_cds += desired_auto_yaw_rate_cds();
    }
    if (plane.channel_throttle->control_in <= 0 && !plane.auto_throttle_mode) {
        // the user may be trying to disarm
        return 0;
    }
    // add in pilot input
    yaw_cds += get_pilot_input_yaw_rate_cds();
    return yaw_cds;
}

// get pilot desired climb rate in cm/s
float QuadPlane::get_pilot_desired_climb_rate_cms(void)
{
    if (plane.failsafe.ch3_failsafe || plane.failsafe.ch3_counter > 0) {
        // descend at 0.5m/s for now
        return -50;
    }
    uint16_t dead_zone = plane.channel_throttle->get_dead_zone();
    uint16_t trim = (plane.channel_throttle->radio_max + plane.channel_throttle->radio_min)/2;
    return pilot_velocity_z_max * plane.channel_throttle->pwm_to_angle_dz_trim(dead_zone, trim) / 100.0f;
}


/*
  initialise throttle_wait based on throttle and is_flying()
 */
void QuadPlane::init_throttle_wait(void)
{
    if (plane.channel_throttle->control_in >= 10 ||
        plane.is_flying()) {
        throttle_wait = false;
    } else {
        throttle_wait = true;        
    }
}
    
// set motor arming
void QuadPlane::set_armed(bool armed)
{
    if (!initialised) {
        return;
    }
    motors->armed(armed);
    if (armed) {
        motors->enable();
    }
}


/*
  estimate desired climb rate for assistance (in cm/s)
 */
float QuadPlane::assist_climb_rate_cms(void)
{
    float climb_rate;
    if (plane.auto_throttle_mode) {
        // use altitude_error_cm, spread over 10s interval
        climb_rate = plane.altitude_error_cm / 10;
    } else {
        // otherwise estimate from pilot input
        climb_rate = plane.g.flybywire_climb_rate * (plane.nav_pitch_cd/(float)plane.aparm.pitch_limit_max_cd);
        climb_rate *= plane.channel_throttle->control_in;
    }
    climb_rate = constrain_float(climb_rate, -wp_nav->get_speed_down(), wp_nav->get_speed_up());
    return climb_rate;
}

/*
  calculate desired yaw rate for assistance
 */
float QuadPlane::desired_auto_yaw_rate_cds(void)
{
    float aspeed;
    if (!ahrs.airspeed_estimate(&aspeed) || aspeed < plane.aparm.airspeed_min) {
        aspeed = plane.aparm.airspeed_min;
    }
    if (aspeed < 1) {
        aspeed = 1;
    }
    float yaw_rate = degrees(GRAVITY_MSS * tanf(radians(plane.nav_roll_cd*0.01f))/aspeed) * 100;
    return yaw_rate;
}

/*
  update for transition from quadplane to fixed wing mode
 */
void QuadPlane::update_transition(void)
{
    if (plane.control_mode == MANUAL ||
        plane.control_mode == ACRO ||
        plane.control_mode == TRAINING) {
        // in manual modes quad motors are always off
        motors->set_desired_spool_state(AP_Motors::DESIRED_SHUT_DOWN);
        motors->output();
        transition_state = TRANSITION_DONE;
        return;
    }

    float aspeed;
    bool have_airspeed = ahrs.airspeed_estimate(&aspeed);
    
    /*
      see if we should provide some assistance
     */
    if (have_airspeed && aspeed < assist_speed &&
        (plane.auto_throttle_mode ||
         plane.channel_throttle->control_in>0 ||
         plane.is_flying())) {
        // the quad should provide some assistance to the plane
        transition_state = TRANSITION_AIRSPEED_WAIT;
        transition_start_ms = millis();
        assisted_flight = true;
    } else {
        assisted_flight = false;
    }

    if (transition_state < TRANSITION_TIMER) {
        // set a single loop pitch limit in TECS
        plane.TECS_controller.set_pitch_max_limit(transition_pitch_max);
    } else if (transition_state < TRANSITION_DONE) {
        plane.TECS_controller.set_pitch_max_limit((transition_pitch_max+1)*2);
    }
    
    switch (transition_state) {
    case TRANSITION_AIRSPEED_WAIT: {
        motors->set_desired_spool_state(AP_Motors::DESIRED_THROTTLE_UNLIMITED);
        // we hold in hover until the required airspeed is reached
        if (transition_start_ms == 0) {
            GCS_MAVLINK::send_statustext_all(MAV_SEVERITY_INFO, "Transition airspeed wait");
            transition_start_ms = millis();
        }

        if (have_airspeed && aspeed > plane.aparm.airspeed_min && !assisted_flight) {
            transition_start_ms = millis();
            transition_state = TRANSITION_TIMER;
            GCS_MAVLINK::send_statustext_all(MAV_SEVERITY_INFO, "Transition airspeed reached %.1f", (double)aspeed);
        }
        assisted_flight = true;
        hold_hover(assist_climb_rate_cms());
        attitude_control->rate_controller_run();
        motors_output();
        last_throttle = motors->get_throttle();
        break;
    }
        
    case TRANSITION_TIMER: {
        motors->set_desired_spool_state(AP_Motors::DESIRED_THROTTLE_UNLIMITED);
        // after airspeed is reached we degrade throttle over the
        // transition time, but continue to stabilize
        if (millis() - transition_start_ms > (unsigned)transition_time_ms) {
            transition_state = TRANSITION_DONE;
            GCS_MAVLINK::send_statustext_all(MAV_SEVERITY_INFO, "Transition done");
        }
        float throttle_scaled = last_throttle * (transition_time_ms - (millis() - transition_start_ms)) / (float)transition_time_ms;
        if (throttle_scaled < 0) {
            throttle_scaled = 0;
        }
        assisted_flight = true;
        hold_stabilize(throttle_scaled);
        attitude_control->rate_controller_run();
        motors_output();
        break;
    }

    case TRANSITION_DONE:
        motors->set_desired_spool_state(AP_Motors::DESIRED_SHUT_DOWN);
        motors->output();
        break;
    }
}

/*
  update motor output for quadplane
 */
void QuadPlane::update(void)
{
    if (!setup()) {
        return;
    }

    if (motor_test.running) {
        motor_test_output();
        return;
    }
    
    if (!in_vtol_mode()) {
        update_transition();
    } else {
        assisted_flight = false;
        
        // run low level rate controllers
        attitude_control->rate_controller_run();

        // output to motors
        motors_output();
        transition_start_ms = 0;
        if (throttle_wait && !plane.is_flying()) {
            transition_state = TRANSITION_DONE;
        } else {
            transition_state = TRANSITION_AIRSPEED_WAIT;
        }
        last_throttle = motors->get_throttle();
    }

    // disable throttle_wait when throttle rises above 10%
    if (throttle_wait &&
        (plane.channel_throttle->control_in > 10 ||
         plane.failsafe.ch3_failsafe ||
         plane.failsafe.ch3_counter>0)) {
        throttle_wait = false;
    }
}

/*
  output motors and do any copter needed
 */
void QuadPlane::motors_output(void)
{
    motors->output();
    if (motors->armed()) {
        plane.DataFlash.Log_Write_Rate(plane.ahrs, *motors, *attitude_control, *pos_control);
        Log_Write_QControl_Tuning();
    }
}

/*
  update control mode for quadplane modes
 */
void QuadPlane::control_run(void)
{
    if (!initialised) {
        return;
    }

    switch (plane.control_mode) {
    case QSTABILIZE:
        control_stabilize();
        break;
    case QHOVER:
        control_hover();
        break;
    case QLOITER:
    case QLAND:
        control_loiter();
    default:
        break;
    }
    // we also stabilize using fixed wing surfaces
    float speed_scaler = plane.get_speed_scaler();
    plane.stabilize_roll(speed_scaler);
    plane.stabilize_pitch(speed_scaler);
}

/*
  enter a quadplane mode
 */
bool QuadPlane::init_mode(void)
{
    if (!setup()) {
        return false;
    }
    if (!initialised) {
        GCS_MAVLINK::send_statustext_all(MAV_SEVERITY_CRITICAL, "QuadPlane mode refused");        
        return false;
    }
    switch (plane.control_mode) {
    case QSTABILIZE:
        init_stabilize();
        break;
    case QHOVER:
        init_hover();
        break;
    case QLOITER:
        init_loiter();
        break;
    case QLAND:
        init_land();
        break;
    default:
        break;
    }
    return true;
}

/*
  handle a MAVLink DO_VTOL_TRANSITION
 */
bool QuadPlane::handle_do_vtol_transition(const mavlink_command_long_t &packet)
{
    if (!available()) {
        plane.gcs_send_text_fmt(MAV_SEVERITY_NOTICE, "VTOL not available");
        return MAV_RESULT_FAILED;
    }
    if (plane.control_mode != AUTO) {
        plane.gcs_send_text_fmt(MAV_SEVERITY_NOTICE, "VTOL transition only in AUTO");
        return MAV_RESULT_FAILED;
    }
    switch ((uint8_t)packet.param1) {
    case MAV_VTOL_STATE_MC:
        if (!plane.auto_state.vtol_mode) {
            plane.gcs_send_text_fmt(MAV_SEVERITY_NOTICE, "Entered VTOL mode");
        }
        plane.auto_state.vtol_mode = true;
        return MAV_RESULT_ACCEPTED;
    case MAV_VTOL_STATE_FW:
        if (plane.auto_state.vtol_mode) {
            plane.gcs_send_text_fmt(MAV_SEVERITY_NOTICE, "Exited VTOL mode");
        }
        plane.auto_state.vtol_mode = false;
        return MAV_RESULT_ACCEPTED;
    }

    plane.gcs_send_text_fmt(MAV_SEVERITY_NOTICE, "Invalid VTOL mode");
    return MAV_RESULT_FAILED;
}

/*
  are we in a VTOL auto state?
 */
bool QuadPlane::in_vtol_auto(void)
{
    if (plane.control_mode != AUTO) {
        return false;
    }
    if (plane.auto_state.vtol_mode) {
        return true;
    }
    switch (plane.mission.get_current_nav_cmd().id) {
    case MAV_CMD_NAV_VTOL_LAND:
    case MAV_CMD_NAV_VTOL_TAKEOFF:
        return true;
    default:
        return false;
    }
}

/*
  are we in a VTOL mode?
 */
bool QuadPlane::in_vtol_mode(void)
{
    return (plane.control_mode == QSTABILIZE ||
            plane.control_mode == QHOVER ||
            plane.control_mode == QLOITER ||
            plane.control_mode == QLAND ||
            in_vtol_auto());
}

/*
  handle auto-mode when auto_state.vtol_mode is true
 */
void QuadPlane::control_auto(const Location &loc)
{
    if (!setup()) {
        return;
    }

    motors->set_desired_spool_state(AP_Motors::DESIRED_THROTTLE_UNLIMITED);

    Location origin = inertial_nav.get_origin();
    Vector2f diff2d;
    Vector3f target;
    diff2d = location_diff(origin, loc);
    target.x = diff2d.x * 100;
    target.y = diff2d.y * 100;
    target.z = loc.alt - origin.alt;

    if (!locations_are_same(loc, last_auto_target) ||
        loc.alt != last_auto_target.alt ||
        millis() - last_loiter_ms > 500) {
        wp_nav->set_wp_destination(target);
        last_auto_target = loc;
    }
    last_loiter_ms = millis();
    
    // initialize vertical speed and acceleration
    pos_control->set_speed_z(-pilot_velocity_z_max, pilot_velocity_z_max);
    pos_control->set_accel_z(pilot_accel_z);

    if (plane.mission.get_current_nav_cmd().id == MAV_CMD_NAV_VTOL_TAKEOFF) {
        /*
          for takeoff we need to use the loiter controller wpnav controller takes over the descent rate
          control
         */
        float ekfGndSpdLimit, ekfNavVelGainScaler;    
        ahrs.getEkfControlLimits(ekfGndSpdLimit, ekfNavVelGainScaler);
    
        // run loiter controller
        wp_nav->update_loiter(ekfGndSpdLimit, ekfNavVelGainScaler);

        attitude_control->input_euler_angle_roll_pitch_euler_rate_yaw_smooth(plane.nav_roll_cd,
                                                                             plane.nav_pitch_cd,
                                                                             get_pilot_input_yaw_rate_cds(),
                                                                             smoothing_gain);

        // nav roll and pitch are controller by position controller
        plane.nav_roll_cd = pos_control->get_roll();
        plane.nav_pitch_cd = pos_control->get_pitch();
    } else if (plane.mission.get_current_nav_cmd().id == MAV_CMD_NAV_VTOL_LAND &&
               land_state >= QLAND_FINAL) {
        /*
          for land-final we use the loiter controller
         */
        float ekfGndSpdLimit, ekfNavVelGainScaler;    
        ahrs.getEkfControlLimits(ekfGndSpdLimit, ekfNavVelGainScaler);
    
        // run loiter controller
        wp_nav->update_loiter(ekfGndSpdLimit, ekfNavVelGainScaler);

        attitude_control->input_euler_angle_roll_pitch_euler_rate_yaw_smooth(plane.nav_roll_cd,
                                                                             plane.nav_pitch_cd,
                                                                             get_pilot_input_yaw_rate_cds(),
                                                                             smoothing_gain);
        // nav roll and pitch are controller by position controller
        plane.nav_roll_cd = pos_control->get_roll();
        plane.nav_pitch_cd = pos_control->get_pitch();
    } else if (plane.mission.get_current_nav_cmd().id == MAV_CMD_NAV_VTOL_LAND &&
               land_state == QLAND_POSITION1) {
        /*
          for initial land repositioning and descent we run the
          velocity controller. This allows us to smoothly control the
          velocity change from fixed wing flight to VTOL flight
         */
        float ekfGndSpdLimit, ekfNavVelGainScaler;    
        ahrs.getEkfControlLimits(ekfGndSpdLimit, ekfNavVelGainScaler);

        Vector2f diff_wp;
        diff_wp = location_diff(plane.current_loc, loc);
        float distance = diff_wp.length();

        if (land_speed_scale <= 0) {
            // initialise scaling so we start off targeting our
            // current linear speed towards the target. If this is
            // less than the wpnav speed then the wpnav speed is used
            // land_speed_scale is then used to linearly change
            // velocity as we approach the waypoint, aiming for zero
            // speed at the waypoint
            Vector2f groundspeed = ahrs.groundspeed_vector();
            // newdiff is the delta to our target if we keep going for one second
            Vector2f newdiff = diff_wp - groundspeed;
            // speed towards target is the change in distance to target over one second
            float speed_towards_target = diff_wp.length() - newdiff.length();
            // setup land_speed_scale so at current distance we maintain speed towards target, and slow down as
            // we approach
            land_speed_scale = MAX(speed_towards_target, wp_nav->get_speed_xy() * 0.01) / MAX(distance, 1);
        }

        // set target vertical velocity to zero. The aircraft may
        // naturally try to climb or descend a bit depending on its
        // approach speed and angle. We let it do that, just asking
        // the quad motors to aim for zero climb rate
        pos_control->set_desired_velocity_z(0);

        // set the desired XY velocity to bring us to the waypoint
        pos_control->set_desired_velocity_xy(diff_wp.x*land_speed_scale*100,
                                             diff_wp.y*land_speed_scale*100);
        pos_control->update_vel_controller_xyz(ekfNavVelGainScaler);
        
        // nav roll and pitch are controller by position controller
        plane.nav_roll_cd = pos_control->get_roll();
        plane.nav_pitch_cd = pos_control->get_pitch();

        // initially constrain pitch so we don't nose down too
        // much. The velocity controller tends to want to nose down
        // initially
        land_wp_proportion = constrain_float(MAX(land_wp_proportion, plane.auto_state.wp_proportion), 0, 1);
        int32_t limit = MIN(land_wp_proportion * plane.aparm.pitch_limit_min_cd, -500);
        plane.nav_pitch_cd = constrain_int32(plane.nav_pitch_cd, limit, plane.aparm.pitch_limit_max_cd);
        
        // call attitude controller
        attitude_control->input_euler_angle_roll_pitch_euler_rate_yaw_smooth(plane.nav_roll_cd,
                                                                             plane.nav_pitch_cd,
                                                                             get_pilot_input_yaw_rate_cds(),
                                                                             smoothing_gain);
        if (distance < 5 || plane.auto_state.wp_proportion >= 1) {
            // move to loiter controller at 5m or if we overshoot the waypoint
            land_state = QLAND_POSITION2;
            plane.gcs_send_text_fmt(MAV_SEVERITY_INFO,"Land position2 started v=%.1f d=%.1f",
                                    ahrs.groundspeed(), distance);
        }
    } else if (plane.mission.get_current_nav_cmd().id == MAV_CMD_NAV_VTOL_LAND) {
        /*
          for final land repositioning and descent we run the loiter controller
         */
        
        // also run fixed wing navigation
        plane.nav_controller->update_waypoint(plane.prev_WP_loc, plane.next_WP_loc);

        pos_control->set_xy_target(target.x, target.y);
        
        float ekfGndSpdLimit, ekfNavVelGainScaler;    
        ahrs.getEkfControlLimits(ekfGndSpdLimit, ekfNavVelGainScaler);
        
        // run loiter controller
        wp_nav->update_loiter(ekfGndSpdLimit, ekfNavVelGainScaler);

        // nav roll and pitch are controller by position controller
        plane.nav_roll_cd = wp_nav->get_roll();
        plane.nav_pitch_cd = wp_nav->get_pitch();

        // call attitude controller
        attitude_control->input_euler_angle_roll_pitch_euler_rate_yaw_smooth(plane.nav_roll_cd,
                                                                             plane.nav_pitch_cd,
                                                                             get_pilot_input_yaw_rate_cds(),
                                                                             smoothing_gain);
    } else {
        /*
          this is full copter control of auto flight
         */
        // run wpnav controller
        wp_nav->update_wpnav();

        // call attitude controller
        attitude_control->input_euler_angle_roll_pitch_yaw(wp_nav->get_roll(),
                                                           wp_nav->get_pitch(),
                                                           wp_nav->get_yaw(),
                                                           true);
        // nav roll and pitch are controller by loiter controller
        plane.nav_roll_cd = wp_nav->get_roll();
        plane.nav_pitch_cd = wp_nav->get_pitch();
    }


    switch (plane.mission.get_current_nav_cmd().id) {
    case MAV_CMD_NAV_VTOL_LAND:
        if (land_state <= QLAND_POSITION2) {
            pos_control->set_alt_target_from_climb_rate(0, plane.G_Dt, false); 
        } else if (land_state > QLAND_POSITION2 && land_state < QLAND_FINAL) {
            float height_above_ground = (plane.current_loc.alt - plane.next_WP_loc.alt)*0.01;
            pos_control->set_alt_target_from_climb_rate(-landing_descent_rate_cms(height_above_ground),
                                                        plane.G_Dt, true);
        } else {
            pos_control->set_alt_target_from_climb_rate(-land_speed_cms, plane.G_Dt, true);
        }
        break;
    case MAV_CMD_NAV_VTOL_TAKEOFF:
        pos_control->set_alt_target_from_climb_rate(100, plane.G_Dt, true);            
        break;
    default:
        pos_control->set_alt_target_from_climb_rate_ff(assist_climb_rate_cms(), plane.G_Dt, false);
        break;
    }
    
    pos_control->update_z_controller();
}

/*
  start a VTOL takeoff
 */
bool QuadPlane::do_vtol_takeoff(const AP_Mission::Mission_Command& cmd)
{
    if (!setup()) {
        return false;
    }

    plane.set_next_WP(cmd.content.location);
    plane.next_WP_loc.alt = plane.current_loc.alt + cmd.content.location.alt;
    throttle_wait = false;

    // set target to current position
    wp_nav->init_loiter_target();

    // initialize vertical speed and acceleration
    pos_control->set_speed_z(-pilot_velocity_z_max, pilot_velocity_z_max);
    pos_control->set_accel_z(pilot_accel_z);

    // initialise position and desired velocity
    pos_control->set_alt_target(inertial_nav.get_altitude());
    pos_control->set_desired_velocity_z(inertial_nav.get_velocity_z());
    
    // also update nav_controller for status output
    plane.nav_controller->update_waypoint(plane.prev_WP_loc, plane.next_WP_loc);
    return true;
}


/*
  start a VTOL landing
 */
bool QuadPlane::do_vtol_land(const AP_Mission::Mission_Command& cmd)
{
    if (!setup()) {
        return false;
    }
    attitude_control->get_rate_roll_pid().reset_I();
    attitude_control->get_rate_pitch_pid().reset_I();
    attitude_control->get_rate_yaw_pid().reset_I();
    pid_accel_z.reset_I();
    pi_vel_xy.reset_I();
    
    plane.set_next_WP(cmd.content.location);
    // initially aim for current altitude
    plane.next_WP_loc.alt = plane.current_loc.alt;
    land_state = QLAND_POSITION1;
    land_speed_scale = 0;
    pos_control->init_vel_controller_xyz();
    
    throttle_wait = false;
    land_yaw_cd = get_bearing_cd(plane.prev_WP_loc, plane.next_WP_loc);
    land_wp_proportion = 0;
    motors_lower_limit_start_ms = 0;
    Location origin = inertial_nav.get_origin();
    Vector2f diff2d;
    Vector3f target;
    diff2d = location_diff(origin, plane.next_WP_loc);
    target.x = diff2d.x * 100;
    target.y = diff2d.y * 100;
    target.z = plane.next_WP_loc.alt - origin.alt;
    wp_nav->set_wp_origin_and_destination(inertial_nav.get_position(), target);
    pos_control->set_alt_target(inertial_nav.get_altitude());
    
    // also update nav_controller for status output
    plane.nav_controller->update_waypoint(plane.prev_WP_loc, plane.next_WP_loc);
    return true;
}

/*
  check if a VTOL takeoff has completed
 */
bool QuadPlane::verify_vtol_takeoff(const AP_Mission::Mission_Command &cmd)
{
    if (!available()) {
        return true;
    }
    if (plane.current_loc.alt < plane.next_WP_loc.alt) {
        return false;
    }
    transition_state = TRANSITION_AIRSPEED_WAIT;
    plane.TECS_controller.set_pitch_max_limit(transition_pitch_max);
    return true;
}

void QuadPlane::check_land_complete(void)
{
    if (land_state == QLAND_FINAL &&
        (motors_lower_limit_start_ms != 0 &&
         millis() - motors_lower_limit_start_ms > 5000)) {
        plane.disarm_motors();
        land_state = QLAND_COMPLETE;
        plane.gcs_send_text(MAV_SEVERITY_INFO,"Land complete");
    }
}

/*
  check if a VTOL landing has completed
 */
bool QuadPlane::verify_vtol_land(const AP_Mission::Mission_Command &cmd)
{
    if (!available()) {
        return true;
    }
    if (land_state == QLAND_POSITION2 &&
        plane.auto_state.wp_distance < 2) {
        land_state = QLAND_DESCEND;
        plane.gcs_send_text(MAV_SEVERITY_INFO,"Land descend started");
        plane.set_next_WP(cmd.content.location);        
    }

    if (should_relax()) {
        wp_nav->loiter_soften_for_landing();
    }

    // at land_final_alt begin final landing
    if (land_state == QLAND_DESCEND &&
        plane.current_loc.alt < plane.next_WP_loc.alt+land_final_alt*100) {
        land_state = QLAND_FINAL;
        pos_control->set_alt_target(inertial_nav.get_altitude());
        plane.gcs_send_text(MAV_SEVERITY_INFO,"Land final started");
    }

    check_land_complete();
    return false;
}

// Write a control tuning packet
void QuadPlane::Log_Write_QControl_Tuning()
{
    const Vector3f &desired_velocity = pos_control->get_desired_velocity();
    struct log_QControl_Tuning pkt = {
        LOG_PACKET_HEADER_INIT(LOG_QTUN_MSG),
        time_us             : AP_HAL::micros64(),
        angle_boost         : attitude_control->angle_boost(),
        throttle_out        : motors->get_throttle(),
        desired_alt         : pos_control->get_alt_target() / 100.0f,
        inav_alt            : inertial_nav.get_altitude() / 100.0f,
        baro_alt            : (int32_t)plane.barometer.get_altitude() * 100,
        desired_climb_rate  : (int16_t)pos_control->get_vel_target_z(),
        climb_rate          : (int16_t)inertial_nav.get_velocity_z(),
        dvx                 : desired_velocity.x*0.01f,
        dvy                 : desired_velocity.y*0.01f,
    };
    plane.DataFlash.WriteBlock(&pkt, sizeof(pkt));
}
