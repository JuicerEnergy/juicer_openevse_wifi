#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <commandsource.h>
#include <juicer_constants.h>

class RPCCommand
{
protected:
    int mCommandID = 0;
    int mCommandSrc = COMMAND_SOURCE_BLE;
    unsigned long mCommandTime = 0;
    int mCommand = CMD_NONE;
    char* mCommandName = NULL;
    DynamicJsonDocument *mpCommandJSON = NULL ;
    CommandSource *mpCommandSource;
    char response[256];
public:
    ~RPCCommand();
    static RPCCommand *getCommandHandler(char *scmd, int source, CommandSource *pCmdSource);
    void setJSON(DynamicJsonDocument *pDoc){mpCommandJSON = pDoc;}
    virtual void executeCommand() = 0;
    void replaceQuotes(char *str);
    void returnBlankResult();
};
