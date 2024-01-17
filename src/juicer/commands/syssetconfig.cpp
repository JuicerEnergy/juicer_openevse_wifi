#include <Arduino.h>
#include <rpccommand.h>
#include <logging.h>
#include <allcommands.h>
#include <juicer_constants.h>
#include <evse_man.h>
#include <evse_monitor.h>
extern EvseManager evse;

void SysSetConfigCmd::executeCommand()
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
        if (mpCommandJSON->containsKey("params") && ((*mpCommandJSON)["params"]).containsKey("config") && ((*mpCommandJSON)["params"]["config"]).containsKey("device"))
        {
            if (((*mpCommandJSON)["params"]["config"]["device"]).containsKey("name"))
            {
                String name = (*mpCommandJSON)["params"]["config"]["device"]["name"];
                GlobalState::getInstance()->setPropertyStr(PROP_DEVICE_NAME, name);
            }

            if (((*mpCommandJSON)["params"]["config"]["device"]).containsKey("juicer"))
            {
                if (((*mpCommandJSON)["params"]["config"]["device"]["juicer"]).containsKey("level")){
                    long level = (*mpCommandJSON)["params"]["config"]["device"]["juicer"]["level"];
                    if (level == 1 || level == 2 ){
                        GlobalState::getInstance()->setPropertyLong(PROP_SERVICE_LEVEL, level);
                        GlobalState::getInstance()->setPropertyLong(PROP_VOLTAGE, level == 1 ? 120 : 240);
                    }
                }
                if (((*mpCommandJSON)["params"]["config"]["device"]["juicer"]).containsKey("maxamps")){
                    long maxamps = (*mpCommandJSON)["params"]["config"]["device"]["juicer"]["maxamps"];
                    long level = GlobalState::getInstance()->getPropertyLong(PROP_SERVICE_LEVEL);
                    if (level == 1 && maxamps >=6 && maxamps <= 12 ){
                        GlobalState::getInstance()->setPropertyLong(PROP_MAX_AMPS, maxamps);
                    }
                    if (level == 2 && maxamps >=6 && maxamps <= 40 ){
                        GlobalState::getInstance()->setPropertyLong(PROP_MAX_AMPS, maxamps);
                    }
                }
                if (((*mpCommandJSON)["params"]["config"]["device"]["juicer"]).containsKey("webserver")){
                    GlobalState::getInstance()->setPropertyLong(PROP_WEB_SERVER, (*mpCommandJSON)["params"]["config"]["device"]["juicer"]["webserver"]);
                }

                evse.setServiceLevel((EvseMonitor::ServiceLevel)GlobalState::getInstance()->getPropertyLong(PROP_SERVICE_LEVEL));
                evse.setVoltage(GlobalState::getInstance()->getPropertyLong(PROP_VOLTAGE));
                evse.setMaxConfiguredCurrent(GlobalState::getInstance()->getPropertyLong(PROP_MAX_AMPS));
            }
        }
        sprintf(response, responseFmt, JUICER_MACID);
    }
    replaceQuotes(response);
    mpCommandSource->sendResponse(response);
}
