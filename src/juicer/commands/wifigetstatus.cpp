#include <Arduino.h>
#include <ArduinoJson.h>
#include <rpccommand.h>
#include <logging.h>
#include <allcommands.h>
#include <app_config.h>
#include <net_manager.h>

extern NetManagerTask net;

void WifiGetStatusCmd::executeCommand()
{
    logLineLevel(10, "executing %s", mCommandName);
    char response[500];
    const char *connected = (net.isWifiClientConnected() ? "got ip" : "disconnected");
    const char *responseFmt = "{'src':'%s','result':{'status':'%s','sta':{'ssid':'%s', 'enable':%s},'ap':{'ssid':'%s','enable':%s}}}";
    sprintf(response, responseFmt, JUICER_MACID, connected, esid.c_str(), esid.length()>0?"true":"false", JUICER_MACID, ap_enabled?"true":"false");
    for (int i = 0; i < strlen(response); i++)
    {
        if (response[i] == '\'') response[i] = '\"';
    }
    mpCommandSource->sendResponse(response);
    logLineLevel(10, "AP SSID is %s", esid.c_str() );
}

/*
reference
{"id":1700244209744,"src":"juicer_123123","result":{"sta":{"enable":false,"ssid":"juicer_123123"},"ap":{"enable":false}}}
*/