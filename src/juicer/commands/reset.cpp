#include <Arduino.h>
#include <rpccommand.h>
#include <logging.h>
#include <allcommands.h>
#include <app_config.h>
#include <net_manager.h>

void restart_system();

void ResetCommand::executeCommand()
{

    logLineLevel(10, "executing %s", mCommandName);
    returnBlankResult();
    restart_system();
}
