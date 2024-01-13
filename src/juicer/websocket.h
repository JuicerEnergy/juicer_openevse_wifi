#pragma once
#include <MicroTasks.h>
#include <WebSocketsClient.h>
#include <commandsource.h>


#define AMAZON_CA "-----BEGIN CERTIFICATE-----\n\
MIIEkjCCA3qgAwIBAgITBn+USionzfP6wq4rAfkI7rnExjANBgkqhkiG9w0BAQsF\n\
ADCBmDELMAkGA1UEBhMCVVMxEDAOBgNVBAgTB0FyaXpvbmExEzARBgNVBAcTClNj\n\
b3R0c2RhbGUxJTAjBgNVBAoTHFN0YXJmaWVsZCBUZWNobm9sb2dpZXMsIEluYy4x\n\
OzA5BgNVBAMTMlN0YXJmaWVsZCBTZXJ2aWNlcyBSb290IENlcnRpZmljYXRlIEF1\n\
dGhvcml0eSAtIEcyMB4XDTE1MDUyNTEyMDAwMFoXDTM3MTIzMTAxMDAwMFowOTEL\n\
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n\
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n\
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n\
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n\
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n\
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n\
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n\
jgSubJrIqg0CAwEAAaOCATEwggEtMA8GA1UdEwEB/wQFMAMBAf8wDgYDVR0PAQH/\n\
BAQDAgGGMB0GA1UdDgQWBBSEGMyFNOy8DJSULghZnMeyEE4KCDAfBgNVHSMEGDAW\n\
gBScXwDfqgHXMCs4iKK4bUqc8hGRgzB4BggrBgEFBQcBAQRsMGowLgYIKwYBBQUH\n\
MAGGImh0dHA6Ly9vY3NwLnJvb3RnMi5hbWF6b250cnVzdC5jb20wOAYIKwYBBQUH\n\
MAKGLGh0dHA6Ly9jcnQucm9vdGcyLmFtYXpvbnRydXN0LmNvbS9yb290ZzIuY2Vy\n\
MD0GA1UdHwQ2MDQwMqAwoC6GLGh0dHA6Ly9jcmwucm9vdGcyLmFtYXpvbnRydXN0\n\
LmNvbS9yb290ZzIuY3JsMBEGA1UdIAQKMAgwBgYEVR0gADANBgkqhkiG9w0BAQsF\n\
AAOCAQEAYjdCXLwQtT6LLOkMm2xF4gcAevnFWAu5CIw+7bMlPLVvUOTNNWqnkzSW\n\
MiGpSESrnO09tKpzbeR/FoCJbM8oAxiDR3mjEH4wW6w7sGDgd9QIpuEdfF7Au/ma\n\
eyKdpwAJfqxGF4PcnCZXmTA5YpaP7dreqsXMGz7KQ2hsVxa81Q4gLv7/wmpdLqBK\n\
bRRYh5TmOTFffHPLkIhqhBGWJ6bt2YFGpn6jcgAKUj6DiAdjd4lpFw85hdKrCEVN\n\
0FE6/V1dN2RMfjCyVSRCnTawXZwXgWHxyvkQAiSr6w10kY17RSlQOYiypok1JR4U\n\
akcjMS9cmvqtmg5iUaQqqcT5NJ0hGA==\n\
-----END CERTIFICATE-----\n"

class JuicerWebSocketTask : public CommandSource, public MicroTasks::Task 
{
private:
    MicroTasks::EventListener _onQueueChanged;
    class QueueChanged : public MicroTasks::Event
    {
    private:
    public:
        QueueChanged();
        void fire();
    };

protected:
    static JuicerWebSocketTask *mManager;
    QueueChanged __changed;
    WebSocketsClient _wsClient; 
    boolean mPendingStatusUpdate = false; 
    char mTempBuff[500];
protected:
    void setup();
    unsigned long loop(MicroTasks::WakeReason reason);

public: // Access specifier
    JuicerWebSocketTask();
    void wsLoop();
    void begin();
    void sendText(const char* txt);
    static void setupWebSockets(); // Method/function declaration
    static JuicerWebSocketTask *getInstance();
    void closeConnection();

    void sendInitialStatus();
    void sendSwitchStatus();
    void triggerStatusUpdate(){mPendingStatusUpdate = true ; __changed.fire();}
    void sendResponse(const char* response);
};

#define NOTIFY_FULL_FORMAT "{\"src\":\"shellyplus1pm-c049ef8cf4e0\",\"dst\":\"ws\",\"method\":\"NotifyFullStatus\","
#define NOTIFY_FULL_FORMAT1 """{"src":"shellyplus1pm-c049ef8cf4e0","dst":"ws","method":"NotifyFullStatus",\
"params":{\
"switch:0":{"id":0,"source":"init","output":false,"apower":0,"voltage":119.1,"current":0,"aenergy":{"total":0},"temperature":{"tC":19.3,"tF":66.8}},\
"sys":{"mac":"C049EF8CF4E0","uptime":4,"ram_size":259508,"ram_free":126632,"fs_size":458752,"fs_free":147456,\
"cfg_rev":18,"kvs_rev":1591,"schedule_rev":0,"webhook_rev":0,"available_updates":{}},\
"wifi":{"sta_ip":"192.168.1.21","status":"got ip","ssid":"Touchdown","rssi":-37},\
"ws":{"connected":true}}}"""
