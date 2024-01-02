#include <Arduino.h>
#include <juicer_constants.h>
#include <sessions.h>
#include <storagemanager.h>
#include <logging.h>

SessionInfo::~SessionInfo(){
    if (sessionDoc) delete sessionDoc ;
}

/**
 * If there is a current session, load the session
*/
SessionInfo* SessionInfo::loadSession(){
    const char* sess = StorageManager::getInstance()->readText("currentsession.key");
    // logLine("Session load %s", sess);
    if (sess){
        SessionInfo *pSession = new SessionInfo();
        pSession->sessionDoc = new DynamicJsonDocument(1024);
        deserializeJson(*(pSession->sessionDoc), sess);
        return pSession ;
    }
    return NULL;
}

bool SessionInfo::saveSession(){
    serializeJson(*sessionDoc, writeBuffer);
    logLineLevel(5, "Saving session %s", writeBuffer);
    StorageManager::getInstance()->writeText("currentsession.key", writeBuffer);
    return true;
}

const char* SessionInfo::getSessionID(){
    return NULL;
}

float SessionInfo::getSessionEnergy(){
    return 0.0;
}

void SessionInfo::setUpTimeSecs(long secs){
    (*sessionDoc)["secs"] = secs;
}

void SessionInfo::addSessionEnergy(float energy){
    float energydelivered = 0.0 ;
    if (sessionDoc->containsKey("edel")){
        energydelivered = (*sessionDoc)["edel"];
    }
    energydelivered += energy ;
    (*sessionDoc)["edel"] = energydelivered;
}

void SessionInfo::setSessionEnergy(float energy){
    (*sessionDoc)["edel"] = energy;
}
