#include <Arduino.h>
#include <rpccommand.h>
#include <logging.h>
#include <allcommands.h>
#include <storagemanager.h>

void SysGetKVSCmd::executeCommand()
{
    logLineLevel(10, "executing %s", mCommandName);
    const char *responseErrFmt = "{'id':%ld,'src':'%s'}";
    const char *responseFmt = "{'id':%ld,'src':'%s','result':{'value': %s}}";
    long id = 0 ;

    if (!mpCommandJSON || !(mpCommandJSON->containsKey("params") && ((*mpCommandJSON)["params"]).containsKey("key")))
    {
        sprintf(response, responseErrFmt, (*mpCommandJSON)["id"], JUICER_MACID);
    }
    else
    {
        if (mpCommandJSON->containsKey("id")){
            id = (*mpCommandJSON)["id"];
        }
        char filename[50];
        const char *keyname = (*mpCommandJSON)["params"]["key"];
        sprintf(filename, "%s.key", keyname);
        const char* keyvalue = StorageManager::getInstance()->readText(filename);
        if (!keyvalue){
            sprintf(response, responseFmt, id, JUICER_MACID, "{}");
        }else{
            sprintf(response, responseFmt, id, JUICER_MACID, keyvalue);
        }
    }
    replaceQuotes(response);
    mpCommandSource->sendResponse(response);
}
