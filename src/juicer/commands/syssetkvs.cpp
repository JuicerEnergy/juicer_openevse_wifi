#include <Arduino.h>
#include <rpccommand.h>
#include <logging.h>
#include <allcommands.h>
#include <storagemanager.h>

char output[300];

void SysSetKVSCmd::executeCommand()
{
    logLineLevel(10, "executing %s", mCommandName);
    
    const char *responseErrFmt = "{'src':'%s'}";
    const char *responseFmt = "{'src':'%s','result':{'value': %s}}";

    if (!mpCommandJSON || !(mpCommandJSON->containsKey("params") && ((*mpCommandJSON)["params"]).containsKey("key")) && ((*mpCommandJSON)["params"]).containsKey("value"))
    {
        sprintf(response, responseErrFmt, JUICER_MACID);
    }
    else
    {
        char filename[50];
        const char *keyname = (*mpCommandJSON)["params"]["key"];
        sprintf(filename, "%s.key", keyname);
        serializeJson((*mpCommandJSON)["params"]["value"], output);
        StorageManager::getInstance()->writeText(filename, output);
        sprintf(response, responseFmt, JUICER_MACID, "{}");
    }
    replaceQuotes(response);
    mpCommandSource->sendResponse(response);
}
