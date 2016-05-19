#include <AP_Tango.h>
#include <AP_Common.h>

AP_Tango::AP_Tango() {
	location.lat = 37.4275 * 10000000;     // Stanford latitude * 10**7
	location.lng = -122.1697 * 10000000;		// Stanford longitude * 10**&
	location.alt = 20 * 100;						// Stanford altitude in centimeters	
}
////////////// Velocity //////////////////////////
void AP_Tango::set_velocity(float xvel_param, float yvel_param, float zvel_param) {
	xvel = xvel_param;
	yvel = yvel_param;
	zvel = zvel_param;
}
////////////////////////////////////////////////

////////////// Location //////////////////////////
Location AP_Tango::get_location() const {
	return location;
}

void AP_Tango::set_location(Location loc_param) {
  location = loc_param;
}
/////////////////////////////////////////////////

////////////// Status //////////////////////////
bool AP_Tango::is_connected() const {
	return isTangoConnected;
}

void AP_Tango::set_is_connected(bool isConnected) {
	isTangoConnected = isConnected;
}
/////////////////////////////////////////////////

////////////// Velocity/ground speed //////////////////////////
Vector3f& AP_Tango::velocity {
	return last_velocity;
}

void AP_Tango::set_velocity(float xvel, float yvel, float zvel) {
	last_velocity[0] = xvel;
	last_velocity[1] = yvel;
	last_velocity[2] = zvel;
}

float AP_Tango::ground_speed() {
	return sqrt(last_velocity[0]*last_velocity[0] + last_velocity[1]*last_velocity[1] + last_velocity[2]*last_velocity[2]);
}
/////////////////////////////////////////////////

////////////// Accuracies //////////////////////////
bool AP_Tango::horizontal_accuracy(float &hacc) {
	*hacc = horizontal_accuracy;
	return true;
}

void AP_Tango::set_horizontal_accuracy(float ha) {
	horizontal_accuracy = ha;
}

bool AP_Tango::speed_accuracy(float &sacc) {
	*sacc = speed_accuracy;
	return true;
}

void AP_Tango::set_speed_accuracy(float sa) {
	speed_accuracy = sa;
}
/////////////////////////////////////////////////

////////////// Timestamp //////////////////////////
uint32_t AP_Tango::last_message_time_ms() {
	return last_tango_time_ms;
}

void AP_Tango:set_last_message_time_speed_ms(uint32_t timestamp) {
	last_tango_time_ms = timestamp;
}
/////////////////////////////////////////////////

////////////// Lag //////////////////////////
float AP_Tango::get_lag() const { 
	return 0.2f;
} 
/////////////////////////////////////////////////



