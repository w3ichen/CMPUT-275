#include "serial_handling.h"

extern shared_vars shared;

// max size of buffer, including null terminator
const uint16_t buf_size = 256;
// current number of chars in buffer, not counting null terminator
uint16_t buf_len = 0;
// input buffer
char* buffer = (char *) malloc(buf_size);


void process_line() {
  // print what's in the buffer back to server
  Serial.println(buffer);

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

  // for now, nothing is stored
  shared.num_waypoints = 0;
/*
  // set up buffer as empty string
  buf_len = 0;
  buffer[buf_len] = 0;

  // sending request to server
  bufferAdd('R'); bufferAdd(' ');bufferAdd(start.lon);bufferAdd(' ');
  bufferAdd(start.lat); bufferAdd(' '); bufferAdd(end.lon); bufferAdd(' ');
  bufferAdd(end.lat);
  process_line();

  
  if (Serial.available()) {
      // read the incoming byte:
      char in_char = Serial.read();

      // if end of line is received, waiting for line is done:
      if (in_char == '\n' || in_char == '\r') {
          // now we process the buffer
          process_line();
          }
      else {
          // add character to buffer, provided that we don't overflow.
          // drop any excess characters.
          if ( buf_len < buf_size-1 ) {
              buffer[buf_len] = in_char;
              buf_len++;
              buffer[buf_len] = 0;
          }
        }
    }

  // 1 indicates a successful exchange, of course you should only output 1
  // in your final solution if the exchange was indeed successful
  // (otherwise, return 0 if the communication failed)
  return 1;
  */
}
