#include <mutex>

#pragma once
extern std::mutex juicer_mutex;

class GlobalState
{
protected:
    static GlobalState* mState ;
private:
    float mVoltage = 120.0;
public:                           // Access specifier
    static char DeviceID[50]; 
    static char DeviceMAC[20];     
    
    static void setupGlobalState(); // Method/function declaration
    static GlobalState *getInstance();
    void loopGlobalState();
    float getSysVoltage() {return mVoltage;}
};