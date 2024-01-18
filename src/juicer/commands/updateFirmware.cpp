#include <Arduino.h>
#include <rpccommand.h>
#include <logging.h>
#include <allcommands.h>
#include <app_config.h>
#include <net_manager.h>
#include <http_update.h>
#include <updatemanager.h>

void UpdateFirmwareCommand::executeCommand()
{
    logLineLevel(10, "executing %s, 1", mCommandName);
    DynamicJsonDocument doc(100);
    doc["src"] = JUICER_MACID;
    JsonObject result = doc.createNestedObject("result");

    if (mpCommandJSON && mpCommandJSON->containsKey("params") && ((*mpCommandJSON)["params"]).containsKey("url"))
    {
        const char *url = (*mpCommandJSON)["params"]["url"];
        logLineLevel(10, "executing %s from url %s", mCommandName, url);
        if (!UpdateManager::getInstance()->canUpdate())
        {
            result["status"] = "INPROGRESS";
        }else if (!UpdateManager::getInstance()->startUpdate(url)){
            result["status"] = "FAILEDTOSTART";
        }else{
            result["status"] = "OK";
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