#include <Arduino.h>
#include <relaymanager.h>
#include <logging.h>
#include <manual.h>
#include "lcd.h"
#include "mqtt.h"

/** static singleton */
RelayManager *RelayManager::mManager = NULL;
extern ManualOverride manual;
extern EvseManager evse;

RelayManager::RelayManager() : MicroTasks::Task(),
                               _onRelayChanged(this)
{
}

RelayManager::RelayChanged::RelayChanged() : MicroTasks::Event()
{
}

void RelayManager::RelayChanged::fire()
{
  Trigger();
}

/**
 * Called on startup
 */
void RelayManager::setupRelay()
{
  if (!RelayManager::mManager)
  {
    RelayManager::mManager = new RelayManager();
  }

  mManager->begin();
}

RelayManager *RelayManager::getInstance()
{
  return RelayManager::mManager;
}

long RelayManager::getRelayUpTime()
{
  return millis() - relayOnTime;
}

void RelayManager::begin()
{
  MicroTask.startTask(this);
}

void RelayManager::setup()
{
  __changed.Register(&_onRelayChanged);
}

unsigned long RelayManager::loop(MicroTasks::WakeReason reason)
{

  if (relayState && relayToggleTime > 0)
  {
    if (millis() > relayToggleTime)
    {
      resetRelay();
    }
  }

  // if update is required either due to event or due to a flip here
  assertClaimsIfChanged();
  return 5000;
}

void RelayManager::assertClaimsIfChanged()
{
  EvseState targetState = relayState ? EvseState::Active : EvseState::Disabled;
  if (!evse.isError() && evse.getEvseState() != OPENEVSE_STATE_STARTING && evse.getState(EvseClient_OpenEVSE_Manual) != targetState)
  {
    EvseProperties props(targetState);
    props.setChargeCurrent(evse.getMaxCurrent(EvseClient_OpenEVSE_Manual));
    props.setAutoRelease(false);
    manual.release();
    manual.claim(props);
  }
}

void RelayManager::relayOff()
{
  // digitalWrite(RELAY_PIN, LOW); // Turn off the relay
  relayOffTime = millis();
  relayState = false;
  __changed.fire();

  // code to turn off relay
  logLine("Relay OFF");
}

void RelayManager::relayOn()
{
  relayState = true;
  relayOnTime = millis();
  // code to turn on relay
  __changed.fire();
  logLine("Relay ON");
}

void RelayManager::resetRelay()
{
  relayOff();
}

/**
 * Set the relay state to on or off, with an option to toggle after a specified
 * interval in seconds
 */
void RelayManager::setRelay(bool onoff, int toggleAfter = 0)
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

void RelayManager::loopRelay()
{
  if (relayState && relayToggleTime > 0)
  {
    if (millis() > relayToggleTime)
      resetRelay();
  }

  // if (!relayState && (evse.getState(EvseClient_OpenEVSE_Manual) != EvseState::Disabled))
  // {
  //   EvseProperties props(EvseState::Disabled);
  //   props.setChargeCurrent(evse.getMaxCurrent(EvseClient_OpenEVSE_Manual));
  //   props.setAutoRelease(false);
  //   manual.release();
  //   manual.claim(props);
  //   mqtt_publish_override();
  // }
}
