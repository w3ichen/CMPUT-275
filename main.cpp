// Adding libraries
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <SPI.h>
#include <SD.h>
#include "lcd_image.h"
MCUFRIEND_kbv tft;
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

// the cursor position on the display
int cursorX, cursorY;
int yegMapX = YEG_SIZE/2 - (DISPLAY_WIDTH - 60)/2;
int yegMapY = YEG_SIZE/2 - DISPLAY_HEIGHT/2;
// forward declaration for redrawing the cursor
void redrawCursor(uint16_t colour);

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

	Serial.print("Initializing SD card...");
	if (!SD.begin(SD_CS)) {
		Serial.println("failed! Is it inserted properly?");
		while (true) {}
	}
	Serial.println("OK!");

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
void scrollMap(){
  // constrain to inside the YEG map
  yegMapX = constrain(yegMapX,0,YEG_SIZE-DISPLAY_WIDTH-60);
  yegMapY = constrain(yegMapY,0,YEG_SIZE-DISPLAY_HEIGHT);
  // draw the map
  lcd_image_draw(&yegImage, &tft, yegMapX, yegMapY,
                 0, 0, DISPLAY_WIDTH - 60, DISPLAY_HEIGHT);
  // reset cursor to middle
  cursorX = (DISPLAY_WIDTH - 60)/2;
  cursorY = DISPLAY_HEIGHT/2;
}

void redrawCursor(uint16_t colour) {
  /*
    Redraws the red cursor and constrains the cursor
    Arguemnts: the color of the cursor
    Returns: void
  */
  int initialX = cursorX;
  int initialY = cursorY;
  // constrain cursor to edge of map
  cursorX = constrain(cursorX,CURSOR_SIZE/2, DISPLAY_WIDTH-60-(CURSOR_SIZE/2));
  cursorY = constrain(cursorY,CURSOR_SIZE/2,DISPLAY_HEIGHT-(CURSOR_SIZE/2));
  if (cursorX - initialX < 0){
    // too far to the right , scoll to the right
    if (yegMapX > 0 && yegMapX < YEG_SIZE-DISPLAY_WIDTH-60){
      yegMapX += DISPLAY_WIDTH-60; 
      scrollMap();
    }
  }else if (cursorX - initialX > 0){
    // too far to the left, scroll to the left
    if (yegMapX > 0 && yegMapX < YEG_SIZE-DISPLAY_WIDTH-60){ 
      yegMapX -= DISPLAY_WIDTH-60;
      scrollMap();
    }
  }
  if (cursorY - initialY < 0){
    // too far to the bottom , scoll down
    if (yegMapY > 0 && yegMapY < YEG_SIZE-DISPLAY_HEIGHT){
      yegMapY += DISPLAY_HEIGHT; 
      scrollMap();
    }
  }else if (cursorY - initialY > 0){
    // too far to the top, scroll to the top
    if (yegMapY > 0 && yegMapY < YEG_SIZE-DISPLAY_HEIGHTg){
      yegMapY -= DISPLAY_HEIGHT; 
      scrollMap();
  }
  }
  // draw the new cursor
  tft.fillRect(cursorX - CURSOR_SIZE/2, cursorY - CURSOR_SIZE/2,
               CURSOR_SIZE, CURSOR_SIZE, colour);

}

void moveCursorBackground(){
  /*
    Covers the trail of the cursor by drawing a over trail with small section of the map
    Arguments: none
    Returns: void function
  */
  // draw small patch of map over cursor trail
  lcd_image_draw(&yegImage, &tft,yegMapX+(cursorX- CURSOR_SIZE/2),yegMapY+(cursorY- CURSOR_SIZE/2), 
                cursorX - CURSOR_SIZE/2, cursorY - CURSOR_SIZE/2,CURSOR_SIZE,CURSOR_SIZE);
}

int x_speed(int xVal){
  /*
    Determines the speed of cursor based on how far joystick is pushed
    Arguments: x value of joystick
    Returns: speed in x direction
  */
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

int y_speed(int yVal){
  /*
    Determines the speed of cursor based on how far joystick is pushed
    Arguments: y value of joystick
    Returns: speed in y direction
  */
  if (yVal < JOY_CENTER){
    // bottom half
    double distance = yVal - JOY_CENTER;
    int speed = (distance/JOY_CENTER)*15;
    return speed;
  }else if (yVal > JOY_CENTER){
    // top half
    double distance = yVal - JOY_CENTER;
    int speed = (distance/JOY_CENTER)*15;
    return speed;
  }
}

void processJoystick() {
  /*
    Moves cursor according to joystick inputs
    Arguements: none
    Returns:none
  */
  // Read in joystick 
  int xVal = analogRead(JOY_HORIZ);
  int yVal = analogRead(JOY_VERT);
  int buttonVal = digitalRead(JOY_SEL);
  // call speed functions get get speed
  int horizontal_speed = x_speed(xVal);
  int vertical_speed = y_speed(yVal);

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

int main() {
	setup();

  while (true) {
    processJoystick();
  }

	Serial.end();
	return 0;
}