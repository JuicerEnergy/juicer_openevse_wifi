#include <Arduino.h>
#include <rpccommand.h>
#include <logging.h>
#include <allcommands.h>

void SysGetConfigCmd::executeCommand(){
   logLineLevel(10, "executing %s", mCommandName);
    char response[500];
    const char *responseFmt = "{'src':'%s','result':{'device':{'mac':'%s','ver':'1.0'}}}";
    sprintf(response, responseFmt, JUICER_MACID, JUICER_MACADDRESS);
    for (int i = 0; i < strlen(response); i++)
    {
        if (response[i] == '\'') response[i] = '\"';
    }
    mpCommandSource->sendResponse(response);
}
