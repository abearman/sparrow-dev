#ifndef __AP_TANGO_H__
#define __AP_TANGO_H__

#include <AP_Common.h>

class AP_Tango {
public:
	AP_Tango();
	void set_location(Location loc);
	Location get_location() const;

private:
	Location location;
};

#endif  /* AP_TANGO */
