#include <Arduino.h>
#include <ArduinoJson.h>
#include <rpccommand.h>
#include <logging.h>
#include <allcommands.h>

void WifiGetStatusCmd::executeCommand()
{
    logLineLevel(10, "executing %s", mCommandName);
    char response[500];
    const char *responseFmt = "{'src':'%s','result':{'status':'not available','sta':{'ssid':'%s'},'ap':{'ssid':'%s'}}}";
    sprintf(response, responseFmt, JUICER_MACID, JUICER_MACID, JUICER_MACID);
    for (int i = 0; i < strlen(response); i++)
    {
        if (response[i] == '\'') response[i] = '\"';
    }
    mpCommandSource->sendResponse(response);
}

/*
reference
{"id":1700244209744,"src":"juicer_123123","result":{"sta":{"enable":false,"ssid":"juicer_123123"},"ap":{"enable":false}}}
*/