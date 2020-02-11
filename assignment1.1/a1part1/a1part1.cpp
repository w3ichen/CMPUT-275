//====================================================================
//  Major Assignment #1 Part 1
//  Restaurant Finder
//  CMPUT 275 Winter 2020
//
//  Names: Gurbani Baweja, Weichen Qiu
//  ID: 1590254, 1578205
//=====================================================================


// Adding libraries
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <SPI.h>
#include <SD.h>
#include "lcd_image.h"
MCUFRIEND_kbv tft;

#define REST_START_BLOCK 4000000    // address of the first restaurant data
#define NUM_RESTAURANTS 1066        // total number of restaurants

// constants for converting lat and lon functions
#define  MAP_WIDTH  2048
#define  MAP_HEIGHT  2048
#define  LAT_NORTH  5361858l
#define  LAT_SOUTH  5340953l
#define  LON_WEST  -11368652l
#define  LON_EAST  -11333496l

// Define common variables
#define SD_CS 10
#define JOY_VERT  A9 // should connect A9 to pin VRx
#define JOY_HORIZ A8 // should connect A8 to pin VRy
#define JOY_SEL   53
#define DISPLAY_WIDTH  480
#define DISPLAY_HEIGHT 320
#define YEG_SIZE 2048
lcd_image_t yegImage = { "yeg-big.lcd", YEG_SIZE, YEG_SIZE };
#define JOY_CENTER   512
#define JOY_DEADZONE 64
#define CURSOR_SIZE 10

// Add Touchscreen functions
#include <Adafruit_GFX.h>
#include <TouchScreen.h>

// touch screen pins, obtained from the documentaion
#define YP A3 // must be an analog pin, use "An" notation!
#define XM A2 // must be an analog pin, use "An" notation!
#define YM 9  // can be a digital pin
#define XP 8  // can be a digital pin

// width/height of the display when rotated horizontally
#define TFT_WIDTH  480
#define TFT_HEIGHT 320

// calibration data for the touch screen, obtained from documentation
// the minimum/maximum possible readings from the touch point
#define TS_MINX 100
#define TS_MINY 110
#define TS_MAXX 960
#define TS_MAXY 910

// thresholds to determine if there was a touch
#define MINPRESSURE   10
#define MAXPRESSURE 1000

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300); // intialize touchscreen

// the cursor position on the display
int cursorX, cursorY;

// segment of the map displayed
int yegMapX = YEG_SIZE/2 - (DISPLAY_WIDTH - 60)/2;
int yegMapY = YEG_SIZE/2 - DISPLAY_HEIGHT/2;

Sd2Card card;


// forward declaration for redrawing the cursor
void redrawCursor(uint16_t colour);


// restaurant structure
struct restaurant{ 
  int32_t lat;
  int32_t lon;
  uint8_t rating;
  char name[55];
};


// structure for sorting distance
struct Restuarant_Dist{ 
  uint16_t index;
  uint16_t dist;
}rest_distance[NUM_RESTAURANTS];


//The block number is made global to store the recent memory block number.
restaurant restBlock[8];
uint32_t num_memoryblock=0; 


// conversion functions from lat/lon to map coord ad vice versa
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


void setup() {

  init();
  Serial.begin(9600);

  // declare pin modes
  pinMode(JOY_SEL, INPUT_PULLUP);
  pinMode(YP, OUTPUT); 
  pinMode(XM, OUTPUT); 

  // tft.reset();                // hardware reset
  uint16_t ID = tft.readID();    // read ID from display
  Serial.print("ID = 0x");
  Serial.println(ID, HEX);
  if (ID == 0xD3D3) ID = 0x9481; // write-only shield

  // must come before SD.begin() ...
  tft.begin(ID);                 // LCD gets ready to work

  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("failed! Is it inserted properly?");
    while (true) {}
  }
  Serial.println("OK!");

  Serial.print("Initializing SPI communication for raw reads...");
  if (!card.init(SPI_HALF_SPEED, SD_CS)) {
    Serial.println("failed! Is the card inserted properly?");
    while (true) {}
  }
  else {
    Serial.println("OK!");
  }

  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  // draws the centre of the Edmonton map, leaving the rightmost 60 columns black
  lcd_image_draw(&yegImage, &tft, yegMapX, yegMapY,
                 0, 0, DISPLAY_WIDTH - 60, DISPLAY_HEIGHT);

  // initial cursor position is the middle of the screen
  cursorX = (DISPLAY_WIDTH - 60)/2;
  cursorY = DISPLAY_HEIGHT/2;
 
  // draw the first cursor
  redrawCursor(TFT_RED);
}


