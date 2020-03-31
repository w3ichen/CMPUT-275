#include "serial_handling.h"

extern shared_vars shared;

// max size of buffer, including null terminator
const uint16_t buf_size = 256;
// current number of chars in buffer, not counting null terminator
uint16_t buf_len = 0;
// input buffer
char* buffer = (char *) malloc(buf_size);
int waypoint_index = 0;

void process_line() {
  // print what's in the buffer back to server
  Serial.println(buffer);
  // clear the buffer
  buf_len = 0;
  buffer[buf_len] = 0;
}
void storeWaypoint(){
  // print what's in the buffer back to server
  String substring;
  substring = strtok(buffer," ");// W 
  substring = strtok(NULL," "); //lon
  shared.waypoints[waypoint_index].lon = substring.toInt();
  substring = strtok(NULL," "); //lat
  shared.waypoints[waypoint_index].lat = substring.toInt();

  waypoint_index ++; //increment index in storage array

  // clear the buffer
  buf_len = 0;
  buffer[buf_len] = 0;
}
void bufferAdd(char newChar){
  buffer[buf_len] = newChar;
  buf_len++;
  buffer[buf_len] = 0;
}

uint8_t get_waypoints(const lon_lat_32& start, const lon_lat_32& end) {
  // Currently this does not communicate over the serial port.
  // It just stores a path length of 0. You should make it communicate with
  // the server over Serial using the protocol in the assignment description.

  // TODO: implement the communication protocol from the assignment
  Serial.flush();

  enum {INITIAL_REQUEST,WAY_NUM, GET_WAYPOINTS, END} curr_mode = INITIAL_REQUEST;
  // for now, nothing is stored
  shared.num_waypoints = 0;

  // set up buffer as empty string
  buf_len = 0;
  buffer[buf_len] = 0;

  char in_char; // to read in char from serial
  String in_str;

  while (curr_mode != END){

    // sending request to server
    if (curr_mode == INITIAL_REQUEST){
      waypoint_index = 0;
      buf_len = 0;
      Serial.print("R ");
      Serial.print(start.lon); Serial.print(" ");
      Serial.print(start.lat); Serial.print(" ");
      Serial.print(end.lon); Serial.print(" ");
      Serial.println(end.lat);
      Serial.flush();
      curr_mode = WAY_NUM;
    }
    if (Serial.available() && curr_mode == WAY_NUM){
int x=0;
      while (in_char != '\n'){
        in_char = Serial.read();
        bufferAdd(in_char); //add to digits of number to buffer    
  shared.tft->setCursor(x,0);x+=20;
  shared.tft->print(in_char);

      }
      // exits while when char is \n
      in_str = strtok(buffer," "); // this is N
      in_str = strtok(NULL, " "); // this is the number

      int waypoints_integer = in_str.toInt(); // N
      shared.num_waypoints = waypoints_integer;

      // send acknowledgement
      Serial.println("A");

      // clear the buffer
      buf_len = 0;
      buffer[buf_len] = 0;
      curr_mode = GET_WAYPOINTS; // next
    }
    
    if (Serial.available() && curr_mode == GET_WAYPOINTS) {
        // read the incoming byte:
        in_char = Serial.read();
        // if end of line is received, waiting for line is done:
        if (in_char == 'E'){
          //end communication
          curr_mode = END; 
          Serial.flush();
          return 1;
        }
        else if (in_char == '\n' || in_char == '\r') {
            // now we process the buffer
            storeWaypoint();
            Serial.println("A"); //send acknowledgement
        }
        else {
            // add character to buffer, provided that we don't overflow.
            // drop any excess characters.
            if ( buf_len < buf_size-1 ) {
                bufferAdd(in_char);
            }
        }
      }
  }

  // 1 indicates a successful exchange, of course you should only output 1
  // in your final solution if the exchange was indeed successful
  // (otherwise, return 0 if the communication failed)
  return 1;
  
}
