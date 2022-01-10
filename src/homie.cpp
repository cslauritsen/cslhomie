#include "homie.hpp"

namespace homie {

const std::string DEGREE_SYMBOL = "°";
const std::string HOMIE_VERSION = "4.0";

const std::string NODE_NM_WIFI = "wifi";
const std::string PROP_NM_RSSI = "rssi";
const std::string PROP_NM_WIFI_SIGNAL = "signal";

std::string DATA_TYPES[] = {"integer", "string", "float",    "percent",
                            "boolean", "enum",   "dateTime", "duration"};

std::string LIFECYCLE_STATES[] = {"init",     "ready", "disconnected",
                                  "sleeping", "lost",  "alert"};

std::string formatMac(std::string str) {
  std::string tmp;
  // remove all colons  (if any)
  str.erase(std::remove(str.begin(), str.end(), ':'), str.end());
  // put them back
  for (std::string::size_type i = 0; i < str.length(); i += 2) {
    if (i > 0) {
      tmp += ':';
    }
    tmp += str[i];
    tmp += str[i + 1];
  }
  return tmp;
}

std::string to_string(bool v) { return std::string(v ? "true" : "false"); }


float to_fahrenheit(float celsius) {
  celsius *= 1.8f;
  celsius += 32.0f;
  return celsius;
}
} // namespace homie
