#pragma once
#include "homie.hpp"

namespace homie {
  class Property;
class Node {
private:
  /** Required unique id of node */
  std::string id;
  /** Required friendly name of node */
  std::string name;
  /** Required arbitrary string describing the type of node */
  std::string type;
  /** Required list of properties the node exposes */
  std::map<std::string, Property *> properties;

  /** The device owning this node */
  Device *device;

  std::string topicBase;
  std::string psk;
  std::string identity;

public:
  Node(Device *d, std::string id, std::string aname, std::string nodeType);
  virtual ~Node();

  std::string getId() { return id; }
  Device *getDevice() { return device; }

  std::string getName() { return name; }
  std::string getType() { return type; }

  std::string getTopicBase() { return topicBase; }

  void addProperty(Property *p);
  Property *getProperty(std::string nm);
  void introduce();
};
} // namespace homie
