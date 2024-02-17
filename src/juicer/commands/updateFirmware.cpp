#include <Arduino.h>
#include <rpccommand.h>
#include <logging.h>
#include <allcommands.h>
#include <app_config.h>
#include <net_manager.h>
#include <http_update.h>
#include <updatemanager.h>
extern EvseManager evse;

void UpdateFirmwareCommand::executeCommand()
{
    logLineLevel(10, "executing %s, 1", mCommandName);
    DynamicJsonDocument doc(100);
    doc["src"] = JUICER_MACID;
    JsonObject result = doc.createNestedObject("result");
    if (mpCommandJSON && mpCommandJSON->containsKey("params") && ((*mpCommandJSON)["params"]).containsKey("url"))
    {
        const char *url = (*mpCommandJSON)["params"]["url"];
        logLineLevel(10, "executing %s from url %s , state : %d", mCommandName, url, evse.getEvseState());
        if (evse.getEvseState() == OPENEVSE_STATE_CHARGING){
            result["status"] = "NOT_DISABLED";
            result["statusdesc"] = "EVSE is active";
        }else if (!UpdateManager::getInstance()->canUpdate())
        {
            result["status"] = "INPROGRESS";
            result["statusdesc"] = "An update is in progress";
        }else{
            GlobalState::getInstance()->setPropertyStr(PROP_FIRMWARE_UPDATE_URL, String(url));
            logLineLevel(10, "Update URL set to %s", GlobalState::getInstance()->getPropertyStr(PROP_FIRMWARE_UPDATE_URL));
            result["status"] = "OK";
            result["statusdesc"] = "Started";
            serializeJson(doc, response);
            mpCommandSource->sendResponse(response);
            logLineLevel(10, "Restarting system to begin update");
            restart_system();
            return ;
        }
    }else{ // we just want status
        const char * stat = "Unknown" ;
        switch(UpdateManager::getInstance()->getUpdateStatus()){
            case UpdateManager::UpdateStatus::Failed : stat = "Failed";break;
            case UpdateManager::UpdateStatus::NotStarted : stat = "Not Started";break;
            case UpdateManager::UpdateStatus::InProgress : stat = "In Progress";break;
            case UpdateManager::UpdateStatus::Success : stat = "Success";break;
        }
        result["status"] = stat  ;
        result["progress"] = UpdateManager::getInstance()->getUpdateProgress() ;
    }

    serializeJson(doc, response);
    mpCommandSource->sendResponse(response);
}