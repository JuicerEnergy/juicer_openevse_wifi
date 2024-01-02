#include <Arduino.h>
#include <rpccommand.h>
#include <logging.h>
#include <allcommands.h>
#include <relaymanager.h>
#include <powermanager.h>


void juicerOnOff(bool on, long toggleAfter);

void SwitchSetCmd::executeCommand()
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
        if (mpCommandJSON->containsKey("params") && (*mpCommandJSON)["params"].containsKey("on"))
        {
            bool switchOn = (*mpCommandJSON)["params"]["on"];
            long offAfter = 0 ;
            if ((*mpCommandJSON)["params"].containsKey("toggle_after")){
                offAfter = (*mpCommandJSON)["params"]["toggle_after"];
            }
            juicerOnOff(switchOn, offAfter);
        }
        sprintf(response, responseFmt, JUICER_MACID);
    }
    replaceQuotes(response);
    mpCommandSource->sendResponse(response);
}

void juicerOnOff(bool on, long toggleAfter){
    logLine("Turning Switch... %d -> %ld", on, toggleAfter);
    RelayManager::getInstance()->setRelay(on, toggleAfter);
    if (on){
        PowerManager::getInstance()->startSession();    // start power management session
    }
}
