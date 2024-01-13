
#include <Arduino.h>
#include <processor.h>
#include <logging.h>
#include <rpccommand.h>
// #include <Crypto.h>
// #include <AES.h>

/** static singleton */
CommandProcessor* CommandProcessor::mProcessor = NULL ;

/**
 * Called on startup
*/
void CommandProcessor::setupProcessor(){
    if (!CommandProcessor::mProcessor){
        CommandProcessor::mProcessor = new CommandProcessor();
    }
}

CommandProcessor *CommandProcessor::getInstance(){
    return CommandProcessor::mProcessor;
}


void CommandProcessor::loopProcessor()
{
}

void CommandProcessor::onCommandReceived(CommandSource *pCmdSource, char* sCommand, int src){
    logLineLevel(10, "Command received : %s", sCommand);
    RPCCommand *pcmd = RPCCommand::getCommandHandler(sCommand, src, pCmdSource);
    if (!pcmd){
        logLine("Command not recognized:", 0);
        logLine(sCommand, 0);
        return ;
    }
    pcmd->executeCommand();
    delete pcmd;
}
