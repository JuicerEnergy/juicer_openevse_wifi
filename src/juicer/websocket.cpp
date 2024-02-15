#include <Arduino.h>
#include <websocket.h>
#include <logging.h>
#include <net_manager.h>
#include <globalstate.h>
#include <powermanager.h>
#include <relaymanager.h>
#include <processor.h>
#include <emonesp.h>
#include <juicer_constants.h>

extern EvseManager evse;

JuicerWebSocketTask *JuicerWebSocketTask::mManager = NULL;
long PeriodicTimer = 0;
#define PERIODIC_STATUS_MILLIS 50 * 1000

double dTof(double d)
{
    return (d * 9 / 5) + 32;
}

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
        CommandProcessor::getInstance()->onCommandReceived(JuicerWebSocketTask::getInstance(), (char *)payload, COMMAND_SOURCE_WS);
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
    if (_wsClient)
    {
        _wsClient->loop();
    }
    return 10;
}

void JuicerWebSocketTask::sendText(const char *txt)
{
    if (_wsClient)
    {
        _wsClient->sendTXT(txt);
    }
}

void JuicerWebSocketTask::wsLoop()
{
    if (!net.isConnected() || mPaused)
        return;

    logLineLevel(10, "Web socket checking ...");
    if (!_wsClient)
    {
        _wsClient = new WebSocketsClient();
    }
    if (!_wsClient->isConnected())
    {
        logLineLevel(10, "Web socket connecting ...");
        _wsClient->onEvent(onWebSocketEvent);
        //_wsClient->begin("192.168.1.23", 3001, "/", "ws");
        _wsClient->beginSslWithCA("hub.juicerenergy.net", 443, "/", AMAZON_CA, "wss");
    }
    else
    {
        logLineLevel(10, "Checking for update");
        logLineLevel(10, "Time elapsed: %ld secs", ((millis() - PeriodicTimer)/1000));
        // if a status update is pending, send it.
        if (mPendingStatusUpdate)
        {
            logLineLevel(10, "Pending update, sending");
            PeriodicTimer = millis();
            sendSwitchStatus();
        }
        else if ((millis() - PeriodicTimer) >= PERIODIC_STATUS_MILLIS)
        {
            logLineLevel(10, "Time elapsed, sending");
            PeriodicTimer = millis();
            sendSwitchStatus();
        }
    }
}

void JuicerWebSocketTask::closeConnection()
{
    logLineLevel(10, "Disconnecting web socket, will check later");
    if (_wsClient)
    {
        _wsClient->disconnect();
        delete _wsClient;
        _wsClient = NULL;
    }
}

/**
 * On first connection send the initial status information
 */
void JuicerWebSocketTask::sendInitialStatus()
{
    if (!_wsClient || !_wsClient->isConnected())
    {
        logLineLevel(10, "WS NOT CONNECTED: Skipping full status");
        return;
    }

    PowerManager *pm = PowerManager::getInstance();
    DynamicJsonDocument *pDoc = new DynamicJsonDocument(1024);
    (*pDoc)["src"] = GlobalState::getInstance()->DeviceID;
    (*pDoc)["dst"] = "ws";
    (*pDoc)["method"] = "NotifyFullStatus";
    JsonObject params = pDoc->createNestedObject("params");
    JsonObject sw = params.createNestedObject("switch:0");
    sw["id"] = 0;
    sw["output"] = RelayManager::getInstance()->getRelay();
    sw["apower"] = pm->getLastPower();
    lastSentPower = pm->getLastPower();
    sw["voltage"] = GlobalState::getInstance()->getSysVoltage();
    lastSentVolts = GlobalState::getInstance()->getSysVoltage();
    sw["current"] = pm->getCurrent();
    lastSentCurrent = pm->getCurrent();
    JsonObject aenergy = params.createNestedObject("aenergy");
    aenergy["total"] = pm->getTotalEnergy();
    if (evse.isTemperatureValid(EVSE_MONITOR_TEMP_MONITOR))
    {
        JsonObject temp = params.createNestedObject("temperature");
        double celsius = evse.getTemperature(EVSE_MONITOR_TEMP_MONITOR);
        temp["tC"] = celsius;
        lastSentTempdC = celsius;
        temp["tF"] = dTof(celsius);
    }
    JsonObject sys = params.createNestedObject("sys");
    sys["mac"] = GlobalState::getInstance()->DeviceMAC;
    JsonObject wifi = params.createNestedObject("wifi");
    wifi["sta_ip"] = net.getIp();
    wifi["ssid"] = esid;
    wifi["ssid"] = "got ip";
    serializeJson(*pDoc, mTempBuff);
    _wsClient->sendTXT(mTempBuff);
    delete pDoc;
}

