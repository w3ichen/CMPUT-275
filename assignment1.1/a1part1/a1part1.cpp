// ---------------------------------------------------
//    Name: Kaiwen Tang, Chanpreet Singh
//    ID: 1575518, 1576137
//    CMPUT 275, Winter 2020
//
//    Major Assignment 1: Restaurant Finder Part 1
// ---------------------------------------------------

// initialize all pins and global constants

#define SD_CS 10
#define JOY_VERT  A9 // should connect A9 to pin VRx
#define JOY_HORIZ A8 // should connect A8 to pin VRy
#define JOY_SEL   53

// physical dimensions of the tft display (# of pixels)
#define DISPLAY_WIDTH  480
#define DISPLAY_HEIGHT 320

// touch screen pins, obtained from the documentaion
#define YP A3 // must be an analog pin, use "An" notation!
#define XM A2 // must be an analog pin, use "An" notation!
#define YM 9  // can be a digital pin
#define XP 8  // can be a digital pin

// dimensions of the part allocated to the map display
#define MAP_DISP_WIDTH (DISPLAY_WIDTH - 60)
#define MAP_DISP_HEIGHT DISPLAY_HEIGHT

#define REST_START_BLOCK 4000000
#define NUM_RESTAURANTS 1066

// calibration data for the touch screen, obtained from documentation
// the minimum/maximum possible readings from the touch point
#define TS_MINX 100
#define TS_MINY 120
#define TS_MAXX 940
#define TS_MAXY 920

// thresholds to determine if there was a touch
#define MINPRESSURE   10
#define MAXPRESSURE 1000

#define YEG_SIZE 2048
#define MAP_WIDTH 2048
#define MAP_HEIGHT 2048

#define LAT_NORTH 5361858l
#define LAT_SOUTH 5340953l
#define LON_WEST -11368652l
#define LON_EAST -11333496l

#define JOY_CENTER   512
#define JOY_DEADZONE 64

#define CURSOR_SIZE 9
#define TEXT_HEIGHT 16
#define TEXT_WIDTH 12

#include <Arduino.h>

// core graphics library (written by Adafruit)
#include <Adafruit_GFX.h>

// Hardware-specific graphics library for MCU Friend 3.5" TFT LCD shield
#include <MCUFRIEND_kbv.h>

// LCD and SD card will communicate using the Serial Peripheral Interface (SPI)
// e.g., SPI is used to display images stored on the SD card
#include <SPI.h>

// needed for reading/writing to SD card
#include <SD.h>

#include "lcd_image.h"
#include <TouchScreen.h>


MCUFRIEND_kbv tft;

lcd_image_t yegImage = { "yeg-big.lcd", YEG_SIZE, YEG_SIZE };

// the cursor position on the display
int16_t cursorX, cursorY;
int yegStartX, yegStartY, yegEndX, yegEndY;

// a multimeter reading says there are 300 ohms of resistance across the plate,
// so initialize with this to get more accurate readings
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// different than SD
Sd2Card card;

// forward declaration for redrawing the cursor
void redrawCursor(uint16_t colour);

int selected_index = 0;
//-------------------------------------------------------------------------------------------

// define the restaurant struct
struct restaurant {
  int32_t lat;
  int32_t lon;
  uint8_t rating; // from 0 to 10
  char name[55];
};

int block_start_index = 0;
int block_end_index = 7;

restaurant readBlock[8];

struct RestDist {uint16_t  index; // index  of  restaurant  from 0 to  NUM_RESTAURANTS -1
                 uint16_t  dist;   //  Manhatten  distance  to  cursor  position
};
RestDist  rest_dist[NUM_RESTAURANTS];

//---------------------------------------------------------------------------------------------

// We use enum to control the flow of program and know keep track of varoius modes
enum StateNames {
	Initial_Map, Print_List, Scrollable_List, Relocate_Map, Display_Restaurants
};
StateNames current = Initial_Map;

//---------------------------------------------------------------------------------------------


// Following funtions are used to maek conversions since these conversitions are needed again and again
int32_t x_to_lon(int16_t x){
	return map(x, 0, MAP_WIDTH, LON_WEST, LON_EAST);
}

