#include <Arduino.h>
#include <rpccommand.h>
#include <logging.h>
#include <allcommands.h>
#include <app_config.h>

void WifiSetConfigCmd::executeCommand()
{

    logLineLevel(10, "executing %s", mCommandName);
    if (mpCommandJSON && mpCommandJSON->containsKey("params") && ((*mpCommandJSON)["params"]).containsKey("config") && ((*mpCommandJSON)["params"]["config"]).containsKey("sta")){
        bool bEnabled = true ;
        const char* ssid = NULL;
        const char* pass = NULL;
        if ((*mpCommandJSON)["params"]["config"]["sta"].containsKey("enabled")){
            bEnabled = (*mpCommandJSON)["params"]["config"]["sta"]["enabled"];
        }
        ssid = (*mpCommandJSON)["params"]["config"]["sta"]["ssid"];
        pass = (*mpCommandJSON)["params"]["config"]["sta"]["pass"];
        config_set("esid", ssid);
        config_set("pass", pass);
    }
    const size_t capacity =
        JSON_ARRAY_SIZE(0) + 2 * JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(4);
    DynamicJsonDocument doc(capacity);
    doc["src"] = JUICER_MACID;

    JsonObject result = doc.createNestedObject("result");
    JsonObject sta = result.createNestedObject("sta");
    sta["ssid"] = JUICER_MACID;
    JsonObject ap = result.createNestedObject("ap");
    ap["ssid"] = JUICER_MACID;
    char buff[512];
    serializeJson(doc, buff);
    mpCommandSource->sendResponse(buff);
}

