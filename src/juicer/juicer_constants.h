#ifndef _JUICER_CONSTANTS_H
#define _JUICER_CONSTANTS_H

#define DEVICE_ID_PREFIX "JUCR-"

#define JUICER_MACID GlobalState::DeviceID
#define JUICER_MACADDRESS GlobalState::DeviceMAC
#define JUICER_SERVICE "5f6d4f53-5f52-5043-5f53-56435f49445f"
#define JUICER_CHARACTERISTIC_W "5f6d4f53-5f52-5043-5f74-785f63746c5f"
#define JUICER_CHARACTERISTIC_RW "5f6d4f53-5f52-5043-5f64-6174615f5f5f"
#define JUICER_CHARACTERISTIC_NOTIFY "5f6d4f53-5f52-5043-5f72-785f63746c5f"

#define COMMAND_SOURCE_BLE 1
#define COMMAND_SOURCE_WS 2
#define COMMAND_SOURCE_INTERNAL 3

#define CMD_NONE            0
#define CMD_SYS_GETINFO     1

#define LOW_CURRENT_THRESHOLD 0.5 // low power if draw is less than 10 milli amps
#define POWER_DRAW_START_THRESHOLD 1.0
#define MS_PER_HOUR (60*60*1000) 

#define INTERRUPT_GRACE_MS (20*1000) // grace period for interruption past the initial power draw
#define PLUGIN_GRACE_MS (60*1000) // grace period for initial plug in

#define PROP_DEVICE_NAME "devicename"
#define PROP_SERVICE_LEVEL "level"
#define PROP_VOLTAGE "voltage"
#define PROP_MAX_AMPS "maxamps"
#define PROP_WEB_SERVER "webserver"
#endif
