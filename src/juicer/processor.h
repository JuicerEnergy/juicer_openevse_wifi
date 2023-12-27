#include <commandsource.h>

class CommandProcessor
{
protected:
    static CommandProcessor* mProcessor ;
private:
public:                           // Access specifier
    static void setupProcessor(); // Method/function declaration
    static CommandProcessor *getInstance();
    void loopProcessor();

    void onCommandReceived(CommandSource *pCmdSource, char* sCommand, int src);
};