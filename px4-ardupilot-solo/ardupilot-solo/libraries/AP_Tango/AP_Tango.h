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
	const Vector3f &velocity() const;
	int32_t ground_course_cd() const;
	float get_lag() const; 
	bool horizontal_accuracy(float &hacc) const;
	bool speed_accuracy(float &sacc) const;
	uint32_t last_message_time_ms(void) const;

private:
	Location location;
	bool isTangoConnected;
	float ground_speed;
};

#endif  /* AP_TANGO */
