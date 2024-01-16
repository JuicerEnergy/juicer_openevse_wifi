#include <mutex>

#pragma once
#include <ArduinoJson.h>
extern std::mutex juicer_mutex;

class GlobalState
{
protected:
    static GlobalState* mState ;
private:
    float mVoltage = 120.0;
    StaticJsonDocument<128> mSettings;    
public:                           // Access specifier
    static char DeviceID[50]; 
    static char DeviceMAC[20];     
    static void setupGlobalState(); // Method/function declaration
    static GlobalState *getInstance();
    void loopGlobalState();
    void loadGlobalState();
    void saveGlobalState();
    String getPropertyStr(const char* propName);
    void setPropertyStr(const char* propName, String val);

    long getPropertyLong(const char* propName);
    void setPropertyLong(const char* propName, long val);

    float getSysVoltage() {return mVoltage;}
};