#include <Arduino.h>
#include <rpccommand.h>
#include <logging.h>
#include <allcommands.h>

void ShellySetAuthCmd::executeCommand()
{
    logLineLevel(10, "executing set %s", mCommandName);
    const char *responseErrFmt = "{'src':'%s'}";
    const char *responseFmt = "{'src':'%s','result':{'restart_required': false}}";
    char response[500];
    if (!mpCommandJSON)
    {
        logLineLevel(10, "No command JSON");
        sprintf(response, responseErrFmt, JUICER_MACID);
    }
    else
    {
        if (mpCommandJSON->containsKey("params") && ((*mpCommandJSON)["params"]).containsKey("user") && ((*mpCommandJSON)["params"]).containsKey("realm") && ((*mpCommandJSON)["params"]).containsKey("ha1"))
        {
            GlobalState::getInstance()->setPropertyStr(PROP_AUTH_PASSWORD, (*mpCommandJSON)["params"]["ha1"]);
        }
        sprintf(response, responseFmt, JUICER_MACID);
    }
    replaceQuotes(response);
    mpCommandSource->sendResponse(response);
}
