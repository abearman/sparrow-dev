#include <AP_Tango.h>
#include <AP_Common.h>

AP_Tango::AP_Tango() {
	location.lat = 37.4275 * 10000000;     // Stanford latitude * 10**7
	location.lng = -122.1697 * 10000000;		// Stanford longitude * 10**&
	location.alt = 20 * 100;						// Stanford altitude in centimeters	
}

void AP_Tango::set_location(Location loc_param) {
	location = loc_param;
}

Location AP_Tango::get_location() {
	return location;
}
