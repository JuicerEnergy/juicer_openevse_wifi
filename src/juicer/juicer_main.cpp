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

std::mutex juicer_mutex;

void juicer_setup()
{
  logLine("Starting Juicer Setup !\n");
  StorageManager::setupStorage();
  GlobalState::setupGlobalState();
  BLEManager::setupBLE();
  RelayManager::setupRelay();
  CommandProcessor::setupProcessor();
  PowerManager::setupPM();
  
  // StorageManager::getInstance()->writeText("currentsession.key", "");
  logLine("Done Juicer Setup !\n");
}


void juicer_loop()
{
  std::lock_guard<std::mutex> lck(juicer_mutex);
  logLine("Juicer loop ! Free:%ld, Max Alloc: %ld, Min Free: %ld\n", ESP.getFreeHeap(), ESP.getMaxAllocHeap(), ESP.getMinFreeHeap());
  
  // RelayManager::getInstance()->loopRelay();
  if (!BLEManager::getInstance()){
    logLine("Setting up BLE");
    BLEManager::setupBLE();
  }
  BLEManager::getInstance()->loopBLE();
  CommandProcessor::getInstance()->loopProcessor();
  // PowerManager::getInstance()->loopPM();
}

/**
 * An event came to juicer from OpenEVSE modules
*/
void juicer_event(JsonDocument &event)
{
  // String json;
  // serializeJson(event, json);
  // logLineLevel(10, "EVENT RECEIVED : %s", json.c_str());
  if (event.containsKey("total_energy")){
    double energy = event["total_energy"];
    logLineLevel(10, "Energy Used : %f", energy);
  }
  if (event.containsKey("amp") && event.containsKey("power")){
    double amps = event["amp"];
    amps = amps / AMPS_SCALE_FACTOR;
    double power = event["power"];
    logLineLevel(10, "Amps : %f, Power: %f", amps, power);
    PowerManager::getInstance()->setCurrent(amps);
    PowerManager::getInstance()->setPower(power);
  }
}
