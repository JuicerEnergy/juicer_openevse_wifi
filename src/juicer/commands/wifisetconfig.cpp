#include <Arduino.h>
#include <rpccommand.h>
#include <logging.h>
#include <allcommands.h>
#include <app_config.h>
#include <net_manager.h>

void WifiSetConfigCmd::executeCommand()
{

    logLineLevel(10, "executing %s", mCommandName);
    if (mpCommandJSON && mpCommandJSON->containsKey("params") && ((*mpCommandJSON)["params"]).containsKey("config"))
    {
        bool bEnabled = true;
        bool bAPEnabled = false;
        const char *ssid = NULL;
        const char *pass = NULL;
        const char *appass = NULL;
        if (((*mpCommandJSON)["params"]["config"]).containsKey("sta") && (*mpCommandJSON)["params"]["config"]["sta"].containsKey("enable"))
        {
            bEnabled = (*mpCommandJSON)["params"]["config"]["sta"]["enable"];
            if ((*mpCommandJSON)["params"]["config"]["sta"].containsKey("ssid")){
                ssid = (*mpCommandJSON)["params"]["config"]["sta"]["ssid"];
                config_set("ssid", bEnabled ? String(ssid) : String(""));
            }
            if ((*mpCommandJSON)["params"]["config"]["sta"].containsKey("pass")){
                pass = (*mpCommandJSON)["params"]["config"]["sta"]["pass"];
                config_set("pass", bEnabled ? String(pass) : String(""));
            }
            if (!bEnabled){
                config_set("ssid", String(""));
                config_set("pass", String(""));
            }
        }

        if (((*mpCommandJSON)["params"]["config"]).containsKey("ap") && (*mpCommandJSON)["params"]["config"]["ap"].containsKey("enable"))
        {
            bAPEnabled = (*mpCommandJSON)["params"]["config"]["ap"]["enable"];
            if ((*mpCommandJSON)["params"]["config"]["ap"].containsKey("pass")){
                appass = (*mpCommandJSON)["params"]["config"]["ap"]["pass"];
            }
            config_set("ap_enabled", bAPEnabled);
            if (appass && strlen(appass) > 0)
            {
                logLineLevel(10, "Setting ap_pass to %s", appass);
                config_set("ap_pass", String(appass));
            }
        }

        config_commit();
        net.wifiRestart();
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
