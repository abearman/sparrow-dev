
static bool send_tango_coords(float xvel, float yvel, float zvel, float lat, float lng, float alt) {
		int32_t lat_int = lat * 10000000;	// lat * 10**7
		int32_t lng_int = lng * 10000000;	// lng * 10**7 
		int32_t alt_int = alt * 100;	 	// Converts meters to cm	
		Location tango_loc;
		tango_loc.lat = lat_int;
		tango_loc.lng = lng_int;
		tango_loc.alt = alt_int;
		tango.set_location(tango_loc);
		tango.set_velocity(xvel, yvel, zvel);
		tango.set_is_connected(true);
		return true;
}

static int32_t get_tango_coords() {
	return tango.get_location().lat;
}
