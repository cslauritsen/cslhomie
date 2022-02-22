#include "homie.hpp"
void homie::Device::inquireNetConfig() {
  //    this->localIpProp->setValue("bugger-off");
  //   this->macProp->setValue(formatMac("aabbccddee"));
}

void homie::Device::subscribe(std::string commandTopic) {
  // normally impl logic to subscribe to "${homieBaseTopic}/+/+/+/set"
  // but only if there are settable properties
  // e.g. mgos_mqtt_sub(commandTopic.c_str(), this->subscriptionHandler, this);
  std::cerr << "NO sub logic needed " << commandTopic << std::endl;
}

void homie::Device::publish(Message m) {
  std::cout << "Publish topic=" << m.topic << ": " << m.payload
            << " q=" << m.qos << " r=" << m.retained << std::endl;
}

int homie::Device::getRssi() { return -58; }