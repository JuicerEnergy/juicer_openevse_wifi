
#include <Arduino.h>
#include <juicer_constants.h>
#include <processor.h>
#include <logging.h>
#include <globalstate.h>
#include <WiFi.h>
char GlobalState::DeviceID[50];
char GlobalState::DeviceMAC[20];

/** static singleton */
GlobalState *GlobalState::mState = NULL;

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
        logLine("Device id : %s", DeviceID);
    }
}

GlobalState *GlobalState::getInstance()
{
    return GlobalState::mState;
}

void GlobalState::loopGlobalState()
{
}
