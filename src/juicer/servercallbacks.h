
/**  None of these are required as they will be handled by the library with defaults. **
 **                       Remove as you see fit for your needs                        */
class ServerCallbacks: public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer) ;
    void onConnect(NimBLEServer* pServer, ble_gap_conn_desc* desc) ;
    void onDisconnect(NimBLEServer* pServer);
    void onMTUChange(uint16_t MTU, ble_gap_conn_desc* desc);
    uint32_t onPassKeyRequest();
    bool onConfirmPIN(uint32_t pass_key);
    void onAuthenticationComplete(ble_gap_conn_desc* desc);
};