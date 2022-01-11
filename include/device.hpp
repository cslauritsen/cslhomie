#pragma once
#include "homie.hpp"
#include "enum.hpp"

namespace homie {
  class Node;
  class Property;
  class Message;

/**
 * @brief  Models a homie device. A homie device has 0 or many nodes, and  has
 * exactly one mqtt connection.
 *
 */
class Device {
protected:
  // required attributes:
  std::string id;
  std::string name;
  LifecycleState lifecycleState;
  std::map<std::string, Node *> nodes;
  std::vector<std::string> extensions;
  std::string version;
  std::string mac;
  std::string localIp;

  std::string topicBase;
  std::string homieTopicBase;

  Node *wifiNode;
  Property *rssiProp;
  Property *wifiSignalProp;
  Property *localIpProp;
  Property *macProp;
  std::vector<std::string> topicElements;

public:
  Device(std::string aid, std::string aVersion, std::string aname,
         std::string homieTopicBase = "homie");
  virtual ~Device();

  virtual void publish(Message);
  virtual void subscribe(std::string commandTopic);
  void onMessage(Message);

  void setLocalIp(std::string s) { this->localIp = s; }
  std::string getLocalIp() { return this->localIp; }

  void setMac(std::string s) { this->mac = s; }
  std::string getMac() { return this->mac; }

  std::string getTopicBase() { return topicBase; }
  void addNode(Node *n);
  Node *getNode(std::string nm);

  LifecycleState getLifecycleState() { return lifecycleState; }
  void setLifecycleState(LifecycleState lcs) { lifecycleState = lcs; }

  virtual int getRssi();
  int getWifiSignalStrength();

  void publishWifi();

  /**
   * @brief  Ask the OS for IP address and MAC address.
   * Set the mac and localIp properties when found.
   * This method can optionally do nothing.
   *
   */
  virtual void inquireNetConfig();

  /**
   * @brief Define device via homie convention.
   * This will generate a list of topic+value pairs that can
   * be published via the MQTT broker to introduce this device
   * to services that grok the homie convention.
   *
   * The list of topics is like so:
   * <pre>
   *
homie / device123 / $homie → 3.0
homie / device123 / $name → My device
homie / device123 / $state → ready
homie / device123 / $nodes → mythermostat

homie / device123 / mythermostat / $name → My thermostat
homie / device123 / mythermostat / $properties → temperature

homie / device123 / mythermostat / temperature → 22
homie / device123 / mythermostat / temperature / $name → Temperature
homie / device123 / mythermostat / temperature / $unit → °C
homie / device123 / mythermostat / temperature / $datatype → integer
homie / device123 / mythermostat / temperature / $settable → true
   * </pre>
   *
   * @return a list of messages to perform a homie introduction
   */
  void introduce();

  std::string getLifecycleTopic();
  Message getLwt();
  Message getLifecycleMsg();
};
}
