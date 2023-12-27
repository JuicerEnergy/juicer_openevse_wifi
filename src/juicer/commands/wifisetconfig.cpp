#include <Arduino.h>
#include <rpccommand.h>
#include <logging.h>
#include <allcommands.h>

void WifiSetConfigCmd::executeCommand()
{
    logLineLevel(10, "executing %s", mCommandName);
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
