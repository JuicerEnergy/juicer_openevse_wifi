#pragma once
#include <MicroTasks.h>
#include <evse_state.h>

class PowerManager: public MicroTasks::Task
{ // The class
private:
 MicroTasks::EventListener _onPowerChanged;
    class PowerChanged : public MicroTasks::Event
    {
      private:
      public:
        PowerChanged();
        void fire();
    };
protected:
  static PowerManager *mManager;
  PowerChanged __changed ;
  bool bInitialized = false ;
  bool bPowerDrawStarted = false ;
  long lastCheckTime = 0;
  float lastCurrent = 0.0;
  float lastPower = 0.0;
  float totalEnergy = 0.0;
  float sessionEnergyUsed = 0.0;
  float startingEnergyReading = -1.0;
  long powerDownDetectedTime = 0 ;

protected:
  void setup();
  unsigned long loop(MicroTasks::WakeReason reason);

private:
  void initialize();
public:                     // Access specifier
  PowerManager();
  void begin();
  static void setupPM(); // Method/function declaration
  static PowerManager *getInstance();

  bool loopPM();
  void startSession();
  void doAutoShutoff();
  long getTimeMSSinceLastCheck();
  float getLastPower(){return lastPower;}
  void setTotalEnergy(float energy){if (startingEnergyReading < 0.0) startingEnergyReading = energy; totalEnergy = energy; sessionEnergyUsed = energy - startingEnergyReading;}
  void setPower(float pow){lastPower = pow ;}
  void setCurrent(float curr){lastCurrent = curr ;}

  float getCurrent(){return lastCurrent;}
  float getTotalEnergy(){return totalEnergy;}
};