#pragma once
#include <string>

namespace homie {

//extern const std::string LIB_VERSION;

extern const std::string DEGREE_SYMBOL;
extern const std::string HOMIE_VERSION;

extern const std::string NODE_NM_WIFI;
extern const std::string PROP_NM_RSSI;
extern const std::string PROP_NM_WIFI_SIGNAL;

extern std::string DATA_TYPES[];
extern std::string LIFECYCLE_STATES[];

enum DataType {
  INTEGER = 0,
  STRING,
  FLOAT,
  PERCENT,
  BOOLEAN,
  ENUM,
  COLOR,
  DATETIME,
  DURATION
};

enum LifecycleState { INIT = 0, READY, DISCONNECTED, SLEEPING, LOST, ALERT };
} // namespace homie