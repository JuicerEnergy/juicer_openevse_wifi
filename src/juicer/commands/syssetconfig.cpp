#include <Arduino.h>
#include <rpccommand.h>
#include <logging.h>
#include <allcommands.h>

void SysSetConfigCmd::executeCommand()
{
    logLineLevel(10, "executing %s", mCommandName);
    const char *responseErrFmt = "{'src':'%s'}";
    const char *responseFmt = "{'src':'%s','result':{'restart_required': false}}";
    char response[500];
    if (!mpCommandJSON)
    {
        sprintf(response, responseErrFmt, JUICER_MACID);
    }
    else
    {
        if (mpCommandJSON->containsKey("config") && ((*mpCommandJSON)["config"]).containsKey("device") && ((*mpCommandJSON)["config"]["device"]).containsKey("name"))
        {
            const char *name = (*mpCommandJSON)["config"]["device"]["name"];
        }
        sprintf(response, responseFmt, JUICER_MACID);
    }
    replaceQuotes(response);
    mpCommandSource->sendResponse(response);
}
