#include <Arduino.h>
#include <rpccommand.h>
#include <logging.h>
#include <allcommands.h>
#include <app_config.h>
#include <net_manager.h>
#include <http_update.h>

void UpdateFirmwareCommand::executeCommand()
{
    const char *status = "FAIL";
    logLineLevel(10, "executing %s, 1", mCommandName);
    if (mpCommandJSON && mpCommandJSON->containsKey("params") && ((*mpCommandJSON)["params"]).containsKey("url"))
    {
        const char* url = (*mpCommandJSON)["params"]["url"];
        logLineLevel(10, "executing %s from url %s", mCommandName, url);
        UpdateFirmwareCommand *pThis = this;
        if (http_update_from_url(url, [pThis](size_t complete, size_t total)
                                 { /*logLine("Updating firmware %ld of %ld", complete, total);*//*pThis->sendUpdateProgress(complete, total);*/ },
                                 [pThis](int)
                                 { /*pThis->sendUpdateResult("OK");*/ },
                                 [pThis](int)
                                 { /*pThis->sendUpdateResult("ERROR");*/ }))
        {
            status = "OK";
        }
    }
}

void UpdateFirmwareCommand::sendUpdateProgress(size_t complete, size_t total)
{
    logLine("Updating firmware %ld of %ld", complete, total);
    DynamicJsonDocument doc(100);
    doc["src"] = JUICER_MACID;

    JsonObject result = doc.createNestedObject("result");
    result["complete"] = complete;
    result["total"] = total;
    serializeJson(doc, response);
    // if (!mpCommandSource){
    //     logLine("No command source !");
    // }else{
    //     mpCommandSource->sendResponse(response);
    //     logLine("sent..");
    // }
}

void UpdateFirmwareCommand::sendUpdateResult(const char *status)
{
    logLine("Updating firmware Status %s", status);
    DynamicJsonDocument *pdoc = new DynamicJsonDocument(100);
    logLine("set macid");
    (*pdoc)["src"] = JUICER_MACID;

    logLine("set result");
    JsonObject result = pdoc->createNestedObject("result");
    logLine("set status");
    result["status"] = status;
    logLine("serializing");
    serializeJson((*pdoc), response);
    logLine("sending..");
    // if (!mpCommandSource){
    //     logLine("No command source !");
    // }else{
    //     mpCommandSource->sendResponse(response);
    //     logLine("sent..");
    // }
    delete pdoc;
}