int32_t y_to_lat(int16_t y){
	return map(y, 0, MAP_HEIGHT, LAT_NORTH, LAT_SOUTH);
}

int16_t lon_to_x(int32_t lon) {
	lon = constrain(lon, LON_WEST, LON_EAST);
	return map(lon, LON_WEST, LON_EAST, 0, MAP_WIDTH);
}

int16_t lat_to_y(int32_t lat) {
	lat = constrain(lat, LAT_SOUTH, LAT_NORTH);
	return map(lat, LAT_NORTH, LAT_SOUTH, 0, MAP_HEIGHT);
}

//--------------------------------------------------------------------------------------------

void setup() {
  init();

  Serial.begin(9600);

	pinMode(JOY_SEL, INPUT_PULLUP);

	//    tft.reset();             // hardware reset
  uint16_t ID = tft.readID();    // read ID from display
  Serial.print("ID = 0x");
  Serial.println(ID, HEX);
  if (ID == 0xD3D3) ID = 0x9481; // write-only shield
  
  // must come before SD.begin() ...
  tft.begin(ID);                 // LCD gets ready to work

	Serial.print("Init SD card...");
	if (!SD.begin(SD_CS)) {
		Serial.println("failed");
		while (true) {}
	}
	Serial.println("OK!");
	Serial.print("Init SPI com...");
  if (!card.init(SPI_HALF_SPEED, SD_CS)) {
    Serial.println("failed");
    while (true) {}
  }
  else {
    Serial.println("OK!");
  }

	tft.setRotation(1);

  tft.fillScreen(TFT_BLACK);

  // draws the centre of the Edmonton map, leaving the rightmost 60 columns black
	yegStartX = YEG_SIZE/2 - (DISPLAY_WIDTH - 60)/2;
	yegStartY = YEG_SIZE/2 - DISPLAY_HEIGHT/2;
	yegEndX = yegStartX + (DISPLAY_WIDTH - 60);
  	yegEndY = yegStartY + DISPLAY_HEIGHT;
	lcd_image_draw(&yegImage, &tft, yegStartX, yegStartY, 0, 0, DISPLAY_WIDTH - 60, DISPLAY_HEIGHT);

  // initial cursor position is the middle of the screen
  cursorX = (DISPLAY_WIDTH - 60)/2;
  cursorY = DISPLAY_HEIGHT/2;

  redrawCursor(TFT_RED);
}

//-------------------------------------------------------------------------------------

//This function is used to redraw the red block curser at the center of the map.
void redrawCursor(uint16_t colour) {
  tft.fillRect(cursorX - CURSOR_SIZE/2, cursorY - CURSOR_SIZE/2,
               CURSOR_SIZE, CURSOR_SIZE, colour);
}

//----------------------------------------------------------------------------------------

// This code was developed as part of a weekly assignment. 
void getRestaurantFast(int restIndex, restaurant* restPtr) {

  //determin whether we need to read from another block
  if (restIndex >= block_start_index && restIndex <= block_end_index) {
    *restPtr = readBlock[restIndex % 8];
  }
  // calculate the blockNum of the new block, and read the data
  // record its start point and end point
  else {
    uint32_t blockNum = REST_START_BLOCK + restIndex / 8;
    card.readBlock(blockNum, (uint8_t*) readBlock);
    *restPtr = readBlock[restIndex % 8];
    block_start_index = restIndex - (restIndex % 8);
    block_end_index = block_start_index + 7;
  } 
}

//---------------------------------------------------------------------------------------

// This function is used to change the displayed map. yegStartX and yegStary are global variables and they determine new coordinates of map portion being displayed. 
void redrawMap() {
	lcd_image_draw(&yegImage, &tft, yegStartX, yegStartY, 0, 0, DISPLAY_WIDTH - 60, DISPLAY_HEIGHT);
    cursorX = (DISPLAY_WIDTH - 60)/2;
    cursorY = DISPLAY_HEIGHT/2;
    redrawCursor(TFT_RED);
}

//-----------------------------------------------------------------------------------------

// Swap function is developed to awap values of a and b.
void swap(RestDist &a, RestDist &b) {
	RestDist temp;
	temp = a;
	a = b;
	b = temp;
}
//---------------------------------------------------------------------------------------------------------

