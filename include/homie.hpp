#pragma once
#include "all.hpp"
#include "device.hpp"
#include "enum.hpp"
#include "message.hpp"
#include "node.hpp"
#include "property.hpp"
#include <vector>

namespace homie {

std::string to_string(bool v);

template <typename T> std::string to_string(const T &t) {
  std::ostringstream stm;
  stm << t;
  return stm.str();
}

std::string f2s(float f);

std::string formatMac(std::string mac);

float to_fahrenheit(float celsius);

void split_string(std::string s, std::string delimiter,
                  std::vector<std::string> &results);
} // namespace homie
