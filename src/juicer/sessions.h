#pragma once
#include <ArduinoJson.h>

class SessionInfo{
    DynamicJsonDocument *sessionDoc;
    char writeBuffer[300];
    public :
        ~SessionInfo();
        static SessionInfo *loadSession() ;
        bool saveSession();

        const char* getSessionID();
        float getSessionEnergy();
        void addSessionEnergy(float energy);
        void setUpTimeSecs(long secs);        
};