
static bool send_tango_pos_vel(float lat, float lng, float alt, float xvel, float yvel, float zvel, uint32_t time_stamp) {
		int32_t lat_int = lat * 10000000;	// lat * 10**7
		int32_t lng_int = lng * 10000000;	// lng * 10**7 
		int32_t alt_int = alt * 100;	 	// Converts meters to cm	

		Location tango_loc;
		tango_loc.lat = lat_int;
		tango_loc.lng = lng_int;
		tango_loc.alt = alt_int;
		tango.set_location(tango_loc);

		tango.set_velocity(xvel, yvel, zvel);
		tango.set_last_message_time_speed_ms(time_stamp);
		tango.set_is_connected(true);
		return true;
}

static bool send_tango_gc_and_accuracy(int32_t ground_course, float accuracy) {
		tango.set_ground_course_cd(ground_course);
		tango.set_horizontal_accuracy(accuracy);
		// TODO: Set (and calculate?) Tango speed accuracy

		tango.set_is_connected(true);
		return true;
}

static int32_t get_tango_lat() {
	return tango.get_location().lat;
}
