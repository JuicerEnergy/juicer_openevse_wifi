#include <Arduino.h>
#include <powermanager.h>
#include <relaymanager.h>
#include <juicer_constants.h>
#include <logging.h>
#include <sessions.h>
#include <globalstate.h>

/** static singleton */
PowerManager *PowerManager::mManager = NULL;

PowerManager::PowerManager() : MicroTasks::Task(),
                               _onPowerChanged(this)
{
}

PowerManager::PowerChanged::PowerChanged() : MicroTasks::Event()
{
}

void PowerManager::PowerChanged::fire()
{
  Trigger();
}

/**
 * Called on startup
 */
void PowerManager::setupPM()
{
  if (!PowerManager::mManager)
  {
    PowerManager::mManager = new PowerManager();
    mManager->begin();
  }
}


void PowerManager::begin()
{
  MicroTask.startTask(this);
}

void PowerManager::setup()
{
  __changed.Register(&_onPowerChanged);
  bInitialized = true;
}

unsigned long PowerManager::loop(MicroTasks::WakeReason reason)
{
  loopPM();
  return 5000; // five seconds between checks.
}

PowerManager *PowerManager::getInstance()
{
  return PowerManager::mManager;
}


float getCurrent()
{
  return 0;
}

bool PowerManager::loopPM()
{
 // if the relay is on,
  if (RelayManager::getInstance()->getRelay())
  {
    SessionInfo *pSession = SessionInfo::loadSession();
    if (pSession)
    {
      // This is set in the main event handler in juicer_main
      // lastCurrent = getCurrent();
      // lastPower = lastCurrent * GlobalState::getInstance()->getSysVoltage();
      /** update the power used into the current running session */
      float powerUsageWH = (getTimeMSSinceLastCheck() * lastPower) / MS_PER_HOUR;
      logLineLevel(10, "PM state : Current :%f, power: %f, WH used:%f", lastCurrent, lastPower, powerUsageWH);

      pSession->addSessionEnergy(powerUsageWH);
      pSession->setUpTimeSecs(RelayManager::getInstance()->getRelayUpTime()/1000);
      pSession->saveSession();
      delete pSession;

      // if powerused is lower than threshold
      long powerDownTime = 0;
      if (lastCurrent < LOW_CURRENT_THRESHOLD)
      {
        if (powerDownDetectedTime)
        {
          powerDownTime = millis() - powerDownDetectedTime;
        }
        else
        {
          powerDownDetectedTime = millis();
        }
      }else{
        powerDownDetectedTime = 0 ;
      }

      // if we have started drawing power, mark that
      if (lastCurrent > POWER_DRAW_START_THRESHOLD)
      {
        bPowerDrawStarted = true;
      }

      logLineLevel(10, "Power Started :%s, interruption time : %ld", (bPowerDrawStarted ? "YES" : "NO"), powerDownTime);
      if ((bPowerDrawStarted && powerDownTime > INTERRUPT_GRACE_MS) || (!bPowerDrawStarted && powerDownTime > PLUGIN_GRACE_MS))
      {
        logLineLevel(5, "AUTO SHUTOFF : Power Started :%s, interruption time : %ld", (bPowerDrawStarted ? "YES" : "NO"), powerDownTime);
        doAutoShutoff();
      }
      return true;
    }else{
      logLineLevel(10, "PM not running, Session not found");
    }
  }else{
    lastCheckTime = 0;
    lastCurrent = 0 ;
    lastPower = 0 ;
    logLine("PM not running, Relay is off");
  }
 return false;
}

/**
 * Auto shut off due to power interruption
*/
void PowerManager::doAutoShutoff()
{
  logLine("Auto Shut off");
  RelayManager::getInstance()->setRelay(false, 0);
}

long PowerManager::getTimeMSSinceLastCheck()
{
  if (!lastCheckTime)
  {
    lastCheckTime = millis();
    return 0;
  }
  long timeElapsed = millis() - lastCheckTime;
  lastCheckTime = millis();
  return timeElapsed;
}

/**
 * When a charging session starts, reinitialize everything
 */
void PowerManager::startSession()
{
  logLine("PM Session starting");
  lastCheckTime = 0;
  lastCurrent = 0 ;
  lastPower = 0 ;
  powerDownDetectedTime = 0;
  bPowerDrawStarted = false;
}