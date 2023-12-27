#pragma once
#define RELAY_PIN 15
#include <MicroTasks.h>
#include <evse_state.h>

class RelayManager : public MicroTasks::Task
{ // The class
private :
    MicroTasks::EventListener _onRelayChanged;
    class RelayChanged : public MicroTasks::Event
    {
      private:
      public:
        RelayChanged();
        void fire();
    };

protected:
  static RelayManager *mManager;
  bool relayState = false;
  int relayOnTime = 0;
  int relayOffTime = 0;
  int relayToggleTime = 0;
  RelayChanged __changed ;
protected:
  void setup();
  unsigned long loop(MicroTasks::WakeReason reason);
  void assertClaimsIfChanged();

private:
  void relayOn();
  void relayOff();
  void resetRelay();

public:                     // Access specifier
RelayManager();
  void begin();
  static void setupRelay(); // Method/function declaration
  static RelayManager *getInstance();

  void setRelay(bool onoff, int toggleAfter);
  bool getRelay() { return relayState; };
  void loopRelay();
  long getRelayUpTime();
};