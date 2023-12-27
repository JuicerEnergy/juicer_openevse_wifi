#include <Arduino.h>
#include <logging.h>

bool relayState = false;
int relayToggleTime = 0;
#define RELAY_PIN 15

void relayOff(){
  digitalWrite(RELAY_PIN, LOW); // Turn off the relay
  relayToggleTime = 0;
  relayState = false;
  // code to turn off relay
  logLine("Relay OFF");
}

void relayOn(){
  relayState = true;
  digitalWrite(RELAY_PIN, HIGH); // Turn off the relay
  // code to turn on relay
  logLine("Relay ON");
}

void resetRelay()
{
  relayOff();
}

void setupRelay(){
  pinMode(RELAY_PIN, OUTPUT);
}

/**
 * Set the relay state to on or off, with an option to toggle after a specified
 * interval in seconds
 */
void setRelay(bool onoff, int toggleAfter = 0)
{
  char s[80];
  sprintf(s, "Relay turned %d, for %d ", onoff, toggleAfter);
  logLine(s);
  if (onoff)
  {
    if (toggleAfter > 0)
    {
      relayToggleTime = millis() + toggleAfter * 1000;
    }
    else
    {
      relayToggleTime = 0;
    }
    relayOn();
  }
  else
  {
    relayOff();
  }
}

void loopRelay()
{
  if (relayState && relayToggleTime > 0)
  {
    if (millis() > relayToggleTime)
      resetRelay();
  }
}


