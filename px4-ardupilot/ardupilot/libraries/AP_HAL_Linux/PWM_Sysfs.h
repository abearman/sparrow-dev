#pragma once

#include <AP_HAL/AP_HAL.h>

#include "AP_HAL_Linux.h"
#include "Util.h"

class Linux::PWM_Sysfs_Base {
public:
    virtual ~PWM_Sysfs_Base();

    enum Polarity {
        NORMAL = 0,
        INVERSE = 1,
    };

    void enable(bool value);
    bool is_enabled();
    void set_period(uint32_t nsec_period);
    uint32_t get_period();
    void set_freq(uint32_t freq);
    uint32_t get_freq();

    /*
     * This is the main method, to be called on hot path. It doesn't log any
     * failure so not to risk flooding the log. If logging is necessary, check
     * the return value.
     */
    bool set_duty_cycle(uint32_t nsec_duty_cycle);

    /*
     * This is supposed to be called in the hot path, so it returns the cached
     * value rather than getting it from hardware.
     */
    uint32_t get_duty_cycle();

    virtual void set_polarity(PWM_Sysfs_Base::Polarity polarity);
    virtual PWM_Sysfs_Base::Polarity get_polarity();

protected:
    PWM_Sysfs_Base(char *export_path, char *polarity_path,
                   char *enable_path, char *duty_path,
                   char *period_path, uint8_t channel);
private:
    uint32_t _nsec_duty_cycle_value = 0;
    int _duty_cycle_fd = -1;
    char *_export_path = NULL;
    char *_polarity_path = NULL;
    char *_enable_path = NULL;
    char *_duty_path = NULL;
    char *_period_path = NULL;
};

class Linux::PWM_Sysfs : public Linux::PWM_Sysfs_Base {
public:
    PWM_Sysfs(uint8_t chip, uint8_t channel);

private:
    char *_generate_export_path(uint8_t chip);
    char *_generate_polarity_path(uint8_t chip, uint8_t channel);
    char *_generate_enable_path(uint8_t chip, uint8_t channel);
    char *_generate_duty_path(uint8_t chip, uint8_t channel);
    char *_generate_period_path(uint8_t chip, uint8_t channel);
};

class Linux::PWM_Sysfs_Bebop : public Linux::PWM_Sysfs_Base {
public:
    PWM_Sysfs_Bebop(uint8_t channel);

private:
    char *_generate_export_path();
    char *_generate_polarity_path(uint8_t channel);
    char *_generate_enable_path(uint8_t channel);
    char *_generate_duty_path(uint8_t channel);
    char *_generate_period_path(uint8_t channel);

    void set_polarity(PWM_Sysfs_Base::Polarity polarity) override { }

    PWM_Sysfs_Base::Polarity get_polarity() override
    {
        return PWM_Sysfs::NORMAL;
    }
};
