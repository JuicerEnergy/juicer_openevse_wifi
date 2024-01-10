
#include <Arduino.h>
#include <juicer_constants.h>
#include <processor.h>
#include <logging.h>
#include <globalstate.h>
#include <WiFi.h>
#include <storagemanager.h>
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
        strcpy(DeviceMAC, address.c_str());
        strcpy(DeviceID, DEVICE_ID_PREFIX);
        int devidlength = strlen(DeviceID);
        for (int i = 0; i < address.length(); i++)
        {
            if (address[i] != ':')
            {
                DeviceID[devidlength] = address[i];
                DeviceID[devidlength + 1] = 0;
                devidlength++;
            }
        }
        GlobalState::mState->loadGlobalState();
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

void GlobalState::setPropertyStr(const char* propName, const char* val){
    mSettings[propName] = val;
    saveGlobalState();
}

void GlobalState::loopGlobalState()
{
}
