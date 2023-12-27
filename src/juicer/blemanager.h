#ifndef _JUICER_BLEMANAGER_H
#define _JUICER_BLEMANAGER_H
#include <commandsource.h>

class NimBLEServer;
class NimBLEService;
class NimBLECharacteristic;
class NimBLEAdvertising;

class BLEManager : CommandSource
{ // The class
protected:
  static BLEManager *mManager;

  NimBLEServer *pServer ;
  NimBLEService *pService;
  NimBLECharacteristic *pCharWrite ;
  NimBLECharacteristic *pCharReadWrite;
  NimBLECharacteristic *pCharNotify;
  NimBLEAdvertising *pAdvertising;

  uint mInputSize = 0 ;
  char mInputString[600];
  // char* mInputString = NULL ;
  uint mOutputSize = 0 ;
  char* mOutputString = NULL ;

public:         
  BLEManager();
  static void setupBLE(); // Method/function declaration
  static BLEManager *getInstance();

  void onInputSizeReceived(uint size);
  void onInputStringReceived(const char* pData);
  void sendResponse(const char* response);

  void loopBLE();
};

#endif