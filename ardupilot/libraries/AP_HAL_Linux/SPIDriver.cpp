#include <AP_Common/AP_Common.h>
#include <AP_HAL/AP_HAL.h>

#if CONFIG_HAL_BOARD == HAL_BOARD_LINUX
#include "SPIDriver.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include "GPIO.h"

using namespace Linux;

extern const AP_HAL::HAL& hal;

#define MHZ (1000U*1000U)
#define KHZ (1000U)

#if CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_PXF || CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_ERLEBOARD
SPIDeviceDriver SPIDeviceManager::_device[] = {
    // different SPI tables per board subtype
    SPIDeviceDriver("lsm9ds0_am", 1, 0, AP_HAL::SPIDevice_LSM9DS0_AM, SPI_MODE_3, 8, BBB_P9_17,  10*MHZ,10*MHZ),
    SPIDeviceDriver("lsm9ds0_g",  1, 0, AP_HAL::SPIDevice_LSM9DS0_G,  SPI_MODE_3, 8, BBB_P8_9,   10*MHZ,10*MHZ),
    SPIDeviceDriver("ms5611",     2, 0, AP_HAL::SPIDevice_MS5611,     SPI_MODE_3, 8, BBB_P9_42,  10*MHZ,10*MHZ),
    SPIDeviceDriver("mpu6000",    2, 0, AP_HAL::SPIDevice_MPU6000,    SPI_MODE_3, 8, BBB_P9_28,  500*1000, 20*MHZ),
    /* MPU9250 is restricted to 1MHz for non-data and interrupt registers */
    SPIDeviceDriver("mpu9250",    2, 0, AP_HAL::SPIDevice_MPU9250,    SPI_MODE_3, 8, BBB_P9_23,  1*MHZ, 20*MHZ),
    SPIDeviceDriver("dataflash",  2, 0, AP_HAL::SPIDevice_Dataflash,  SPI_MODE_3, 8, BBB_P8_12,  6*MHZ, 6*MHZ),
};
#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_MINLURE
SPIDeviceDriver SPIDeviceManager::_device[] = {
    SPIDeviceDriver("mpu6000",    0, 0, AP_HAL::SPIDevice_MPU6000,    SPI_MODE_3, 8, SPI_CS_KERNEL, 1*MHZ, 15*MHZ)
};
#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_NAVIO || CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_NAVIO2
SPIDeviceDriver SPIDeviceManager::_device[] = {
    /* MPU9250 is restricted to 1MHz for non-data and interrupt registers */
    SPIDeviceDriver("mpu9250",    0, 1, AP_HAL::SPIDevice_MPU9250, SPI_MODE_0, 8, SPI_CS_KERNEL,  1*MHZ, 20*MHZ),
    SPIDeviceDriver("ublox",      0, 0, AP_HAL::SPIDevice_Ublox, SPI_MODE_0, 8, SPI_CS_KERNEL,  5*MHZ, 5*MHZ),
};
#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_ERLEBRAIN2 || \
      CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_PXFMINI
