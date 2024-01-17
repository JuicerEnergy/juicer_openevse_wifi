#include <Arduino.h>
#include <ArduinoJson.h>
#include <logging.h>
#include <blemanager.h>
#include <relaymanager.h>
#include <processor.h>
#include <storagemanager.h>
#include <powermanager.h>
#include <globalstate.h>
#include <emonesp.h>
#include <websocket.h>
#include "evse_man.h"
#include "juicer_main.h"

std::mutex juicer_mutex;
extern EvseManager evse;

void juicer_setup()
{
  logLine("Starting Juicer Setup !\n");
  StorageManager::setupStorage();
  GlobalState::setupGlobalState();
  BLEManager::setupBLE();
  JuicerWebSocketTask::setupWebSockets();
  RelayManager::setupRelay();
  CommandProcessor::setupProcessor();
  PowerManager::setupPM();

  String devname = GlobalState::getInstance()->getPropertyStr(PROP_DEVICE_NAME);
  if (!devname.isEmpty())
  {
    logLineLevel(10, "Device name is %s", devname.c_str());
  }
  long level = GlobalState::getInstance()->getPropertyLong(PROP_SERVICE_LEVEL);
  logLineLevel(10, "Service Level is %ld", level);
  long voltage = GlobalState::getInstance()->getPropertyLong(PROP_VOLTAGE);
  logLineLevel(10, "Voltage is %ld", voltage);
  long maxamps = GlobalState::getInstance()->getPropertyLong(PROP_MAX_AMPS);
  logLineLevel(10, "Maxamps is %ld", maxamps);
  logLineLevel(10, "Enable Web Server : %ld", GlobalState::getInstance()->getPropertyLong(PROP_WEB_SERVER));

  /**
   * Initialize the EV parameters from EPROM
   */
  if (level != 0 && voltage != 0 && maxamps >= 6)
  {
    logLine("Initializing EV settings from memory.");
    evse.setServiceLevel((EvseMonitor::ServiceLevel)GlobalState::getInstance()->getPropertyLong(PROP_SERVICE_LEVEL));
    evse.setVoltage(GlobalState::getInstance()->getPropertyLong(PROP_VOLTAGE));
    evse.setMaxConfiguredCurrent(GlobalState::getInstance()->getPropertyLong(PROP_MAX_AMPS));
  }
  // StorageManager::getInstance()->writeText("currentsession.key", "");
  logLine("Done Juicer Setup !\n");
}

unsigned long SlowDown; // Timer for events once every 5 seconds
void juicer_loop_slow();
void juicer_loop()
{
  // JUCR
  if ((millis() - SlowDown) >= 5000)
  {
    uint32_t flash_size = ESP.getFlashChipSize();
    logLine("Juicer is live %ld", flash_size);
    logLine("ActiveState : %s", evse.getState().toString());
    juicer_loop_slow();
    SlowDown = millis();
  }
  else
  {
  }
}

void juicer_loop_slow()
{
  std::lock_guard<std::mutex> lck(juicer_mutex);
  logLine("Juicer loop ! Free:%ld, Max Alloc: %ld, Min Free: %ld\n", ESP.getFreeHeap(), ESP.getMaxAllocHeap(), ESP.getMinFreeHeap());

  // RelayManager::getInstance()->loopRelay();
  if (!BLEManager::getInstance())
  {
    logLine("Setting up BLE");
    BLEManager::setupBLE();
  }
  BLEManager::getInstance()->loopBLE();
  CommandProcessor::getInstance()->loopProcessor();
  // PowerManager::getInstance()->loopPM();
  JuicerWebSocketTask::getInstance()->wsLoop();
}

/**
 * An event came to juicer from OpenEVSE modules
 */
void juicer_event(JsonDocument &event)
{
  // String json;
  // serializeJson(event, json);
  // logLineLevel(10, "EVENT RECEIVED : %s", json.c_str());
  if (event.containsKey("total_energy"))
  {
    double energy = event["total_energy"];
    logLineLevel(10, "Energy Used : %f", energy);
    PowerManager::getInstance()->setTotalEnergy(energy);
  }
  if (event.containsKey("amp") && event.containsKey("power"))
  {
    double amps = event["amp"];
    amps = amps / AMPS_SCALE_FACTOR;
    double power = event["power"];
    logLineLevel(10, "Amps : %f, Power: %f", amps, power);
    PowerManager::getInstance()->setCurrent(amps);
    PowerManager::getInstance()->setPower(power);
  }
}
