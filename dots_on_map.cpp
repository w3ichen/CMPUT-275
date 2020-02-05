#include "joy_cursor.cpp"

int32_t  x_to_lon(int16_t x) {
	return  map(x, 0, MAP_WIDTH , LON_WEST , LON_EAST);
}
int32_t  y_to_lat(int16_t y) {
	return  map(y, 0, MAP_HEIGHT , LAT_NORTH , LAT_SOUTH);
}
int16_t  lon_to_x(int32_t  lon) {
	return  map(lon , LON_WEST , LON_EAST , 0, MAP_WIDTH);
}
int16_t  lat_to_y(int32_t  lat) {
	return  map(lat , LAT_NORTH , LAT_SOUTH , 0, MAP_HEIGHT);
}

void restaurantDots(restaurants){
	TSPoint touch = ts.getPoint();
	if (touch.z > MINPRESSURE) {
		// if pressed
		if (restaurants.lat > )


	}
}