/*
  Gets the restaurant's name, location, and rating from the SD card
  Arguments: index of restaurants and the restuarant structure as a pointer
  Returns: the restuarant structure with all info
*/
void getRestaurant(int restIndex, restaurant* rest_distance) { 
  
  pinMode(YP, INPUT); 
  pinMode(XM, INPUT); 
  uint32_t blockNum = REST_START_BLOCK + restIndex/8;

  // The memory block is read only if it hasn't been read previously.
  if (blockNum != num_memoryblock){
    while(!card.readBlock(blockNum, (uint8_t*) restBlock)){
    }
  }

  *rest_distance = restBlock[restIndex % 8];
  num_memoryblock = blockNum;
  pinMode(YP, OUTPUT); 
  pinMode(XM, OUTPUT); 
}

//=================================================================================================

/*
  This function moves the selection bar up and down the list
  Arguments: index of selected restaurant, boolean of direction
*/
void refresh_list(int16_t Rest_selected, bool up_or_down){
  
  pinMode(YP, OUTPUT); 
  pinMode(XM, OUTPUT); 
  tft.setCursor(0, 0);
  tft.setTextWrap(false);
  tft.setTextSize(2);

  if (up_or_down == true){
    
    // going up
    for (int i=0;i<Rest_selected;i++){ 
      tft.print("\n");
    }
    
    // selected is on top
    // call the restaurant name from sd card
    restaurant r;
    getRestaurant(rest_distance[Rest_selected].index, &r);
    
    // overwrite the old text
    tft.setTextColor(0x0000, 0xFFFF);
    tft.print(r.name);
    tft.print("\n"); 
    
    // deselect the one under it
    getRestaurant(rest_distance[Rest_selected+1].index, &r);
    tft.setTextColor(0xFFFF, 0x0000); 
    tft.print(r.name);
  }

  else{
  
    // going down
    for (int i=0;i<Rest_selected-1;i++){ 
      tft.print("\n");
    }

    restaurant r;
    getRestaurant(rest_distance[Rest_selected-1].index, &r);
    tft.setTextColor(0xFFFF, 0x0000);
    tft.print(r.name);
    tft.print("\n"); 
    
    // deselect the one under it
    getRestaurant(rest_distance[Rest_selected].index, &r);
    tft.setTextColor(0x0000, 0xFFFF); 
    tft.print(r.name);
  }

  delay(100);
}

//=================================================================================================

