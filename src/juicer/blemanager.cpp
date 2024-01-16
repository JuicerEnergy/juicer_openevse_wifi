#include <Arduino.h>
#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLEService.h>
#include <NimBLEAdvertising.h>
#include <NimBLECharacteristic.h>
#include <utils.h>
#include <logging.h>
#include <charcallbacks.h>
#include <juicer_constants.h>
#include <globalstate.h>
#include <blemanager.h>
#include <servercallbacks.h>
#include <processor.h>

/** static singleton */
BLEManager *BLEManager::mManager = NULL;

static CharCallbacks chrCallbacks;

// NimBLEServer *BLEManager::pServer ;
// NimBLEService *BLEManager::pService;
// NimBLECharacteristic *BLEManager::pCharInput;
// NimBLECharacteristic *BLEManager::pCharOutput;
// NimBLECharacteristic *BLEManager::pCharOutputReady;

BLEManager::BLEManager()
{
    pServer = NULL;
    pService = NULL;
    pCharWrite = NULL;
    pCharNotify = NULL;
    pCharReadWrite = NULL;
    pAdvertising = NULL;
}

/**
 * Called on startup
 */
void BLEManager::setupBLE()
{
    if (!BLEManager::mManager)
    {
        BLEManager::mManager = new BLEManager();
        BLEManager *pManager = BLEManager::mManager;
        chrCallbacks.setBLEManager(pManager);
        NimBLEDevice::init(JUICER_MACID);
        logLineLevel(10, "BLE Mac ID %s", JUICER_MACID);
        pManager->pServer = NimBLEDevice::createServer();
        pManager->pServer->setCallbacks(new ServerCallbacks());
        pManager->pService = pManager->pServer->createService(JUICER_SERVICE);
        pManager->pCharWrite = pManager->pService->createCharacteristic(JUICER_CHARACTERISTIC_W);
        pManager->pCharReadWrite = pManager->pService->createCharacteristic(JUICER_CHARACTERISTIC_RW);
        pManager->pCharNotify = pManager->pService->createCharacteristic(JUICER_CHARACTERISTIC_NOTIFY);

        pManager->pCharWrite->setCallbacks(&chrCallbacks);
        pManager->pCharReadWrite->setCallbacks(&chrCallbacks);

        pManager->pService->start();

        pManager->pAdvertising = NimBLEDevice::getAdvertising(); // create advertising instance
        pManager->pAdvertising->setName(JUICER_MACID);
        pManager->pAdvertising->addServiceUUID(JUICER_SERVICE);  // tell advertising the UUID of our service
        pManager->pAdvertising->start();    
        logLine("BT Started");                    // start advertising
    }
}

BLEManager *BLEManager::getInstance()
{
    return BLEManager::mManager;
}

void BLEManager::loopBLE()
{
    if (!BLEManager::mManager){
        setupBLE(); // setup if not yet setup
    }
}

/**
 * received the input size from client
 */
void BLEManager::onInputSizeReceived(uint size)
{
    mInputSize = size;
    // mInputString = NULL;
    mOutputSize = 0;
    mOutputString = NULL;
}

/**
 * Received input string from client
 */
void BLEManager::onInputStringReceived(const char *pData)
{
    strcpy(mInputString, pData);
    logLineLevel(10, "BLE Received : %s", mInputString);
    // mInputString = releaseAndAllocateString(mInputString, pData);
    mOutputSize = 0;
    mOutputString = NULL;
    CommandProcessor::getInstance()->onCommandReceived(this, mInputString, COMMAND_SOURCE_BLE);
}


void BLEManager::sendResponse(const char* response){
    int resplen = strlen(response)  ;
    if (resplen > 500){
        logLineLevel(0, "WARNING : Sending string longer than 500, %d", resplen);
    }
    char linelen [4] = {0,0,char(resplen/(256)),(char)(resplen%256)};
    logLineLevel(10, "Sending response %s", response);
    // char zero [4] = {0,0,0,10};
    pCharReadWrite->setValue(String(response));
    pCharNotify->setValue(linelen);

    return  ;
}
