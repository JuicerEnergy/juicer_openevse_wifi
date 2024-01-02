#include <Arduino.h>
#include <blemanager.h>
#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLEService.h>
#include <NimBLEAdvertising.h>
#include <NimBLEAddress.h>
#include <NimBLECharacteristic.h>
#include <juicer_constants.h>
#include <logging.h>
#include <charcallbacks.h>
#include <globalstate.h>

void CharCallbacks::onRead(NimBLECharacteristic *pCharacteristic)
{
    std::lock_guard<std::mutex> lck(juicer_mutex);
    // logLine(pCharacteristic->getUUID().toString().c_str());
    // logLine(": onRead(), value: ");
    // logLine(pCharacteristic->getValue().c_str());
};

/**
 * When an external party has written a characteristic, this is called.
 * Now, we have to, depending on which characteristic, convert that into a string or an int
 */
void CharCallbacks::onWrite(NimBLECharacteristic *pCharacteristic)
{
    std::lock_guard<std::mutex> lck(juicer_mutex);
    {
        char buff[300];
        std::string uuid = std::string(pCharacteristic->getUUID());
        NimBLEAttValue val = pCharacteristic->getValue();
        if (uuid.compare(JUICER_CHARACTERISTIC_W))
        {
            const char *input = val.c_str();
            // sprintf(buff, "String : %s", input);
            // logLine(buff);
            pManager->onInputStringReceived(input);
        }
        else if (uuid.compare(JUICER_CHARACTERISTIC_RW))
        {
            const uint8_t *pData = val.data();
            // sprintf(buff, "Bytes length : %d, [%d, %d, %d, %d]", val.length(), pData[0], pData[1], pData[2], pData[3]);
            // logLine(buff);
            uint size = 0;
            pManager->onInputSizeReceived(size);
        }
    }
    // logLine("ONWRITE END! Free:%ld, Max Alloc: %ld, Min Free: %ld\n", ESP.getFreeHeap(), ESP.getMaxAllocHeap(), ESP.getMinFreeHeap());
};
/** Called before notification or indication is sent,
 *  the value can be changed here before sending if desired.
 */
void CharCallbacks::onNotify(NimBLECharacteristic *pCharacteristic)
{
    logLine("Sending notification to clients");
};

/** The status returned in status is defined in NimBLECharacteristic.h.
 *  The value returned in code is the NimBLE host return code.
 */
void CharCallbacks::onStatus(NimBLECharacteristic *pCharacteristic, NimBLECharacteristicCallbacks::Status status, int code)
{
    String str = ("Notification/Indication status code: ");
    str += status;
    str += ", return code: ";
    str += code;
    str += ", ";
    str += NimBLEUtils::returnCodeToString(code);
    // Serial.println(str);
};

void CharCallbacks::onSubscribe(NimBLECharacteristic *pCharacteristic, ble_gap_conn_desc *desc, uint16_t subValue)
{
    String str = "Client ID: ";
    str += desc->conn_handle;
    str += " Address: ";
    str += std::string(NimBLEAddress(desc->peer_ota_addr)).c_str();
    if (subValue == 0)
    {
        str += " Unsubscribed to ";
    }
    else if (subValue == 1)
    {
        str += " Subscribed to notfications for ";
    }
    else if (subValue == 2)
    {
        str += " Subscribed to indications for ";
    }
    else if (subValue == 3)
    {
        str += " Subscribed to notifications and indications for ";
    }
    str += std::string(pCharacteristic->getUUID()).c_str();

    // Serial.println(str);
};
