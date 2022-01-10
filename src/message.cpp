#include "homie.hpp"
namespace homie {
Message::Message(std::string topic, std::string payload, bool retained,
                 int qos) {
  this->topic = topic;
  this->payload = payload;
  this->retained = retained;
  this->qos = qos;
}
} // namespace homie
