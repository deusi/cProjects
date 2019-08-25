// Tried to make my code as concise as possible.
// However, some parts of the code are expanded in order to make it easier
// for a human-reader to understand.

#include "thermo.h"

int set_temp_from_ports(temp_t *temp){
  int temperature = THERMO_SENSOR_PORT;
  // tests if the temperature is above its maximum trusted value
  // no checks for temperature < 0 because THERMO_SENSOR_PORT is unsigned
  if(temperature > 64000){
    return 1;
  }
  // calculates temperature in celsius
  short check = temperature % 64;
  temperature /= 64;
  // adds one if remainder is greater then half of the unit
  if(check > 31){
    temperature++;
  }
  temperature -= 500;
  //converts to fahrenheit if the flag is set
  if(THERMO_STATUS_PORT){
    temperature *= 9;
    temperature /= 5;
    temperature += 320;
  }
  // assigns values to the struct fields
  temp->tenths_degrees = temperature;
  temp->is_fahrenheit = THERMO_STATUS_PORT;

  return 0;
}

int set_display_from_temp(temp_t temp, int *display){
  // insane amount of checks for errors, some of conditions are redundant
  // since they are being checked in set_temp_from_ports(), but you are
  // the boss, so I just implemented it
  if((temp.is_fahrenheit != 0 && temp.is_fahrenheit != 1) ||
    (temp.is_fahrenheit == 0 &&
    (temp.tenths_degrees > 500 || temp.tenths_degrees < -500)) ||
    (temp.is_fahrenheit == 1 &&
    (temp.tenths_degrees > 1220 || temp.tenths_degrees < -580))){
      return 1;
  }
  // an array of masks for different digits, sign and mask
  // could be collapsed into two lines, but severy decreases human readability
  short mask[12];
  mask[0] = 0b0111111;
  mask[1] = 0b0000110;
  mask[2] = 0b1011011;
  mask[3] = 0b1001111;
  mask[4] = 0b1100110;
  mask[5] = 0b1101101;
  mask[6] = 0b1111101;
  mask[7] = 0b0000111;
  mask[8] = 0b1111111;
  mask[9] = 0b1101111;
  mask[10] = 0b0000000;
  mask[11] = 0b1000000;

  short temp_thousand, temp_hundreds, temp_tens, temp_ones;
  short var = temp.tenths_degrees;
  _Bool negative = 0;
  // if the temperature is negative, inverse it and set negative flag to 1
  if(var < 0){
    var = -var;
    negative = 1;
  }
  // get the numbers from which a given variable consist of
  temp_thousand = var / 1000;
  temp_hundreds = (var % 1000) / 100;
  temp_tens = ((var % 1000) % 100) / 10;
  temp_ones = (((var % 1000) % 100) % 10);
  // initialize a new, clean binary sequence
  int displayValue = 0;
  // encode F or C into the code
  displayValue = displayValue | ((temp.is_fahrenheit ? 0b10 : 0b01) << 28);
  // add the leftmost part of the display
  // final output depends on the conditions below
  if(temp_thousand){
  displayValue = displayValue | (mask[temp_thousand] << 21);
  }
  else if(temp_hundreds && negative){
    displayValue = displayValue | (mask[11] << 21);
  }
  else{
    displayValue = displayValue | (mask[10] << 21);
  }
  // add the second leftmost part, depends on conditions below
  if(!temp_hundreds && negative){
    displayValue = displayValue | (mask[11] << 14);
  }
  else if(!temp_hundreds){
    displayValue = displayValue | (mask[10] << 14);
  }
  else{
    displayValue = displayValue | (mask[temp_hundreds] << 14);
  }
  // whatever's left, just add it
  displayValue = displayValue | (mask[temp_tens] << 7);
  displayValue = displayValue | mask[temp_ones];
  // assign displayValue to be a new value of a display (via poiter)
  *display = displayValue;

  return 0;
}

int thermo_update(){
  // define the variables to-be-used
  temp_t temp;
  int display;
  // check if one of the functions fails
  if(set_temp_from_ports(&temp) || set_display_from_temp(temp, &display)){
    return 1;
  }
  // assign a new value to THERMO_DISPLAY_PORT to reflect temperature change
  THERMO_DISPLAY_PORT = display;
  return 0;
}
