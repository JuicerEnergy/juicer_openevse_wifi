#pragma once
#include <rpccommand.h>
#include <globalstate.h>
#include <juicer_constants.h>

class Unimplemented : public RPCCommand
{
    void executeCommand();
};
class SwitchGetCmd : public RPCCommand
{
    void executeCommand();
};

class SwitchSetCmd : public RPCCommand
{
    void executeCommand();
};
class SysGetKVSCmd : public RPCCommand
{
    void executeCommand();
};

class SysSetKVSCmd : public RPCCommand
{
    void executeCommand();
};

class SysGetConfigCmd : public RPCCommand
{
    void executeCommand();
};

class SysSetConfigCmd: public RPCCommand
{
    void executeCommand();
};

class WifiGetConfigCmd: public RPCCommand
{
    void executeCommand();
};

class WifiGetStatusCmd: public RPCCommand
{
    void executeCommand();
};

class WifiSetConfigCmd: public RPCCommand
{
    void executeCommand();
};

class ShellyGetConfigCmd: public RPCCommand
{
    void executeCommand();
};

class UpdateFirmwareCommand: public RPCCommand
{
    void executeCommand();
    void sendUpdateProgress(size_t complete, size_t total);
    void sendUpdateResult(const char* result);

};


class ResetCommand: public RPCCommand
{
    void executeCommand();
};
