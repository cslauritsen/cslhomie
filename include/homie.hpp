#pragma once

#include "all.hpp"
#include "device.hpp"
#include "enum.hpp"
#include "message.hpp"
#include "node.hpp"
#include "property.hpp"

namespace homie {

std::string to_string(bool v);

template <typename T> std::string to_string(const T &t) {
  std::ostringstream stm;
  stm << t;
  return stm.str();
}

std::string formatMac(std::string mac);

float to_fahrenheit(float celsius);
} // namespace homie
