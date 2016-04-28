// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <AP_HAL/AP_HAL.h>

#if CONFIG_HAL_BOARD == HAL_BOARD_LINUX && \
    CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_BEBOP

#include "AP_BattMonitor_Bebop.h"
#include <AP_HAL_Linux/RCOutput_Bebop.h>

extern const AP_HAL::HAL& hal;

#define BATTERY_CAPACITY (1200U) /* mAh */
#define BATTERY_VOLTAGE_COMPENSATION_LANDED (0.2f)

/* polynomial compensation coefficients */
static const float bat_comp_polynomial_coeffs[5] = {
    -1.2471059149657287e-16f,
    3.2072883440944087e-12f,
    -3.3012241016211356e-08f,
    1.4612693130825659e-04f,
    -1.9236755589522961e-01f
};

/* battery percent lookup table */
#define BATTERY_PERCENT_LUT_SIZE 12
static const struct {
    float voltage;
    float percent;
} bat_lut[BATTERY_PERCENT_LUT_SIZE] = {
    {10.50f, 0.0f},
    {10.741699f, 2.6063901f},
    {10.835779f, 5.1693798f},
    {10.867705f, 7.7323696f},
    {10.900651f, 10.295359f},
    {11.008754f, 20.547318f},
    {11.148267f, 38.488246f},
    {11.322504f, 53.866185f},
    {11.505738f, 66.681133f},
    {11.746556f, 79.496082f},
    {12.110226f, 94.874021f},
    {12.3f, 100.0f }
};

void AP_BattMonitor_Bebop::init(void)
{
    set_capacity(BATTERY_CAPACITY);
    _battery_voltage_max = bat_lut[BATTERY_PERCENT_LUT_SIZE - 1].voltage;
    _prev_vbat_raw = bat_lut[BATTERY_PERCENT_LUT_SIZE - 1].voltage;
    _prev_vbat = bat_lut[BATTERY_PERCENT_LUT_SIZE - 1].voltage;
}

float AP_BattMonitor_Bebop::_filter_voltage(float vbat_raw)
{
    static const float a[2] = {
        1.0f, -9.9686333183343789e-01f
    };
    static const float b[2] = {
        1.5683340832810533e-03f, 1.5683340832810533e-03f
    };
    float vbat;
    static int only_once = 1;

    /* on first time reset filter with first raw value */
    if (only_once) {
        vbat = vbat_raw;
        _prev_vbat_raw = vbat_raw;
        _prev_vbat = vbat_raw;
        only_once = 0;
    } else  if (vbat_raw > 0.0f) {
        /*  1st order fitler */
        vbat = b[0] * vbat_raw +
            b[1] * _prev_vbat_raw - a[1] * _prev_vbat;
        _prev_vbat_raw = vbat_raw;
        _prev_vbat = vbat;
    } else {
        vbat = _prev_vbat;
    }

    return vbat;
}

float AP_BattMonitor_Bebop::_compute_compensation(const uint16_t *rpm,
                                                  float vbat_raw)
{
    float vbat, res;
    size_t i, j;

    vbat = vbat_raw;
    for (i = 0; i < BEBOP_BLDC_MOTORS_NUM; i++) {
        res = 0;
        for (j = 0; j < ARRAY_SIZE(bat_comp_polynomial_coeffs); j++)
            res = res * rpm[i] + bat_comp_polynomial_coeffs[j];

        vbat -= res;
    }

    return vbat;
}

float AP_BattMonitor_Bebop::_compute_battery_percentage(float vbat)
{
    float percent = 0.0f;
    int i;

    if (vbat <= bat_lut[0].voltage) {
        percent = 0.0f;
    } else if (vbat >= bat_lut[BATTERY_PERCENT_LUT_SIZE - 1].voltage) {
        percent = 100.0f;
    } else {
        i = 0;
        while (vbat >= bat_lut[i].voltage)
            i++;

        percent += bat_lut[i - 1].percent +
            (vbat - bat_lut[i - 1].voltage) *
            (bat_lut[i].percent - bat_lut[i - 1].percent) /
            (bat_lut[i].voltage - bat_lut[i - 1].voltage);
    }

    return percent;
}

void AP_BattMonitor_Bebop::read(void)
{
    int ret;
    uint32_t tnow;
    BebopBLDC_ObsData data;
    float remaining, vbat, vbat_raw;

    auto rcout = Linux::RCOutput_Bebop::from(hal.rcout);
    tnow = AP_HAL::micros();

    ret = rcout->read_obs_data(data);
    if (ret < 0) {
        _state.healthy = false;
        return;
    }

    /* get battery voltage observed by cypress */
    vbat_raw = (float)data.batt_mv / 1000.0f;

    /* do not compute battery status on ramping or braking transition */
    if (data.status == BEBOP_BLDC_STATUS_RAMPING ||
        data.status == BEBOP_BLDC_STATUS_STOPPING)
        return;

    /* if motors are spinning compute polynomial compensation */
    if (data.status == BEBOP_BLDC_STATUS_SPINNING_1 ||
        data.status == BEBOP_BLDC_STATUS_SPINNING_2) {
        vbat = _compute_compensation(data.rpm, vbat_raw);
    /* otherwise compute constant compensation */
    } else {
        vbat = vbat_raw - BATTERY_VOLTAGE_COMPENSATION_LANDED;
    }

    /* filter raw value */
    vbat = _filter_voltage(vbat);

    /* ensure battery voltage/percent will not grow up during use */
    if (vbat > _battery_voltage_max) {
        vbat = _battery_voltage_max;
    } else if (vbat < 0.0f) {
        vbat = 0.0f;
        _battery_voltage_max = 0.0f;
    } else {
        _battery_voltage_max = vbat;
    }

    /* compute remaining battery percent */
    remaining = _compute_battery_percentage(vbat);

    /* fillup battery state */
    _state.voltage = vbat;
    _state.last_time_micros = tnow;
    _state.healthy = true;
    _state.current_total_mah = BATTERY_CAPACITY -
                (remaining * BATTERY_CAPACITY) / 100.0f;
}

#endif
