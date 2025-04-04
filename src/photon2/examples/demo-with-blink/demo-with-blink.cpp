
#include <decoder-breakout.h>

// enable system background thread
SYSTEM_THREAD(ENABLED);

#define POLL_INTERVAL 200
// unsigned long nextPollMillis = 0;
unsigned long prevCount = 0;

char msgBuff[128];

// ADDR_JUMPERS is the address set by the jumpers on the board
#define ADDR_JUMPERS 0b111

#define CNTR_SIZE 4 // number of bytes to configure counters

DecoderBreakout chip(ADDR_JUMPERS, CNTR_SIZE);

// set blinkMoves true to blink Photon 2 LED different colors
// when encoder moves.  Setting this to false will leave LED
// under control of Particle OS.
bool blinkMoves = false;

void setup()
{

    if (blinkMoves) {
        // Take control of the RGB LED
        RGB.control(true);
    }

    // Setup I2C Buss Master
    Wire.begin(); // Set i2c master mode

    // Set Bus Speed to Fast Mode 400KHz
    // unsigned long mstrClock = 400000l;
    // Wire.setClock(mstrClock);    // this might be Arduino
    // Particle might instead use Wire.setSpeed(CLOCK_SPEED_400KHZ);

    // Setup I2c Timeouts (default)
    // Wire.setWireTimeout();

    // Setup Serial Monitor
    Serial.begin(9600); // Setup serial monitor baud rate
    delay(5000);        // Wait for Serial Monitor to start

    // Setup LS7866 registers
    Serial.print("Setting up counter chip.\r\n");
    bool ok = chip.ChipSetup();
    if (!ok)
    {
        Serial.print("Error setting up chip.\r\n");
        return;
    }
    byte chipAddress = chip.GetChipAddress();
    sprintf(msgBuff, "Using library version %s\r\n", DECODER_BREAKOUT_LIBRARY_VERSION);
    Serial.print(msgBuff);
    byte MCR0 = chip.GetMCR0();
    byte MCR1 = chip.GetMCR1();
    sprintf(msgBuff, "Setup Counter %d at Addr:%02x MCR0:%02x MCR1:%02x\r\n", ADDR_JUMPERS, chipAddress, MCR0, MCR1);
    Serial.print(msgBuff);

    Serial.print("Setup complete.\r\n");
    Serial.print("Starting Device Polling.\r\n");

    // nextPollMillis = millis() + POLL_INTERVAL;
}

void loop()
{

    unsigned long count = 0;
    bool verbose = true; // true to always print, false to only print when count changes

    // Check for polling interval
    //  if (millis() > nextPollMillis){
    //  nextPollMillis = millis() + POLL_INTERVAL;   // save next polling time stamp

    count = chip.CounterPoll();

    if (blinkMoves)
    {
        if (count > prevCount)
        {
            // flash Photon 2 LED green
            RGB.color(0, 255, 0);
        }
        if (count < prevCount)
        {
            // flash Photon 2 LED red
            RGB.color(255, 0, 0);
        }
        if (count == prevCount)
        {
            // flash Photon 2 LED white
            RGB.color(255, 255, 255);
        }
    }

    if (verbose)
    {
        // sprintf(msgBuff, "Counter %d: %08d millis %d nextPollMillis %d\n", ADDR_JUMPERS, count, millis(), nextPollMillis);
        sprintf(msgBuff, "Counter %d: %08d\n", ADDR_JUMPERS, count);
        Serial.print(msgBuff);
    }

    delay(POLL_INTERVAL / 2);

    if (blinkMoves)
    {
        // turn off Photon 2 LED
        RGB.color(0, 0, 0);
    }

    prevCount = count;

    // Particle.process();  // Maintain cloud connection (not needed in SYSTEM_THREAD mode)
    // Calling delay() will yield to the system thread when in SYSTEM_THREAD mode
    delay(POLL_INTERVAL / 2);
}
