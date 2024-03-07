#define MAX_BUFF_LEN 600

class BLEManager;
/**  None of these are required as they will be handled by the library with defaults. **
 **                       Remove as you see fit for your needs                        */
class CharCallbacks : public NimBLECharacteristicCallbacks
{
    BLEManager *pManager;
    int sizeToRead = 0 ;
    int currentBuffLen = 0 ;
    char inputbuff[MAX_BUFF_LEN];

    void onRead(NimBLECharacteristic *pCharacteristic);
    void onWrite(NimBLECharacteristic *pCharacteristic);
    /** Called before notification or indication is sent,
     *  the value can be changed here before sending if desired.
     */
    void onNotify(NimBLECharacteristic *pCharacteristic);

    /** The status returned in status is defined in NimBLECharacteristic.h.
     *  The value returned in code is the NimBLE host return code.
     */
    void onStatus(NimBLECharacteristic *pCharacteristic, NimBLECharacteristicCallbacks::Status status, int code);

    void onSubscribe(NimBLECharacteristic *pCharacteristic, ble_gap_conn_desc *desc, uint16_t subValue);

public:
    void setBLEManager(BLEManager *pMgr) { pManager = pMgr; }
};