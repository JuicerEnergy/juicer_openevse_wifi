
#ifndef _JUICER_COMMANDSOURCE_H
#define _JUICER_COMMANDSOURCE_H

class CommandSource
{
public:                      
    virtual void sendResponse(const char* response) = 0;
};
#endif