/*
  This function writes out all the restaurants in the list on button press
  by reading from the sorted rest_distance array
  Arguments: rest_distance array
*/
void disp_restaurants(Restuarant_Dist* rest_distance){

  pinMode(YP, OUTPUT); 
  pinMode(XM, OUTPUT); 

  tft.fillScreen(0);
  tft.setCursor(0, 0);
  tft.setTextWrap(false);
  tft.setTextSize(2);
  
  int16_t Rest_selected = 0;
  for(int16_t k = 0; k<21; k++){

    // read from sd card the first 21 restaurants
    restaurant r;
    getRestaurant(rest_distance[k].index, &r);
    pinMode(YP, OUTPUT); 
    pinMode(XM, OUTPUT); 
    
    if (k != Rest_selected){
      tft.setTextColor(0xFFFF, 0x0000);  
    }
    else{
      tft.setTextColor(0x0000, 0xFFFF);
    }
    
    tft.print(r.name);
    tft.print("\n");
  }

  tft.print("\n");
  int16_t select_bar =0;

  // keep looping until user selects a restaurant
  while(true){
    int yVal = analogRead(JOY_VERT);
    
    if(Rest_selected != 0 && yVal < JOY_CENTER - JOY_DEADZONE){
      
      // user selected up  
       tft.setTextColor(0xFFFF, 0x0000); 
       Rest_selected--;
       refresh_list(Rest_selected, true);
    }
    
    if(Rest_selected != 19 && yVal > JOY_CENTER + JOY_DEADZONE ){
      
      // user selected down
      tft.setTextColor(0x0000, 0xFFFF);
      Rest_selected++;
      refresh_list(Rest_selected,false);
    }

    if(digitalRead(JOY_SEL)==LOW){ 
      
      // when the user selects a restuarant
      int32_t x = 0, y = 0;
      restaurant selected;
      getRestaurant(rest_distance[Rest_selected].index, &selected);
      
      x = lon_to_x(selected.lon) - ((DISPLAY_WIDTH-60)/2);
      y = lat_to_y(selected.lat) - ((DISPLAY_HEIGHT)/2);
      
      // center cursor to middle
      cursorX = (DISPLAY_WIDTH-60)/2;
      cursorY = DISPLAY_HEIGHT/2;

      int rest_x = lon_to_x(selected.lon);
      int rest_y = lat_to_y(selected.lat);
      
      // handle edge conditions
      if (rest_x<((DISPLAY_WIDTH-60)/2)){
        
        // at left edge
        x = 0;
        cursorX = lon_to_x(selected.lon);
      }

      else if (rest_x> ( MAP_WIDTH - (DISPLAY_WIDTH-60/2))){
        
        // at right edge
        cursorX = rest_x - (MAP_WIDTH - (DISPLAY_WIDTH-60));
        x = MAP_WIDTH - (DISPLAY_WIDTH-60);
      }
      
      else if (rest_y<DISPLAY_HEIGHT/2){
        
        // at top edge
        y = 0;
        cursorY = lat_to_y(selected.lat);
      }

      else if (rest_y> (MAP_HEIGHT - (DISPLAY_HEIGHT/2))){
     
        // at bottom edge  
        cursorY = rest_y - (DISPLAY_HEIGHT - DISPLAY_HEIGHT);
        y = MAP_HEIGHT - DISPLAY_HEIGHT; 
      }

      else{
        x = rest_x - (DISPLAY_WIDTH-60)/2;
        y = rest_y - DISPLAY_HEIGHT / 2;
        cursorX = (DISPLAY_WIDTH-60)/2;
        cursorY = DISPLAY_HEIGHT / 2;
      }

      x = constrain(x,0, MAP_WIDTH - DISPLAY_WIDTH-60);
      y = constrain(y,0, MAP_HEIGHT-(DISPLAY_HEIGHT));

      yegMapX = x;
      yegMapY = y;
      pinMode(YP, OUTPUT); 
      pinMode(XM, OUTPUT); 
      
      tft.fillScreen(TFT_BLACK);
      lcd_image_draw(&yegImage, &tft, x , y ,
                   0, 0, DISPLAY_WIDTH - 60, DISPLAY_HEIGHT);
      redrawCursor(TFT_RED);
       break;
    }
  }
}

//=================================================================================================

/* 
  This function swaps restaurant in array. used for sorting
  Arguments: the two values, val1 and val2 to swap
*/
void swap(Restuarant_Dist &val1,Restuarant_Dist &val2){
  
  Restuarant_Dist emptyval;
  emptyval = val1;
  val1 = val2;
  val2 = emptyval ; 

}

//=================================================================================================

/* 
  This is insertion sort used to sort items in array by distance
  Arguments: the array restD, and length
*/
void isort(Restuarant_Dist* restD, uint16_t length){
  
  int16_t i = 1; 
  while(i<length){ 
    
    int16_t j = i;
    
    while((j>0) && (restD[j-1].dist > restD[j].dist)){
          swap(restD[j],restD[j-1]);
          j--;
    }
    
    i = i + 1;
  }

  disp_restaurants(restD);
}

//=================================================================================================

