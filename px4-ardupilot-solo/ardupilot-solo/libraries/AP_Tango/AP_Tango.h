#ifndef __AP_TANGO_H__
#define __AP_TANGO_H__

#include <AP_Common.h>

class AP_Tango {
public:
	AP_Tango();
	void set_location(Location loc);
	Location get_location() const;

	bool is_connected() const;
	void set_is_connected(bool isConnected);

	float ground_speed() const;
	void set_ground_speed(float gs);

	const Vector3f &velocity() const;
	void set_velocity(float xvel, float yvel, float zvel);

	int32_t ground_course_cd() const;
	void set_ground_course_cd(int32_t gc);

	bool horizontal_accuracy(float &hacc) const;
	void set_horizontal_accuracy(float ha);
	bool speed_accuracy(float &sacc) const;
	void set_speed_accuracy(float sa);

	uint32_t last_message_time_ms(void) const;
	void set_last_message_time_ms(uint32_t timestamp);	

	float get_lag() const
	
private:
	Location location;
	float xvel = xvel;
	float yvel = yvel;
	float zvel = zvel;
	bool isTangoConnected;
	Location location;									///< last fix location
	float ground_speed;									 ///< ground speed in m/sec
	Vector3f velocity;                  ///< 3D velocitiy in m/s, in NED format
	int32_t ground_course_cd;           ///< ground course in 100ths of a degree

	float speed_accuracy;
  float horizontal_accuracy;
	
	uint32_t last_tango_time_ms;          ///< the system time we got the last Tango timestamp, milliseconds
};

#endif  /* AP_TANGO */
