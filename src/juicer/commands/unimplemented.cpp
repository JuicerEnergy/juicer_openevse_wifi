#include <Arduino.h>
#include <rpccommand.h>
#include <logging.h>
#include <allcommands.h>

void Unimplemented::executeCommand()
{
    logLineLevel(10, "executing unknown %s", mCommandName);
    returnBlankResult();
}
