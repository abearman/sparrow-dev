#include <AP_Tango.h>
#include <AP_Common.h>

/* Constructor */
AP_Tango::AP_Tango() {
	location.lat = 37.4275 * 10000000;     // Stanford latitude * 10**7
	location.lng = -122.1697 * 10000000;		// Stanford longitude * 10**&
	location.alt = 20 * 100;						// Stanford altitude in centimeters	
}

////////////// Location //////////////////////////
Location AP_Tango::location() const {
	return _location;
}

void AP_Tango::set_location(Location loc_param) {
  _location = loc_param;
}
/////////////////////////////////////////////////

////////////// Status //////////////////////////
bool AP_Tango::is_connected() const {
	return _is_connected;
}

void AP_Tango::set_is_connected(bool isConnected) {
	_is_connected = isConnected;
}
/////////////////////////////////////////////////

////////////// Velocity/ground speed //////////////////////////
Vector3f& AP_Tango::velocity() {
	return _velocity;
}

void AP_Tango::set_velocity(float xvel, float yvel, float zvel) {
	_velocity[0] = xvel;
	_velocity[1] = yvel;
	_velocity[2] = zvel;
}

float AP_Tango::ground_speed() {
	return sqrt(_velocity[0]*_velocity[0] + _velocity[1]*_velocity[1] + _velocity[2]*_velocity[2]);
}
/////////////////////////////////////////////////

////////////// Accuracies //////////////////////////
bool AP_Tango::horizontal_accuracy(float &hacc) {
	*hacc = _horizonal_accuracy;
	return true;
}

void AP_Tango::set_horizontal_accuracy(float ha) {
	_horizontal_accuracy = ha;
}

bool AP_Tango::speed_accuracy(float &sacc) {
	*sacc = _speed_accuracy;
	return true;
}

void AP_Tango::set_speed_accuracy(float sa) {
	_speed_accuracy = sa;
}
/////////////////////////////////////////////////

////////////// Timestamp //////////////////////////
uint32_t AP_Tango::last_message_time_ms() {
	return _last_message_time_ms; 
}

void AP_Tango:set_last_message_time_ms(uint32_t timestamp) {
	_last_message_time_ms = timestamp;
}
/////////////////////////////////////////////////

////////////// Lag //////////////////////////
float AP_Tango::get_lag() const { 
	return 0.2f;
} 
/////////////////////////////////////////////////



