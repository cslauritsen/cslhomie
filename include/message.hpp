#pragma once
#include "all.hpp"

namespace homie {

class Message {
public:
  std::string topic;
  std::string payload;
  int qos;
  bool retained;
  Message(std::string topic, std::string payload, bool retained = true,
          int qos = 1);
};

} // namespace homie