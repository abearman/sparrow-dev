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

Location AP_Tango::get_location() const {
	return location;
}

bool AP_Tango::is_connected() const {
	return isTangoConnected;
}

void AP_Tango::set_is_connected(bool isConnected) {
	isTangoConnected = isConnected;
}

float AP_Tango::ground_speed(){

}

int32_t AP_Tango::ground_course_cd(){

}

float AP_Tango::get_lag() const { 
	return 0.2f;
} 



