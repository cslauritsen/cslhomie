#include "homie.hpp"

namespace homie {

Device::Device(std::string aid, std::string aVersion, std::string aname,
               std::string homieTopicBase) {
  id = aid;
  this->homieTopicBase = homieTopicBase;

  // homie (or mqtt?) says that topic elements must be lower-case
  // do this incase the configurator doesnt know this
  std::transform(this->id.begin(), this->id.end(), this->id.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  this->version = aVersion;
  name = aname;
  this->topicBase = this->homieTopicBase;
  this->topicBase += "/" + id + "/";
  extensions.push_back(std::string("org.homie.legacy-firmware:0.1.1:[4.x]"));
  lifecycleState = INIT;

  this->wifiNode = new Node(this, NODE_NM_WIFI, "WiFi", "WIFI");
  this->rssiProp =
      new Property(this->wifiNode, PROP_NM_RSSI, "RSSI", homie::INTEGER, false);
  this->rssiProp->valueFunction = [this]() {
    return to_string(this->getRssi());
  };
  this->wifiSignalProp = new Property(this->wifiNode, PROP_NM_WIFI_SIGNAL,
                                      "Wifi Signal", homie::INTEGER, false);
  this->wifiSignalProp->valueFunction = [this]() {
    return to_string(this->getWifiSignalStrength());
  };
  this->localIpProp =
      new Property(this->wifiNode, "localip", "Local IP", homie::STRING, false);
  this->localIpProp->valueFunction = [this]() { return this->localIp; };
  this->macProp =
      new Property(this->wifiNode, "mac", "MAC Address", homie::STRING, false);
  this->macProp->valueFunction = [this]() { return formatMac(this->mac); };
}

void Device::publishWifi() {
  this->rssiProp->publish();
  this->wifiSignalProp->publish();
}

void Device::addNode(Node *n) { nodes[n->getId()] = n; }

Node *Device::getNode(std::string nm) {
  auto search = nodes.find(nm);
  if (search == nodes.end()) {
    return nullptr;
  }
  return search->second;
}
void Device::introduce() {
  int i;
  this->publish(Message(topicBase + "$homie", HOMIE_VERSION));
  this->publish(Message(topicBase + "$name", name));
  auto impl = std::string("cslhomie");
  impl += "-" + homie::LIB_VERSION;
  this->publish(Message(topicBase + "$implementation", impl));
  this->setLifecycleState(homie::INIT);
  this->publish(getLifecycleMsg());

  std::string exts("");
  i = 0;
  for (auto elm : extensions) {
    if (i++ > 0) {
      exts += ",";
    }
    exts += elm;
  }
  this->publish(Message(topicBase + "$extensions", exts));

  this->publish(
      Message(topicBase + "$localip", this->localIpProp->valueFunction()));
  this->publish(Message(topicBase + "$mac", this->macProp->valueFunction()));
  this->publish(Message(topicBase + "$fw/name", id + "-firmware"));
  this->publish(Message(topicBase + "$fw/version", version));

  std::string nodeList("");
  i = 0;
  for (auto elm : nodes) {
    if (i++ > 0) {
      nodeList += ",";
    }
    nodeList += elm.first;
  }
  this->publish(Message(topicBase + "$nodes", nodeList));

  for (auto e : nodes) {
    e.second->introduce();
  }
  this->setLifecycleState(homie::READY);
  this->publish(getLifecycleMsg());
}

int Device::getWifiSignalStrength() {
  auto rssi = getRssi();
  if (rssi <= -100)
    return 0;
  if (rssi >= -50)
    return 100;
  return 2 * (rssi + 100);
}

Message Device::getLwt() {
  return Message(getLifecycleTopic(), LIFECYCLE_STATES[(int)homie::LOST]);
}

std::string Device::getLifecycleTopic() { return topicBase + "$state"; }

Message Device::getLifecycleMsg() {
  return Message(getLifecycleTopic(), LIFECYCLE_STATES[(int)lifecycleState]);
}

} // namespace homie
