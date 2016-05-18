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

private:
	Location location;
	bool isTangoConnected;
};

#endif  /* AP_TANGO */
