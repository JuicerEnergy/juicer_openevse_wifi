#include <Arduino.h>
#include <rpccommand.h>
#include <logging.h>
#include <allcommands.h>
#include <relaymanager.h>
#include <powermanager.h>

void SwitchGetCmd::executeCommand()
{
    logLineLevel(10, "executing %s", mCommandName);
    const char *responseErrFmt = "{'src':'%s'}";
    const char *responseFmt = "{'src':'%s','result':{'id': %d, 'output': %s, 'apower': %f}}";

    if (!mpCommandJSON || !(mpCommandJSON->containsKey("params") && ((*mpCommandJSON)["params"]).containsKey("id")))
    {
        sprintf(response, responseErrFmt, JUICER_MACID);
    }
    else
    {
        int switchID = (*mpCommandJSON)["params"]["id"];
        const char* output = RelayManager::getInstance()->getRelay() ? "true" : "false";
        sprintf(response, responseFmt, JUICER_MACID, switchID, output, PowerManager::getInstance()->getLastPower());
    }
    replaceQuotes(response);
    mpCommandSource->sendResponse(response);
}