/**
 * On first connection send the initial status information
 */
void JuicerWebSocketTask::sendSwitchStatus()
{
    mPendingStatusUpdate = false;
    if (!_wsClient || !_wsClient->isConnected())
    {
        logLineLevel(10, "WS NOT CONNECTED: Skipping status");
        return;
    }
    PowerManager *pm = PowerManager::getInstance();
    DynamicJsonDocument *pDoc = new DynamicJsonDocument(1024);
    (*pDoc)["src"] = GlobalState::getInstance()->DeviceID;
    (*pDoc)["dst"] = "ws";
    (*pDoc)["method"] = "NotifyStatus";
    JsonObject params = pDoc->createNestedObject("params");
    JsonObject sw = params.createNestedObject("switch:0");
    sw["id"] = 0;
    if (mSwitchStateChanged){
        mSwitchStateChanged = false ;
        sw["output"] = RelayManager::getInstance()->getRelay();
    }
    if (abs(pm->getLastPower() - lastSentPower) > POWER_UPDATE_DELTA){
        sw["apower"] = pm->getLastPower();
        lastSentPower = pm->getLastPower();
    }

    if (abs(pm->getCurrent() - lastSentPower) > CURRENT_UPDATE_DELTA){
        sw["current"] = pm->getCurrent();
        lastSentCurrent = pm->getCurrent();
    }

    if (abs(GlobalState::getInstance()->getSysVoltage() - lastSentVolts) > VOLT_UPDATE_DELTA){
        sw["voltage"] = GlobalState::getInstance()->getSysVoltage();
        lastSentVolts = GlobalState::getInstance()->getSysVoltage();
    }

    JsonObject aenergy = params.createNestedObject("aenergy");
    aenergy["total"] = pm->getTotalEnergy();
    if (evse.isTemperatureValid(EVSE_MONITOR_TEMP_MONITOR))
    {
        JsonObject temp = params.createNestedObject("temperature");
        double celsius = evse.getTemperature(EVSE_MONITOR_TEMP_MONITOR);
        if (abs(celsius - lastSentTempdC) > TEMP_DC_UPDATE_DELTA){
            temp["tC"] = celsius;
            lastSentTempdC = celsius;
            temp["tF"] = dTof(celsius);
        }
    }
    serializeJson(*pDoc, mTempBuff);
    _wsClient->sendTXT(mTempBuff);
    delete pDoc;
}

void JuicerWebSocketTask::sendResponse(const char *response)
{
    if (!_wsClient || !_wsClient->isConnected())
        return;
    _wsClient->sendTXT(response);
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

/*
PROC:{"src":"shellyplus1pm-c049ef8cf4e0","dst":"ws","method":"NotifyStatus",
"params":{
    "ts":1705084173.38,
    "switch:0":
    {"id":0,"output":true,"source":"GATTS","timer_duration":86400,"timer_started_at":1705084173.38}}}
*/

/*
PROC:{"src":"shellyplus1pm-c049ef8cf4e0","dst":"ws","method":"NotifyStatus",
"params":
{"ts":1705084200.7,
"switch:0":
{"id":0,"aenergy":{"by_minute":[0,0,0],"minute_ts":1705084199,"total":0}}}}
*/

/*
PROC:{"src":"shellyplus1pm-c049ef8cf4e0","dst":"ws","method":"NotifyStatus",
"params":
{
    "ts":1705084233.79,
    "switch:0":
    {"id":0,"apower":0,"current":0,"output":false,"source":"loopback","timer_duration":null,"timer_started_at":null}}}

*/

/*
PROC:{"src":"shellyplus1pm-c049ef8cf4e0","dst":"ws","method":"NotifyStatus",
    "params":
    {"ts":1705084242.7,
        "switch:0":
        {"id":0,"temperature":{"tC":22.8,"tF":73.05}}}}

Periodic :
{"ts":1707806460.65,"switch:0":{"id":0,"aenergy":{"by_minute":[0,0,0],"minute_ts":1707806459,"total":1444.015}}}        
*/