//Insertion sort is used to sort the restaurants list by the distance from the pointers existing location
// Argument: rest_dist[] is an array containing manhatten distances
//           length is number of restaurants i.e. NUM_RESTAURANT
void isort(RestDist rest_dist[], int length) {
	int i = 1, j;
	while (i < length) {
        j = i;
        while ((j > 0) && (rest_dist[j-1].dist > rest_dist[j].dist)) {
        	swap(rest_dist[j], rest_dist[j-1]);
        	j--;
        }
        i++;
	}
}

//--------------------------------------------------------------------------------------------------------

// This function is used to find the manhatten distance of restaurants from the cursor and call isort() to sort restaurants as per distance.
void getRestaurantDistance() {
	restaurant rest;
	int16_t cursor_map_x, cursor_map_y, rest_map_x, rest_map_y;
	cursor_map_x = map(cursorX, 0, DISPLAY_WIDTH-1, yegStartX, yegEndX);
	cursor_map_y = map(cursorY, 0, DISPLAY_HEIGHT-1, yegStartY, yegEndY);
	for (int i = 0; i < NUM_RESTAURANTS; i++) {
        getRestaurantFast(i, &rest);
        rest_dist[i].index = i;
        rest_map_x = lon_to_x(rest.lon);
        rest_map_y = lat_to_y(rest.lat);
        //Manhattan distance
        rest_dist[i].dist = abs(cursor_map_x - rest_map_x) + abs(cursor_map_y - rest_map_y);
    }

    isort(rest_dist, NUM_RESTAURANTS);
}
//----------------------------------------------------------------------------------------

