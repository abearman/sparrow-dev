#ifndef __AP_TANGO_H__
#define __AP_TANGO_H__

#include <AP_Common.h>
#include <AP_Math.h>

class AP_Tango {
public:
	AP_Tango();
	void set_location(Location loc);
	Location location() const;

	bool is_connected() const;
	void set_is_connected(bool isConnected);

	Vector3f velocity() const;
	void set_velocity(float xvel, float yvel, float zvel);
	float ground_speed() const;

	bool horizontal_accuracy(float &hacc) const;
	void set_horizontal_accuracy(float ha);
	bool speed_accuracy(float &sacc) const;
	void set_speed_accuracy(float sa);

	uint32_t last_message_time_ms(void) const;
	void set_last_message_time_ms(uint32_t timestamp);	

	float get_lag() const;
	
private:
	bool _is_connected;
	Location _location;									///< last fix location
	Vector3f _velocity;                 ///< 3D velocity in m/s, in NED format

	float _horizontal_accuracy;
  float _speed_accuracy;
	
	uint32_t _last_message_time_ms;          ///< the system time we got the last Tango timestamp, milliseconds
};

#endif  /* AP_TANGO */