/*
  This is the main function that is called to display and sort the closest 21
  restaurants, makes calls to isort, getrestaurant
  Arguments: rest_distance array
*/
void nearest21_restaurants(Restuarant_Dist* rest_distance){

  restaurant rest; 
  int buttonVal = digitalRead(JOY_SEL);
 
  if(buttonVal == LOW){

    for(int16_t i = 0 ; i <NUM_RESTAURANTS ;i++){
      
      // reads in all the restaurants to rest_distance
      getRestaurant(i,&rest);
    
      rest_distance[i].index = i;
      rest_distance[i].dist = (abs((int32_t)lon_to_x(rest.lon) - 
      						  (int32_t)yegMapX - (int32_t)cursorX)) + 
      						  (abs((int32_t)lat_to_y(rest.lat) - 
      						  (int32_t)yegMapY - (int32_t)cursorY));

    }
    
    // sorts all the restaurants by distance
    isort(rest_distance,NUM_RESTAURANTS);  
  }
}

//=================================================================================================

/* 
  This function is used to scroll the map if cursor hits edge of map
*/
void scrollMap(){
  
  pinMode(YP, OUTPUT); 
  pinMode(XM, OUTPUT); 

  // constrain to inside the YEG map
  yegMapX = constrain(yegMapX,0,YEG_SIZE-DISPLAY_WIDTH-60);
  yegMapY = constrain(yegMapY,0,YEG_SIZE-DISPLAY_HEIGHT);
  
  // draw the map
  lcd_image_draw(&yegImage, &tft, yegMapX, yegMapY,
                 0, 0, DISPLAY_WIDTH - 60, DISPLAY_HEIGHT);
  delay(50);
  
  // reset cursor to middle
  cursorX = (DISPLAY_WIDTH - 60)/2;
  cursorY = DISPLAY_HEIGHT/2;
}

//=================================================================================================
 
/*
  THis function redraws the red cursor and constrains the cursor
  Arguemnts: the color of the cursor
*/
void redrawCursor(uint16_t colour) {
 
  // initial position of cursor before constrain
  int initialX = cursorX;
  int initialY = cursorY;

  // constrain cursor to edge of map
  cursorX = constrain(cursorX,CURSOR_SIZE/2, DISPLAY_WIDTH-60-(CURSOR_SIZE/2));
  cursorY = constrain(cursorY,CURSOR_SIZE/2,DISPLAY_HEIGHT-(CURSOR_SIZE/2));

  if (cursorX - initialX < 0){
    // too far to the right , scoll to the right
    if (yegMapX >= 0 && yegMapX < YEG_SIZE-DISPLAY_WIDTH-60){
      yegMapX += DISPLAY_WIDTH-60; 
      scrollMap();
    }
  }

  else if (cursorX - initialX > 0){
    // too far to the left, scroll to the left
    if (yegMapX > 0 && yegMapX <= YEG_SIZE-DISPLAY_WIDTH-60){ 
      yegMapX -= DISPLAY_WIDTH-60;
      scrollMap();
    }
  }
  
  if (cursorY - initialY < 0){
    // too far to the bottom , scoll down
    if (yegMapY >= 0 && yegMapY < YEG_SIZE-DISPLAY_HEIGHT){
      yegMapY += DISPLAY_HEIGHT; 
      scrollMap();
    }
  }
  
  else if (cursorY - initialY > 0){
    // too far to the top, scroll to the top
    if (yegMapY > 0 && yegMapY <= YEG_SIZE-DISPLAY_HEIGHT){
      yegMapY -= DISPLAY_HEIGHT; 
      scrollMap();
    }
  }
  
  pinMode(YP, OUTPUT); 
  pinMode(XM, OUTPUT);
  
  // draw the new cursor
  tft.fillRect(cursorX - CURSOR_SIZE/2, cursorY - CURSOR_SIZE/2,
               CURSOR_SIZE, CURSOR_SIZE, colour);

}

//=================================================================================================

/*
  This function covers the trail of the cursor by drawing a over trail with small section of the map
  Arguments: none
  Returns: void function
*/
void moveCursorBackground(){
  
  pinMode(YP, OUTPUT); 
  pinMode(XM, OUTPUT); 
  
  // draw small patch of map over cursor trail
  lcd_image_draw(&yegImage, &tft,yegMapX+(cursorX- CURSOR_SIZE/2),yegMapY+(cursorY- CURSOR_SIZE/2), 
                cursorX - CURSOR_SIZE/2, cursorY - CURSOR_SIZE/2,CURSOR_SIZE,CURSOR_SIZE);
}