SPIDeviceDriver SPIDeviceManager::_device[] = {
    /* MPU9250 is restricted to 1MHz for non-data and interrupt registers */
    SPIDeviceDriver("mpu9250",    0, 1, AP_HAL::SPIDevice_MPU9250, SPI_MODE_0, 8, SPI_CS_KERNEL,  1*MHZ, 20*MHZ),
    SPIDeviceDriver("ms5611",     0, 0, AP_HAL::SPIDevice_MS5611,  SPI_MODE_0, 8, SPI_CS_KERNEL,  1*KHZ, 10*MHZ),
};
#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_BBBMINI
SPIDeviceDriver SPIDeviceManager::_device[] = {
    /* MPU9250 is restricted to 1MHz for non-data and interrupt registers */
    SPIDeviceDriver("mpu9250",    2, 0, AP_HAL::SPIDevice_MPU9250,    SPI_MODE_3, 8, SPI_CS_KERNEL,  1*MHZ, 20*MHZ),
    SPIDeviceDriver("ms5611",    2, 1, AP_HAL::SPIDevice_MS5611,     SPI_MODE_3, 8, SPI_CS_KERNEL,  10*MHZ,10*MHZ),
};
#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_RASPILOT
SPIDeviceDriver SPIDeviceManager::_device[] = {
    /* MPU9250 is restricted to 1MHz for non-data and interrupt registers */
    SPIDeviceDriver("mpu6000",    0, 0, AP_HAL::SPIDevice_MPU6000,     SPI_MODE_3, 8, RPI_GPIO_25,  1*MHZ, 20*MHZ),
    SPIDeviceDriver("ms5611",     0, 0, AP_HAL::SPIDevice_MS5611,      SPI_MODE_3, 8, RPI_GPIO_23,  10*MHZ, 10*MHZ),
    SPIDeviceDriver("lsm9ds0_am", 0, 0, AP_HAL::SPIDevice_LSM9DS0_AM,  SPI_MODE_3, 8, RPI_GPIO_22,  10*MHZ, 10*MHZ),
    SPIDeviceDriver("lsm9ds0_g",  0, 0, AP_HAL::SPIDevice_LSM9DS0_G,   SPI_MODE_3, 8, RPI_GPIO_12,  10*MHZ, 10*MHZ),
    SPIDeviceDriver("dataflash",  0, 0, AP_HAL::SPIDevice_Dataflash,   SPI_MODE_3, 8, RPI_GPIO_5,   6*MHZ, 6*MHZ),
    SPIDeviceDriver("raspio",     0, 0, AP_HAL::SPIDevice_RASPIO,      SPI_MODE_3, 8, RPI_GPIO_7,   10*MHZ, 10*MHZ),
};
#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_BH
SPIDeviceDriver SPIDeviceManager::_device[] = {
    SPIDeviceDriver("mpu9250", 0, 0, AP_HAL::SPIDevice_MPU9250, SPI_MODE_0, 8, RPI_GPIO_7, 1*MHZ, 20*MHZ),
    SPIDeviceDriver("ublox", 0, 0, AP_HAL::SPIDevice_Ublox, SPI_MODE_0, 8, RPI_GPIO_8, 250*KHZ, 5*MHZ),
};
#else
// empty device table
SPIDeviceDriver SPIDeviceManager::_device[] = { };
#define LINUX_SPI_DEVICE_NUM_DEVICES 0
#endif

#ifndef LINUX_SPI_DEVICE_NUM_DEVICES
#define LINUX_SPI_DEVICE_NUM_DEVICES ARRAY_SIZE(SPIDeviceManager::_device)
#endif

const uint8_t SPIDeviceManager::_n_device_desc = LINUX_SPI_DEVICE_NUM_DEVICES;

SPIDeviceDriver::SPIDeviceDriver(const char *name, uint16_t bus, uint16_t subdev, enum AP_HAL::SPIDeviceType type, uint8_t mode, uint8_t bitsPerWord, int16_t cs_pin, uint32_t lowspeed, uint32_t highspeed):
    _name(name),
    _bus(bus),
    _subdev(subdev),
    _type(type),
    _mode(mode),
    _bitsPerWord(bitsPerWord),
    _lowspeed(lowspeed),
    _highspeed(highspeed),
    _speed(highspeed),
    _cs_pin(cs_pin),
    _cs(NULL)
{
}

void SPIDeviceDriver::init()
{
    // Init the CS
    if(_cs_pin != SPI_CS_KERNEL) {
        _cs = hal.gpio->channel(_cs_pin);
        if (_cs == NULL) {
            AP_HAL::panic("Unable to instantiate cs pin");
        }
        _cs->mode(HAL_GPIO_OUTPUT);
        _cs->write(1);       // do not hold the SPI bus initially
    } else {
        // FIXME Anything we need to do here for kernel-managed CS?
    }
}

AP_HAL::Semaphore *SPIDeviceDriver::get_semaphore()
{
    return _fake_dev->get_semaphore();
}

bool SPIDeviceDriver::transaction(const uint8_t *tx, uint8_t *rx, uint16_t len)
{
    return SPIDeviceManager::transaction(*this, tx, rx, len);
}

void SPIDeviceDriver::set_bus_speed(enum bus_speed speed)
{
    if (speed == SPI_SPEED_LOW) {
        _speed = _lowspeed;
    } else {
        _speed = _highspeed;
    }
}

void SPIDeviceDriver::cs_assert()
{
    SPIDeviceManager::cs_assert(_type);
}

void SPIDeviceDriver::cs_release()
{
    SPIDeviceManager::cs_release(_type);
}

uint8_t SPIDeviceDriver::transfer(uint8_t data)
{
    uint8_t v = 0;
    transaction(&data, &v, 1);
    return v;
}

void SPIDeviceDriver::transfer(const uint8_t *data, uint16_t len)
{
    transaction(data, NULL, len);
}

