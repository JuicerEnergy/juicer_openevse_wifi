#include <Arduino.h>
#include <rpccommand.h>
#include <logging.h>
#include <allcommands.h>
#include <storagemanager.h>

void SysGetKVSCmd::executeCommand()
{
    logLineLevel(10, "executing %s", mCommandName);
    const char *responseErrFmt = "{'src':'%s'}";
    const char *responseFmt = "{'src':'%s','result':{'value': %s}}";

    if (!mpCommandJSON || !(mpCommandJSON->containsKey("params") && ((*mpCommandJSON)["params"]).containsKey("key")))
    {
        sprintf(response, responseErrFmt, JUICER_MACID);
    }
    else
    {
        char filename[50];
        const char *keyname = (*mpCommandJSON)["params"]["key"];
        sprintf(filename, "%s.key", keyname);
        const char* keyvalue = StorageManager::getInstance()->readText(filename);
        if (!keyvalue){
            sprintf(response, responseFmt, JUICER_MACID, "{}");
        }else{
            sprintf(response, responseFmt, JUICER_MACID, keyvalue);
        }
    }
    replaceQuotes(response);
    mpCommandSource->sendResponse(response);
}