//=================================================================================================

/*
  This function determines the speed of cursor based on how far joystick is pushed
  Arguments: x value of joystick
  Returns: speed in x direction
*/
int x_speed(int xVal){
    
    if (xVal < JOY_CENTER){
    // right half
    // distance joystick is pushed
    double distance = xVal - JOY_CENTER;
    // speed is the distance out of total which is a decimal out of 1
    // multiplieed by 15 the max speed to get the speed
    int speed = (distance/JOY_CENTER)*15;
    return speed;
  }

  else if (xVal > JOY_CENTER){
    // left half
    double distance = xVal - JOY_CENTER;
    int speed = (distance/JOY_CENTER)*15;
    return speed;
  }

}

//=================================================================================================

/*
  This function determines the speed of cursor based on how far joystick is pushed
  Arguments: y value of joystick
  Returns: speed in y direction
*/
int y_speed(int yVal){
  
  if (yVal < JOY_CENTER){
    // bottom half
    double distance = yVal - JOY_CENTER;
    int speed = (distance/JOY_CENTER)*15;
    return speed;
  }

  else if (yVal > JOY_CENTER){
    // top half
    double distance = yVal - JOY_CENTER;
    int speed = (distance/JOY_CENTER)*15;
    return speed;
  }

}

//=================================================================================================

/*
  This function moves the cursor according to joystick inputs
  Arguements: none
  Returns:none
*/
void processJoystick() {
 
  // Read in joystick 
  int xVal = analogRead(JOY_HORIZ);
  int yVal = analogRead(JOY_VERT);
  int buttonVal = digitalRead(JOY_SEL);
  // call speed functions get get speed
  int horizontal_speed = x_speed(xVal);
  int vertical_speed = y_speed(yVal);

  pinMode(YP, OUTPUT); 
  pinMode(XM, OUTPUT);

  if (yVal < JOY_CENTER - JOY_DEADZONE) {
    // call movecursorbackground to move up trail with map
    moveCursorBackground();
    // move cursor according to that speed
    cursorY += vertical_speed; 
    // call redrawcursor to redraw the cursor
    redrawCursor(TFT_RED);
  }

  else if (yVal > JOY_CENTER + JOY_DEADZONE) {
    moveCursorBackground();
    cursorY += vertical_speed;
    redrawCursor(TFT_RED);
  }

  if (xVal > JOY_CENTER + JOY_DEADZONE) {
    moveCursorBackground();
    cursorX -= horizontal_speed;
    redrawCursor(TFT_RED);
  }
  
  else if (xVal < JOY_CENTER - JOY_DEADZONE) {
    moveCursorBackground();
    cursorX -= horizontal_speed;
    redrawCursor(TFT_RED);
  }

  delay(20);
}

//=================================================================================================

// Adding restaurant dots
void restaurantDots(){
  
  pinMode(YP, INPUT); 
  pinMode(XM, INPUT); 

  TSPoint touch = ts.getPoint();
  if (touch.z > MINPRESSURE) {
    // if user touches the screen
    restaurant rest;

    for (int i=0; i<NUM_RESTAURANTS;i++){
      getRestaurant(i, &rest);
      
      if((lon_to_x(rest.lon)-yegMapX)>0 &&
        (lat_to_y(rest.lat)-yegMapY)>0  &&
        (lon_to_x(rest.lon)-yegMapX)<DISPLAY_WIDTH-60 &&
        (lat_to_y(rest.lat)-yegMapY)<DISPLAY_HEIGHT  ){
        
        // if restaurants is in the current map segment
        pinMode(YP, OUTPUT); 
        pinMode(XM, OUTPUT); 
        tft.fillCircle(
          lon_to_x(rest.lon)-yegMapX,
          lat_to_y(rest.lat)-yegMapY,
          CURSOR_SIZE/3,TFT_BLUE);
      }
    }
  }
}

//=================================================================================================

int main() {
	setup();

  while (true) {
    processJoystick();
    restaurantDots();
    nearest21_restaurants(rest_distance);

  }

	Serial.end();
	return 0;
}