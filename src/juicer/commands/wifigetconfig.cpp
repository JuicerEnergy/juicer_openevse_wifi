#include <Arduino.h>
#include <ArduinoJson.h>
#include <rpccommand.h>
#include <logging.h>
#include <allcommands.h>
#include <app_config.h>
#include <net_manager.h>
void WifiGetConfigCmd::executeCommand()
{
    logLineLevel(10, "executing %s", mCommandName);
    char response[500];
    const char *responseFmt = "{'src':'%s','result':{'sta':{'ssid':'%s', 'enable':%s},'ap':{'ssid':'%s','enable':%s}}}";
    sprintf(response, responseFmt, JUICER_MACID, esid.c_str(), esid.length()>0?"true":"false", ap_ssid, ap_enabled?"true":"false");
    for (int i = 0; i< strlen(response); i++){
        if (response[i] == '\'') response[i] = '\"';
    }
    mpCommandSource->sendResponse(response);
}

/*
reference
{"id":1700244209744,"src":"juicer_123123","result":{"sta":{"enable":false,"ssid":"juicer_123123"},"ap":{"enable":false}}}
*/