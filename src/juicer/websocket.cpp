#include <Arduino.h>
#include <websocket.h>
#include <logging.h>
#include <net_manager.h>
#include <globalstate.h>
#include <powermanager.h>
#include <emonesp.h>
extern EvseManager evse;

JuicerWebSocketTask *JuicerWebSocketTask::mManager = NULL;

void onWebSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{

    switch (type)
    {
    case WStype_DISCONNECTED:
        JuicerWebSocketTask::getInstance()->closeConnection();
        logLine("[WSc] Disconnected!\n");
        break;
    case WStype_CONNECTED:
    {
        logLine("[WSc] Connected to url: %s\n", payload);

        // send message to server when Connected
        JuicerWebSocketTask::getInstance()->sendInitialStatus();
    }
    break;
    case WStype_TEXT:
        logLine("[WSc] get text: %s\n", payload);

        // send message to server
        // webSocket.sendTXT("message here");
        break;
    case WStype_BIN:
        logLine("[WSc] get binary length: %u\n", length);

        // send data to server
        // webSocket.sendBIN(payload, length);
        break;
    case WStype_ERROR:
        logLine("WS Error");
        break;
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
        break;
    }
}

JuicerWebSocketTask::JuicerWebSocketTask() : MicroTasks::Task(),
                                             _onQueueChanged(this)
{
}

JuicerWebSocketTask::QueueChanged::QueueChanged() : MicroTasks::Event()
{
}

void JuicerWebSocketTask::QueueChanged::fire()
{
    Trigger();
}

/**
 * Called on startup
 */
void JuicerWebSocketTask::setupWebSockets()
{
    if (!JuicerWebSocketTask::mManager)
    {
        JuicerWebSocketTask::mManager = new JuicerWebSocketTask();
    }

    mManager->begin();
}

JuicerWebSocketTask *JuicerWebSocketTask::getInstance()
{
    return JuicerWebSocketTask::mManager;
}

void JuicerWebSocketTask::begin()
{
    MicroTask.startTask(this);
}

void JuicerWebSocketTask::setup()
{
    __changed.Register(&_onQueueChanged);
}

/**
 * Periodic update of websocket information. every 5 seconds, check and connect
 */
unsigned long JuicerWebSocketTask::loop(MicroTasks::WakeReason reason)
{
    _wsClient.loop();
    return 10;
}

void JuicerWebSocketTask::sendText(const char *txt)
{
    _wsClient.sendTXT(txt);
}

void JuicerWebSocketTask::wsLoop()
{
    logLineLevel(10, "Web socket checking ...");
    if (!_wsClient.isConnected() && net.isConnected())
    {
        logLineLevel(10, "Web socket connecting ...");
        _wsClient.onEvent(onWebSocketEvent);
        _wsClient.begin("192.168.1.23", 3001, "/", "ws");
        // _wsClient.beginSslWithCA("hub.juicerenergy.net", 443, "/", AMAZON_CA, "wss");
    }
    if (!net.isConnected())
    {
        _wsClient.disconnect();
    }
}

void JuicerWebSocketTask::closeConnection()
{
    logLineLevel(10, "Disconnecting web socket, will check later");
    _wsClient.disconnect();
    _wsClient.begin("", 0);
}

/**
 * On first connection send the initial status information
 */
void JuicerWebSocketTask::sendInitialStatus()
{
    PowerManager *pm = PowerManager::getInstance();
    DynamicJsonDocument *pDoc = new DynamicJsonDocument(1024);
    (*pDoc)["src"] = GlobalState::getInstance()->DeviceID ;
    (*pDoc)["dst"] = "ws" ;
    (*pDoc)["method"] = "NotifyFullStatus" ;
    JsonObject params = pDoc->createNestedObject("params");
    JsonObject sw = params.createNestedObject("switch:0");
    sw["id"] = 0 ;
    sw["output"] = false ;
    sw["apower"] = pm->getLastPower() ;
    sw["voltage"] = GlobalState::getInstance()->getSysVoltage() ;
    sw["current"] = pm->getCurrent();
    JsonObject aenergy = params.createNestedObject("aenergy");
    aenergy["total"] = pm->getTotalEnergy();
    JsonObject temp = params.createNestedObject("temperature");
    if(evse.isTemperatureValid(EVSE_MONITOR_TEMP_MONITOR)) {
        temp["tC"] = evse.getTemperature(EVSE_MONITOR_TEMP_MONITOR) * TEMP_SCALE_FACTOR;
    }
    temp["tF"] = 0;
    JsonObject sys = params.createNestedObject("sys");
    sys["mac"] = GlobalState::getInstance()->DeviceMAC;
    JsonObject wifi = params.createNestedObject("wifi");
    wifi["sta_ip"] = net.getIp();
    wifi["ssid"] = esid;
    wifi["ssid"] = "got ip";
    serializeJson(*pDoc, mTempBuff);
    _wsClient.sendTXT(mTempBuff);
}





/*
PROC:
{"src":"shellyplus1pm-c049ef8cf4e0","dst":"ws","method":"NotifyFullStatus",
"params":
{"ts":5.01,
"switch:0":{"id":0,"source":"init","output":false,"apower":0,"voltage":119.1,"current":0,"aenergy":{"total":0},"temperature":{"tC":19.3,"tF":66.8}},
"sys":{"mac":"C049EF8CF4E0","uptime":4,"ram_size":259508,"ram_free":126632,"fs_size":458752,"fs_free":147456,
"cfg_rev":18,"kvs_rev":1591,"schedule_rev":0,"webhook_rev":0,"available_updates":{}},
"wifi":{"sta_ip":"192.168.1.21","status":"got ip","ssid":"Touchdown","rssi":-37},
"ws":{"connected":true}}}

*/