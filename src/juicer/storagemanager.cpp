#include <Arduino.h>
#include <storagemanager.h>
#include <LittleFS.h>
#include <juicer_constants.h>
#include <logging.h>

/** static singleton */
StorageManager *StorageManager::mManager = NULL;

StorageManager::~StorageManager(){
    if (mBuffer) delete mBuffer ;
}
/**
 * Called on startup
 */
void StorageManager::setupStorage()
{
    if (!StorageManager::mManager)
    {
        StorageManager::mManager = new StorageManager();
    }

    // OPENEVSE ALREADY INITIALIZES THIS
    if (!LittleFS.begin(true))
    {
        logLine("Failed to begin Little FS");
    }

    logLineLevel(10, "Checking data folder");
    if (!LittleFS.exists("/data"))
    {
        logLineLevel(10, "Creating data folder");
        LittleFS.mkdir("/data");
    }

    if (LittleFS.exists("/data/currentsession.key")){
        logLineLevel(10, "Current Session does not exist");
    }
}

StorageManager *StorageManager::getInstance()
{
    return StorageManager::mManager;
}

const char* StorageManager::readText(const char* path)
{
    char pathName[40] ;
    sprintf(pathName, "/data/%s", path);
    File f = LittleFS.open(pathName, "r");    
    if (!f || !f.available()){
        if (f) f.close();
        return NULL ;
    }
    allocateBuffer(f.size());
    int nLen = f.readBytes(mBuffer, f.size());
    f.close();
    return mBuffer ;
}

/**
 * Allocate a buffer for this write
*/
void StorageManager::allocateBuffer(size_t size){
    if (mBuffer) delete [] mBuffer ;
    mBuffer = NULL ;

    int arrLength = size + 1 ;
    char* buff = new char[arrLength];
    memset(buff, 0, arrLength);
    mBuffer = buff ;
}


bool StorageManager::writeText(const char* path, const char* s)
{
    char pathName[40] ;
    sprintf(pathName, "/data/%s", path);
    File f = LittleFS.open(pathName, "w", true);
    if (!f){
        if (f) f.close();
        return false ;
    }
    f.print(s);
    f.close();
    return true ;
}

