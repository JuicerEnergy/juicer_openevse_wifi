#include <Arduino.h>
#include <rpccommand.h>
#include <logging.h>
#include <allcommands.h>

void SysGetConfigCmd::executeCommand(){
   logLineLevel(10, "executing %s", mCommandName);
    char response[500];

    DynamicJsonDocument doc(500);
    doc["src"] = JUICER_MACID;

    JsonObject result = doc.createNestedObject("result");
    JsonObject device = result.createNestedObject("device");

    if (GlobalState::getInstance()->getPropertyLong(PROP_DEVICE_NAME)){
        device["name"] = GlobalState::getInstance()->getPropertyLong(PROP_DEVICE_NAME);
    }
    device["mac"] = JUICER_MACADDRESS;
    device["ver"] = STR(JUICER_VERSION);
    JsonObject juicer = device.createNestedObject("juicer");
    juicer["level"] = GlobalState::getInstance()->getPropertyLong(PROP_SERVICE_LEVEL);
    juicer["voltage"] = GlobalState::getInstance()->getPropertyLong(PROP_VOLTAGE);
    juicer["maxamps"] = GlobalState::getInstance()->getPropertyLong(PROP_MAX_AMPS);
    juicer["enableweb"] = GlobalState::getInstance()->getPropertyLong(PROP_WEB_SERVER);

    serializeJson(doc, response);
    if (!mpCommandSource){
        logLine("No command source !");
    }else{
        mpCommandSource->sendResponse(response);
    }    
}
