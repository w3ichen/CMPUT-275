#include "draw_route.h"
#include "map_drawing.h"


extern shared_vars shared;

void draw_route() {

	int32_t start_x,start_y, end_x,end_y;
	for (int i=0;i<shared.num_waypoints-1;i++){
		// access values for lon and lat and covert to x and y
		start_x = longitude_to_x(shared.map_number, shared.waypoints[i].lon);
		start_y = latitude_to_y(shared.map_number, shared.waypoints[i].lat);
		end_x = longitude_to_x(shared.map_number, shared.waypoints[i+1].lon);
		end_y = latitude_to_y(shared.map_number, shared.waypoints[i+1].lat);
		// draw the line from start point to end point
		

		shared.tft->drawLine(start_x,start_y,end_x,end_y,TFT_GREEN);


	}

// 	int i=0;
// shared.tft->setCursor(100,10);
// shared.tft->print(shared.num_waypoints);
// shared.tft->setCursor(30,0);
// shared.tft->print(shared.waypoints[i].lon);
// shared.tft->setCursor(30,50);
// shared.tft->print(shared.waypoints[i].lat);

// shared.tft->setCursor(30,100);
// shared.tft->print(shared.waypoints[i+1].lon);

// shared.tft->setCursor(30,150);
// shared.tft->print(shared.waypoints[i+1].lat);



}
