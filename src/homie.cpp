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

void split_string(std::string s, std::string delimiter,
                  std::vector<std::string> &res) {

  // for string delimiter
  size_t pos_start = 0, pos_end, delim_len = delimiter.length();
  std::string token;

  while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
    token = s.substr(pos_start, pos_end - pos_start);
    pos_start = pos_end + delim_len;
    res.push_back(token);
  }
  res.push_back(s.substr(pos_start));
}

float to_fahrenheit(float celsius) {
  celsius *= 1.8f;
  celsius += 32.0f;
  return celsius;
}

std::string f2s(float f) {
  char buf[10];
  sprintf(buf, "%7.1f", f);
  return std::string(buf);
}
} // namespace homie
