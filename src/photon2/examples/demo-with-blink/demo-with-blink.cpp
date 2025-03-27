
#include <decoder-breakout.h>

#define POLL_INTERVAL 200
unsigned long nextPollMillis = 0;
unsigned long prevCount = 0;

// XXX redundant with msgbuff
char msg2Buff[64];

// XXX application
// ADDR_JUMPERS is the address set by the jumpers on the board
#define ADDR_JUMPERS 0b111

// XXX application
#define CNTR_SIZE 4   // number of bytes to configure counters


void setup() {

  // Take control of the RGB LED
  RGB.control(true);

  // Setup I2C Buss Master  
  Wire.begin();    // Set i2c master mode
  
  // Set Buss Speed to Fast Mode 400KHz
  // unsigned long mstrClock = 400000l;
  // Wire.setClock(mstrClock);    // this might be Arduino
  // Particle might instead use Wire.setSpeed(CLOCK_SPEED_400KHZ);

  // Setup I2c Timeouts (default)
  // Wire.setWireTimeout();    

  // Setup Serial Monitor
  Serial.begin(9600);         // Setup serial monitor baud rate
  delay(5000);                // Wait for Serial Monitor to start

  // Setup LS7866 registers
  Serial.print("Setting up counter chip.\r\n");
  ChipSetup(ADDR_JUMPERS, CNTR_SIZE);

  Serial.print("Setup complete.\r\n");
  Serial.print("Starting Device Polling.\r\n");

  nextPollMillis = millis() + POLL_INTERVAL;
 
  }


void loop() {
  
  // XXX application

  unsigned long count = 0;
  bool verbose = true; // true to always print, false to only print when count changes
  
  // Check for polling interval
  if (millis() > nextPollMillis){
    nextPollMillis = millis() + POLL_INTERVAL;   // save next polling time stamp
  
    count = CounterPoll(ADDR_JUMPERS, CNTR_SIZE); 
    if (count > prevCount){
      // flash Photon 2 LED green
      RGB.color(0, 255, 0);
      verbose = true;
    }
    if (count < prevCount){
      // flash Photon 2 LED red
      RGB.color(255, 0, 0);
      verbose = true;
    }
    if (count == prevCount){
      // flash Photon 2 LED white
      RGB.color(255, 255, 255);
    }
    if (verbose){
      sprintf(msg2Buff, "Counter %d: %08d\n", ADDR_JUMPERS, count);
      Serial.print(msg2Buff);
    }
    delay(POLL_INTERVAL/2);
    RGB.color(0, 0, 0);
    prevCount = count;
  }
}
