#include <Arduino.h>
#include <blemanager.h>
#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLEService.h>
#include <NimBLEAdvertising.h>
#include <NimBLEAddress.h>
#include <logging.h>
#include <servercallbacks.h>

void ServerCallbacks::onConnect(NimBLEServer *pServer)
{
    logLine("Client connected");
    logLine("Multi-connect support: start advertising");
    NimBLEDevice::startAdvertising();
};
/** Alternative onConnect() method to extract details of the connection.
 *  See: src/ble_gap.h for the details of the ble_gap_conn_desc struct.
 */
void ServerCallbacks::onConnect(NimBLEServer *pServer, ble_gap_conn_desc *desc)
{
    logLine("Client address: ");
    logLine(NimBLEAddress(desc->peer_ota_addr).toString().c_str());
    /** We can use the connection handle here to ask for different connection parameters.
     *  Args: connection handle, min connection interval, max connection interval
     *  latency, supervision timeout.
     *  Units; Min/Max Intervals: 1.25 millisecond increments.
     *  Latency: number of intervals allowed to skip.
     *  Timeout: 10 millisecond increments, try for 5x interval time for best results.
     */
    // pServer->updateConnParams(desc->conn_handle, 24, 48, 0, 60);
};
void ServerCallbacks::onDisconnect(NimBLEServer *pServer)
{
    logLine("Client disconnected - start advertising");
    NimBLEDevice::startAdvertising();
};
void ServerCallbacks::onMTUChange(uint16_t MTU, ble_gap_conn_desc *desc)
{
    Serial.printf("MTU updated: %u for connection ID: %u\n", MTU, desc->conn_handle);
};

/********************* Security handled here **********************
****** Note: these are the same return values as defaults ********/
uint32_t ServerCallbacks::onPassKeyRequest()
{
    logLine("Server Passkey Request");
    /** This should return a random 6 digit number for security
     *  or make your own static passkey as done here.
     */
    return 123456;
};

bool ServerCallbacks::onConfirmPIN(uint32_t pass_key)
{
    logLine("The passkey YES/NO number: ");
    //logLine(pass_key);
    /** Return false if passkeys don't match. */
    return true;
};

void ServerCallbacks::onAuthenticationComplete(ble_gap_conn_desc *desc)
{
    /** Check that encryption was successful, if not we disconnect the client */
    if (!desc->sec_state.encrypted)
    {
        NimBLEDevice::getServer()->disconnect(desc->conn_handle);
        Serial.println("Encrypt connection failed - disconnecting client");
        return;
    }
    logLine("Starting BLE work!");
};