void SPIDeviceManager::init()
{
    for (uint8_t i=0; i<LINUX_SPI_DEVICE_NUM_DEVICES; i++) {
        _device[i]._fake_dev = SPIDeviceManager::from(hal.spi)->get_device(_device[i]);
        if (!_device[i]._fake_dev) {
            AP_HAL::panic("SPIDriver: couldn't use spidev%u.%u for %s",
                          _device[i]._bus, _device[i]._subdev, _device[i]._name);
        }
        _device[i].init();
    }
}

void SPIDeviceManager::cs_assert(enum AP_HAL::SPIDeviceType type)
{
    uint16_t bus = 0, i;
    for (i=0; i<LINUX_SPI_DEVICE_NUM_DEVICES; i++) {
        if (_device[i]._type == type) {
            bus = _device[i]._bus;
            break;
        }
    }
    if (i == LINUX_SPI_DEVICE_NUM_DEVICES) {
        AP_HAL::panic("Bad device type");
    }

    // Kernel-mode CS handling
    if(_device[i]._cs_pin == SPI_CS_KERNEL)
        return;

    for (i=0; i<LINUX_SPI_DEVICE_NUM_DEVICES; i++) {
        if (_device[i]._bus != bus) {
            // not the same bus
            continue;
        }
        if (_device[i]._type != type) {
            if (_device[i]._cs->read() != 1) {
                hal.console->printf("two CS enabled at once i=%u %u and %u\n",
                                    (unsigned)i, (unsigned)type, (unsigned)_device[i]._type);
            }
        }
    }
    for (i=0; i<LINUX_SPI_DEVICE_NUM_DEVICES; i++) {
        if (_device[i]._type == type) {
            _device[i]._cs->write(0);
        }
    }
}

void SPIDeviceManager::cs_release(enum AP_HAL::SPIDeviceType type)
{
    uint16_t bus = 0, i;
    for (i=0; i<LINUX_SPI_DEVICE_NUM_DEVICES; i++) {
        if (_device[i]._type == type) {
            bus = _device[i]._bus;
            break;
        }
    }
    if (i == LINUX_SPI_DEVICE_NUM_DEVICES) {
        AP_HAL::panic("Bad device type");
    }

    // Kernel-mode CS handling
    if(_device[i]._cs_pin == SPI_CS_KERNEL)
        return;

    for (i=0; i<LINUX_SPI_DEVICE_NUM_DEVICES; i++) {
        if (_device[i]._bus != bus) {
            // not the same bus
            continue;
        }
        _device[i]._cs->write(1);
    }
}

bool SPIDeviceManager::transaction(SPIDeviceDriver &driver, const uint8_t *tx, uint8_t *rx, uint16_t len)
{
    int r;
    // we set the mode before we assert the CS line so that the bus is
    // in the correct idle state before the chip is selected
    int fd = driver._fake_dev->get_fd();
    r = ioctl(fd, SPI_IOC_WR_MODE, &driver._mode);
    if (r == -1) {
        hal.console->printf("SPI: error on setting mode\n");
        return false;
    }

    cs_assert(driver._type);
    struct spi_ioc_transfer spi[1];
    memset(spi, 0, sizeof(spi));
    spi[0].tx_buf        = (uint64_t)tx;
    spi[0].rx_buf        = (uint64_t)rx;
    spi[0].len           = len;
    spi[0].delay_usecs   = 0;
    spi[0].speed_hz      = driver._speed;
    spi[0].bits_per_word = driver._bitsPerWord;
    spi[0].cs_change     = 0;

    if (rx != NULL) {
        // keep valgrind happy
        memset(rx, 0, len);
    }

    r = ioctl(fd, SPI_IOC_MESSAGE(1), &spi);
    cs_release(driver._type);

    if (r == -1) {
        hal.console->printf("SPI: error on doing transaction\n");
        return false;
    }

    return true;
}

/*
  return a SPIDeviceDriver for a particular device
 */
AP_HAL::SPIDeviceDriver *SPIDeviceManager::device(enum AP_HAL::SPIDeviceType dev, uint8_t index)
{
    uint8_t count = 0;
    for (uint8_t i=0; i<LINUX_SPI_DEVICE_NUM_DEVICES; i++) {
        if (_device[i]._type == dev) {
            if (count == index) {
                return &_device[i];
            } else {
                count++;
            }
        }
    }
    return NULL;
}

#endif // CONFIG_HAL_BOARD
