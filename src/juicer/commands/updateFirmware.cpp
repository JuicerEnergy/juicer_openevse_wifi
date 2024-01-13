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
    logLineLevel(10, "executing %s", mCommandName);
    if (mpCommandJSON && mpCommandJSON->containsKey("params") && ((*mpCommandJSON)["params"]).containsKey("url"))
    {
        UpdateFirmwareCommand *pThis = this;
        if (http_update_from_url((*mpCommandJSON)["params"]["url"], [pThis](size_t complete, size_t total)
                                 { pThis->sendUpdateProgress(complete, total); },
                                 [&](int)
                                 { pThis->sendUpdateResult("OK"); },
                                 [&](int)
                                 { pThis->sendUpdateResult("ERROR"); }))
        {
            status = "OK";
        }
    }
}

void UpdateFirmwareCommand::sendUpdateProgress(size_t complete, size_t total)
{
    logLine("Updating firmware %ld of %ld", complete, total);
    const size_t capacity =
        JSON_ARRAY_SIZE(0) + 2 * JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(4);
    DynamicJsonDocument doc(capacity);
    doc["src"] = JUICER_MACID;

    JsonObject result = doc.createNestedObject("result");
    result["complete"] = complete;
    result["total"] = total;
    char buff[512];
    serializeJson(doc, buff);
    mpCommandSource->sendResponse(buff);
}

void UpdateFirmwareCommand::sendUpdateResult(const char *status)
{
    logLine("Updating firmware Status %s", status);
    const size_t capacity =
        JSON_ARRAY_SIZE(0) + 2 * JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(4);
    DynamicJsonDocument doc(capacity);
    doc["src"] = JUICER_MACID;

    JsonObject result = doc.createNestedObject("result");
    result["status"] = status;
    if (mpCommandJSON && mpCommandJSON->containsKey("id"))
    {
        doc["id"] = (*mpCommandJSON)["id"];
    }

    char buff[512];
    serializeJson(doc, buff);
    mpCommandSource->sendResponse(buff);
}