//This function is used to highlight the restaurants as joystich moves up or down
void updateSelectedItem(int new_index, int old_index) {
    restaurant rest;

    tft.setCursor(0, (TEXT_HEIGHT-1)*old_index);
    getRestaurantFast(rest_dist[old_index].index, &rest);
    tft.fillRect(0, (TEXT_HEIGHT-1)*old_index, TEXT_WIDTH*strlen(rest.name), TEXT_HEIGHT, TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.print(rest.name);

    
    tft.setCursor(0, (TEXT_HEIGHT-1)*new_index);
    getRestaurantFast(rest_dist[new_index].index, &rest);
    tft.fillRect(0, (TEXT_HEIGHT-1)*new_index, TEXT_WIDTH*strlen(rest.name), TEXT_HEIGHT, TFT_WHITE);
    tft.setTextColor(TFT_BLACK);
    tft.print(rest.name);    
}
//----------------------------------------------------------------------------------------

//This function is called after displaying initial list and here we process the scrolling of list as per assignemnt guidelines.
void processScrollList() {
	int new_index = 0;
    int yVal = analogRead(JOY_VERT);
    int buttonVal = digitalRead(JOY_SEL);
    if ((yVal < JOY_CENTER - JOY_DEADZONE) || (yVal > JOY_CENTER + JOY_DEADZONE)) {
        if ((yVal < JOY_CENTER - JOY_DEADZONE) && selected_index != 0) {
            new_index = selected_index - 1;
            updateSelectedItem(new_index, selected_index);
            selected_index = new_index;
            delay(100);
        }
        if ((yVal > JOY_CENTER + JOY_DEADZONE) && selected_index < 21) {
            new_index = selected_index + 1;
            if(new_index == 21) {
            	new_index = 20;
            }
            updateSelectedItem(new_index, selected_index);
            selected_index = new_index;
            delay(100);
        }
    }
    // We change mode and go to map
    if (buttonVal == 0) {
        current = Relocate_Map;
    }
}
//----------------------------------------------------------------------------------------

// When we go to mode 1 this function is used to create new interface with all the restaurants.
void printRestList() {
	tft.fillScreen(TFT_BLACK);
    tft.setRotation(1);
    tft.setTextColor(TFT_WHITE);
	getRestaurantDistance();
	int cursor_start_y = 0;
	tft.setTextSize(2);
	restaurant rest;
	for (int i = 0; i < 21; i++) {
        getRestaurantFast(rest_dist[i].index, &rest);
        tft.setCursor(0, cursor_start_y);
        tft.print(rest.name);
        cursor_start_y += 15;
	}
	updateSelectedItem(0,0);
	current = Scrollable_List;
}
//----------------------------------------------------------------------------------------

// This function is used to process movement of cursor on map and also changes map each time curser hit the boundary.
// This funciton also display all nearby restaurant on map when used touch anywhere on screen except black part.
void processJoystick() {
  int xVal = analogRead(JOY_HORIZ);
  int yVal = analogRead(JOY_VERT);
  int buttonVal = digitalRead(JOY_SEL);
  
  TSPoint touch = ts.getPoint();

  pinMode(YP, OUTPUT); 
  pinMode(XM, OUTPUT);
  // Determin if the Joystick is moved, and only redraw the red cursor when the Joystick is moved
  // This solved the flickering problem
  if ((xVal < JOY_CENTER - JOY_DEADZONE) || (xVal > JOY_CENTER + JOY_DEADZONE) || (yVal < JOY_CENTER - JOY_DEADZONE) || (yVal > JOY_CENTER + JOY_DEADZONE)) {

  	// When cursor is moved, redraw the part of the edmonton map that was covered by the cursor at the previous position
    lcd_image_draw(&yegImage, &tft, cursorX + yegStartX - CURSOR_SIZE/2, cursorY + yegStartY - CURSOR_SIZE/2 , cursorX - CURSOR_SIZE/2, cursorY - CURSOR_SIZE/2, CURSOR_SIZE, CURSOR_SIZE);

    // now move the cursor
    // only move cursor when it is not at the edge of the map and when the joystick is moved
    if ((yVal < JOY_CENTER - JOY_DEADZONE) && (cursorY >= CURSOR_SIZE / 2 + 1)) {
        cursorY -= 1; // decrease the y coordinate of the cursor
    }
    else if ((yVal > JOY_CENTER + JOY_DEADZONE) && (cursorY <= DISPLAY_HEIGHT - CURSOR_SIZE / 2 - 2)) {
        cursorY += 1;
    }

    // remember the x-reading increases as we push left
    if ((xVal > JOY_CENTER + JOY_DEADZONE) && (cursorX >= CURSOR_SIZE / 2 + 1)) {
        cursorX -= 1;
    }
    else if ((xVal < JOY_CENTER - JOY_DEADZONE) && (cursorX <= DISPLAY_WIDTH - 60 - CURSOR_SIZE / 2 - 2)) {
        cursorX += 1;
    }

    // draw a small patch of the Edmonton map at the old cursor position before
    // drawing a red rectangle at the new cursor position

    redrawCursor(TFT_RED);
    }

    if (buttonVal == 0) {
    	current = Print_List;
    }

    int16_t screen_x = map(touch.y, TS_MINX, TS_MAXX,  DISPLAY_WIDTH-1, 0);

    //If user touch anywhere on screen other then black part then nearby restaurants are displayed on map
    if (touch.z > MINPRESSURE && touch.z < MAXPRESSURE && screen_x<DISPLAY_WIDTH-60) {
    	current = Display_Restaurants;
    }

    //Following if-else statements are used to change the map coordinates displayed on the map as cursor hit the boundary
    if(cursorX == 4){
  		if ((yegStartX < (DISPLAY_WIDTH - 60)) && (yegStartX != 0)) {
  			yegStartX = 0;
  			redrawMap();
  		}
  		else if (yegStartX > (DISPLAY_WIDTH - 60)){
            yegStartX -= (DISPLAY_WIDTH - 60);
            redrawMap();
  		}

  	}
  	else if(cursorY == 4){
  		if ((yegStartY < DISPLAY_HEIGHT) && (yegStartY != 0)) {
  			yegStartY = 0;
            redrawMap();
        }
        else if (yegStartY > DISPLAY_HEIGHT){
        	yegStartY -= DISPLAY_HEIGHT;
        	redrawMap();
        }
  	}
  	else if(cursorY == 315){
  		if ((yegEndY > MAP_HEIGHT- DISPLAY_HEIGHT) && ((yegStartY + DISPLAY_HEIGHT+1) != MAP_HEIGHT)) {
  			yegStartY = MAP_HEIGHT - DISPLAY_HEIGHT - 1;
  	        redrawMap();
  		}
  		else if (yegEndY <= MAP_HEIGHT - DISPLAY_HEIGHT){
  			yegStartY += DISPLAY_HEIGHT;
  			redrawMap();
  		}
  	}
  	else if(cursorX == 415){
  		if ((yegEndX > MAP_WIDTH - (DISPLAY_WIDTH - 60)) && ((yegStartX + (DISPLAY_WIDTH-60)) != MAP_WIDTH)) {
  			yegStartX = MAP_WIDTH - (DISPLAY_WIDTH - 60);
  		    redrawMap();
  		}
  		else if (yegEndX <= MAP_WIDTH - (DISPLAY_WIDTH - 60)){
  			yegStartX += (DISPLAY_WIDTH-60);
  			redrawMap();
  		}
  	}
  	yegEndX = yegStartX + (DISPLAY_WIDTH - 60);
  	yegEndY = yegStartY + DISPLAY_HEIGHT;
}

//---------------------------------------------------------------------------------------------------------

//This function is used when user select a restaurant from the scrolable list, so we find coordinates of the restaurant and adjust map accordingly. We go back to mode 0.
void renewMap (int index) {
	restaurant rest;
	int16_t rest_map_x, rest_map_y;
	getRestaurantFast(rest_dist[index].index, &rest);
	rest_map_x = lon_to_x(rest.lon);
    rest_map_y = lat_to_y(rest.lat);
    if ((rest_map_x + (DISPLAY_WIDTH-60)/2) > MAP_WIDTH) {
    	yegStartX = MAP_WIDTH - (DISPLAY_WIDTH-60);
    	cursorX = rest_map_x - yegStartX;
    }
    else if ((rest_map_y + DISPLAY_HEIGHT / 2) > MAP_HEIGHT) {
    	yegStartY = MAP_HEIGHT - DISPLAY_HEIGHT;
    	cursorY = rest_map_y - yegStartY;
    }
    else if (rest_map_x < (DISPLAY_WIDTH-60)/2) {
    	yegStartX = 0;
    	cursorX = rest_map_x;
    } 
    else if (rest_map_y < DISPLAY_HEIGHT/2) {
    	yegStartY = 0;
    	cursorY = rest_map_y;
    }
    else{
        yegStartX = rest_map_x - (DISPLAY_WIDTH-60)/2;
        yegStartY = rest_map_y - DISPLAY_HEIGHT / 2;
        cursorX = (DISPLAY_WIDTH-60)/2;
        cursorY = DISPLAY_HEIGHT / 2;
    }
    tft.fillScreen(TFT_BLACK);
    lcd_image_draw(&yegImage, &tft, yegStartX, yegStartY, 0, 0, DISPLAY_WIDTH - 60, DISPLAY_HEIGHT);
    redrawCursor(TFT_RED);
    selected_index = 0;
    current = Initial_Map; 
}

//---------------------------------------------------------------------------------------------------------

//This function is used to display resttaurants when user touch anywhere on the screen. We display all nearby restaurants that fit in current displayed map.
void displayRestaurant() {
	int16_t rest_map_x, rest_map_y;
	restaurant rest;
	for (int i = 0; i < NUM_RESTAURANTS; i++) {
        getRestaurantFast(rest_dist[i].index, &rest);
        rest_map_x = lon_to_x(rest.lon);
        rest_map_y = lat_to_y(rest.lat);
        if (rest_map_x > yegStartX && rest_map_x < yegEndX && rest_map_y > yegStartY && rest_map_y < yegEndY ) {
        	tft.fillCircle(rest_map_x - yegStartX, rest_map_y - yegStartY, 4, TFT_BLUE);
        	Serial.println(1);
        }
	}
	current = Initial_Map;
}
//---------------------------------------------------------------------------------------------------------

int main() {

	setup();
	
	card.readBlock(REST_START_BLOCK, (uint8_t*) readBlock);
	getRestaurantDistance();

	while (true) {

		if (current == Initial_Map) {
            processJoystick();
		}
		else if (current == Print_List) {
		    printRestList();
		}
		else if (current == Scrollable_List) {
			processScrollList();
		}
		else if (current == Relocate_Map) {
			renewMap(selected_index);
		}
		else if (current == Display_Restaurants) {
			displayRestaurant();
		}
	}
}