#include <Arduino.h>
#include <rpccommand.h>
#include <logging.h>
#include <allcommands.h>

RPCCommand::~RPCCommand()
{
    if (mpCommandJSON)
    {
        delete(mpCommandJSON);
        mpCommandJSON = NULL;
    }
    if (mCommandName)
    {
        free(mCommandName);
        mCommandName = NULL;
    }
}

// void RPCCommand::setJSON(DynamicJsonDocument doc){
//     mDoc = doc ;
// }

RPCCommand *RPCCommand::getCommandHandler(char *scmd, int source, CommandSource *pCmdSource)
{
    RPCCommand *pCmd = NULL;

    // first parse the command
    DynamicJsonDocument *pdoc = new DynamicJsonDocument(1024);
    deserializeJson(*pdoc, scmd);
    const char *command = (*pdoc)["method"];
    if (!strcasecmp(command, "Sys.GetConfig"))
    {
        pCmd = new SysGetConfigCmd();
    }
    else if (!strcasecmp(command, "Sys.SetConfig"))
    {
        pCmd = new SysSetConfigCmd();
    }
    else if (!strcasecmp(command, "Shelly.GetDeviceInfo"))
    {
        pCmd = new ShellyGetConfigCmd();
    }
    else if (!strcasecmp(command, "Wifi.GetConfig"))
    {
        pCmd = new WifiGetConfigCmd();
    }
    else if (!strcasecmp(command, "Wifi.GetStatus"))
    {
        pCmd = new WifiGetStatusCmd();
    }
    else if (!strcasecmp(command, "Wifi.SetConfig"))
    {
        pCmd = new WifiSetConfigCmd();
    }
    else if (!strcasecmp(command, "KVS.Get"))
    {
        pCmd = new SysGetKVSCmd();
    }
    else if (!strcasecmp(command, "KVS.Set"))
    {
        pCmd = new SysSetKVSCmd();
    }
    else if (!strcasecmp(command, "Switch.GetStatus"))
    {
        pCmd = new SwitchGetCmd();
    }
    else if (!strcasecmp(command, "Switch.Set"))
    {
        pCmd = new SwitchSetCmd();
    }
    else{
        pCmd = new Unimplemented();
    }

    if (pCmd)
    {
        pCmd->mCommandName = strdup(command);
        pCmd->mCommandSrc = source;
        pCmd->mpCommandSource = pCmdSource; // who sent the command
        pCmd->setJSON(pdoc);
    }
    return pCmd;
}

/**
 * return a result which is blank
 */
void RPCCommand::returnBlankResult()
{
    const char *responseFmt = "{'src':'%s','result':{'restart_required': false}}";
    char response[500];
    sprintf(response, responseFmt, JUICER_MACID);
    replaceQuotes(response);
    mpCommandSource->sendResponse(response);
}

void RPCCommand::replaceQuotes(char *str)
{
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] == '\'')
            str[i] = '\"';
    }
}
