#include <Arduino.h>
#include <rpccommand.h>
#include <logging.h>
#include <allcommands.h>

void ShellyGetConfigCmd::executeCommand(){
   logLineLevel(10, "executing %s", mCommandName);
    char response[500];
    const char *responseFmt = "{'src':'%s','result':{'ver':'%s'}}";
    sprintf(response, responseFmt, JUICER_MACID, STR(JUICER_VERSION));
    for (int i = 0; i < strlen(response); i++)
    {
        if (response[i] == '\'') response[i] = '\"';
    }
    mpCommandSource->sendResponse(response);
}
