#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <SD.h>
#include <TouchScreen.h>

#define SD_CS 10

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

MCUFRIEND_kbv tft;

// a multimeter reading says there are 300 ohms of resistance across the plate,
// so initialize with this to get more accurate readings
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// different than SD
Sd2Card card;

// initializing variables to record time elapsed
uint32_t slow_recent, fast_recent, startTime, endTime;
uint32_t slow_sum=0, fast_sum=0, slow_count=0, fast_count=0;

struct restaurant {
  int32_t lat;
  int32_t lon;
  uint8_t rating; // from 0 to 10
  char name[55];
};

// initializing structure to store blocks for fast function
restaurant storedBlock[8];
restaurant rest;
uint32_t current_block_start = NULL;

void setup() {
  init();
  Serial.begin(9600);
  // tft display initialization
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(1);

  // SD card initialization for raw reads
  Serial.print("Initializing SPI communication for raw reads...");
  if (!card.init(SPI_HALF_SPEED, SD_CS)) {
    Serial.println("failed! Is the card inserted properly?");
    while (true) {}
  }
  else {
    Serial.println("OK!");
  }
}

void getRestaurant(int restIndex, restaurant* restPtr) {
/* 
  slow implementation of reading restaurants from card
  Arguments: index of restaurant, and structure to store restaurant
  Returns: void
*/
  uint32_t blockNum = REST_START_BLOCK + restIndex/8;
  restaurant restBlock[8];

  // reads from card
  while (!card.readBlock(blockNum, (uint8_t*) restBlock)) {
    Serial.println("Read block failed, trying again.");
  }
  *restPtr = restBlock[restIndex % 8];
}

void getRestaurantFast(int restIndex, restaurant* restPtr) {
/* 
  Fast implementation of reading restaurants from card
  Arguments: index of restaurants and structure to store restaurants
  Returns: void function
*/
  // the memory address of the block, where rest start block si the first block
  uint32_t blockNum = REST_START_BLOCK + restIndex/8;

  // if the index is not in  the current stored block
  if (restIndex > current_block_start + 8){
    current_block_start = restIndex;

    while (!card.readBlock(blockNum, (uint8_t*) storedBlock)) {
      Serial.println("Read block failed, trying again.");
    }
  }
  *restPtr = storedBlock[restIndex % 8];
}

void tft_start_display(){
/*
  Draws the initial display. Draws the buttons and the initial text
  Arguments: none
  returns: void
*/
  // change pinmode to output to write to display
  pinMode(YP, OUTPUT);
  pinMode(XM, OUTPUT);
  uint32_t line_spacing = 25, space=15;
  uint32_t slow_average = slow_sum/slow_count;
  uint32_t fast_average = fast_sum/fast_count;
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  // write the text to display
  tft.setCursor(0,0); tft.print("RECENT SLOW RUN:");
  tft.setCursor(0,line_spacing); tft.print("Not yet run");
  tft.setCursor(0,2*line_spacing+space); tft.print("SLOW RUN AVG:");
  tft.setCursor(0,3*line_spacing+space);tft.print("Not yet run");
  tft.setCursor(0,4*line_spacing+2*space); tft.print("RECENT FAST RUN");
  tft.setCursor(0,5*line_spacing+2*space);tft.print("Not yet run");
  tft.setCursor(0,6*line_spacing+3*space);tft.print("FAST RUN AVG:");
  tft.setCursor(0,7*line_spacing+3*space);tft.print("Not yet run");

  // buttons
  tft.drawRect(DISPLAY_WIDTH-60,0,60,320/2,TFT_RED);
  tft.drawRect(DISPLAY_WIDTH-60,320/2,60,320/2,TFT_RED);

  // button text for slow button
  uint32_t y_position = DISPLAY_HEIGHT/8;
  const char* slow_array[] = {"S","L","O","W"};
  for (int slow_text=0; slow_text<4;slow_text++){
    tft.setCursor(DISPLAY_WIDTH-32,y_position);
    y_position += 20;
    tft.print(slow_array[slow_text]);
  }
  // button text for fast button
  y_position = 160 + DISPLAY_HEIGHT/8;
  const char* fast_array[] = {"F","A","S","T"};
  for (int fast_text=0; fast_text<4;fast_text++){
    tft.setCursor(DISPLAY_WIDTH-32,y_position);
    y_position += 20;
    tft.print(fast_array[fast_text]);
  }
}

void runFast(){
/* loops through the getRestaurantFast function and records the time to run
  then prints the recent time and average time to display
  Arguments: none
  Returns: void
*/
  // reset the current block start for each run
  current_block_start = NULL;

  // start the timer
  startTime = millis();

  // loop through all the resturants
  for (int i=0; i<NUM_RESTAURANTS;i++){
    getRestaurantFast(i,&rest);
  }
  // stop the time and calculate sum and increase count
  endTime = millis();
  fast_recent = endTime - startTime;
  fast_sum += fast_recent;
  fast_count++;

  pinMode(YP, OUTPUT);
  pinMode(XM, OUTPUT);
  // over old text with box and write new time
  tft.fillRect(0,155,DISPLAY_WIDTH/2,24 ,TFT_BLACK);
  tft.setCursor(0,155); tft.print(fast_recent); tft.print(" ms");
  tft.fillRect(0,220,DISPLAY_WIDTH/2,24 ,TFT_BLACK); 
  tft.setCursor(0,220);tft.print(fast_sum/fast_count); tft.print(" ms");

}

void runSlow(){
/*
  Loops through the slow getRestaurant function and prints the time elapsed and average time to display
  Arguments: none
  Returns: void
*/

  //start timer
  startTime = millis();

  // loop through all the restaurants
  for (int i=0; i<NUM_RESTAURANTS;i++){
    getRestaurant(i,&rest);
  }

  // stop timer
  endTime = millis();
  slow_recent = endTime - startTime;
  slow_sum += slow_recent;
  slow_count++;

  pinMode(YP, OUTPUT);
  pinMode(XM, OUTPUT);

  // Cover old text and write new text
  tft.fillRect(0,25,DISPLAY_WIDTH/2,24 ,TFT_BLACK);
  tft.setCursor(0,25); tft.print(slow_recent); tft.print(" ms");
  tft.fillRect(0,90,DISPLAY_WIDTH/2,24 ,TFT_BLACK); 
  tft.setCursor(0,90);tft.print(slow_sum/slow_count); tft.print(" ms");

}

int main() {
  setup();
  // draw the buttons and intial tezt
  tft_start_display();

  // infinite loop
  while (true){
    pinMode(YP, INPUT);
    pinMode(XM, INPUT);
    TSPoint touch = ts.getPoint();
  
    // if the user presses the screen
    if (touch.z >= MINPRESSURE && touch.z <= MAXPRESSURE) {
      // get he points
      int ptx = map(touch.y, TS_MINX, TS_MAXX, 0, DISPLAY_WIDTH);
      int pty = map(touch.x, TS_MINY, TS_MAXY, 0, DISPLAY_HEIGHT);
      if (ptx < 60 && pty > 320/2){
        // slow button
        runSlow();
      }else if (ptx < 60 && pty < 320/2){
        // fast button
        runFast();
      }
      delay(100);
    }
  }
  Serial.end();
  return 0;
}