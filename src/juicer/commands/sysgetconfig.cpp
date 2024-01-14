#include <Arduino.h>
#include <rpccommand.h>
#include <logging.h>
#include <allcommands.h>

void SysGetConfigCmd::executeCommand(){
   logLineLevel(10, "executing %s", mCommandName);
    char response[500];

    DynamicJsonDocument doc(100);
    doc["src"] = JUICER_MACID;

    JsonObject result = doc.createNestedObject("result");
    JsonObject device = result.createNestedObject("device");
    JsonObject juicer = device.createNestedObject("juicer");

    device["mac"] = JUICER_MACADDRESS;
    device["ver"] = JUICER_VERSION;
    juicer["level"] = GlobalState::getInstance()->getPropertyLong("service");
    juicer["voltage"] = GlobalState::getInstance()->getPropertyLong("voltage");
    juicer["maxamps"] = GlobalState::getInstance()->getPropertyLong("maxamps");

    serializeJson(doc, response);
    if (!mpCommandSource){
        logLine("No command source !");
    }else{
        mpCommandSource->sendResponse(response);
    }    
}
