
#include <Arduino.h>
#include <juicer_constants.h>
#include <processor.h>
#include <logging.h>
#include <globalstate.h>
#include <WiFi.h>
#include <storagemanager.h>
#include <app_config.h>

char GlobalState::DeviceID[50];
char GlobalState::DeviceMAC[20];

/** static singleton */
GlobalState *GlobalState::mState = NULL;
char saveBuff[1024];

/**
 * Called on startup
 */
void GlobalState::setupGlobalState()
{
    if (!GlobalState::mState)
    {
        GlobalState::mState = new GlobalState();
        String address = WiFi.macAddress();
        strcpy(DeviceID, DEVICE_ID_PREFIX);
        int devidlength = strlen(DeviceID);
        int maclength = 0;
        for (int i = 0; i < address.length(); i++)
        {
            if (address[i] != ':')
            {
                DeviceID[devidlength] = address[i];
                DeviceID[devidlength + 1] = 0;
                devidlength++;

                DeviceMAC[maclength++] = address[i];
                DeviceMAC[maclength] = 0;
            }
        }
        GlobalState::mState->loadGlobalState();
        // set the AP SSID to the device id
        config_set("ap_ssid", String(DeviceID));
        config_commit();
        
        logLine("Device id : %s", DeviceID);
    }
}

void GlobalState::loadGlobalState()
{
    const char* state = StorageManager::getInstance()->readText("globalstate.json");
    if (state){
        deserializeJson(mSettings, state);
    }
}

void GlobalState::saveGlobalState()
{
    serializeJson(mSettings, saveBuff);
    StorageManager::getInstance()->writeText("globalstate.json", saveBuff);
    // delete str;
}


GlobalState *GlobalState::getInstance()
{
    return GlobalState::mState;
}

String GlobalState::getPropertyStr(const char* propName){
    return mSettings[propName];
}

void GlobalState::setPropertyStr(const char* propName, String val){
    mSettings[propName] = val;
    saveGlobalState();
}

long GlobalState::getPropertyLong(const char* propName){
    return mSettings[propName];
}

void GlobalState::setPropertyLong(const char* propName, long val){
    mSettings[propName] = val;
    saveGlobalState();
}

void GlobalState::loopGlobalState()
{
}
