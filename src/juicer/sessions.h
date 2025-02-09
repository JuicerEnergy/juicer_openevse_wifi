#pragma once
#include <ArduinoJson.h>

class SessionInfo{
    DynamicJsonDocument *sessionDoc = NULL;
    char writeBuffer[300];
    public :
        ~SessionInfo();
        static SessionInfo *loadSession() ;
        bool saveSession();

        const char* getSessionID();
        float getSessionEnergy();
        void addSessionEnergy(float energy);
        void setSessionEnergy(float energy);
        
        void setUpTimeSecs(long secs);        
};