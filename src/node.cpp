#include "homie.hpp"
namespace homie {
Node::Node(Device *d, std::string aid, std::string aname,
           std::string nodeType) {
  device = d;
  id = aid;
  name = aname;
  type = nodeType;
  topicBase = device->getTopicBase() + id + "/";
  device->addNode(this);
}

void Node::addProperty(Property *p) {
  if (p) {
    properties[p->getId()] = p;
  }
}

void Node::introduce() {
  // homie/super-car/engine/$name → "Car engine"
  // homie/super-car/engine/$type → "V8"
  // homie/super-car/engine/$properties → "speed,direction,temperature"
  this->device->publish(Message(topicBase + "$name", name));
  this->device->publish(Message(topicBase + "$type", type));
  std::string propList;
  int i = 0;
  for (auto e : properties) {
    if (i++ > 0)
      propList += ',';
    propList += e.first;
  }
  this->device->publish(Message(topicBase + "$properties", propList));
  for (auto e : properties) {
    e.second->introduce();
  }
}

Property *Node::getProperty(std::string nm) {
  auto search = properties.find(nm);
  if (search == properties.end()) {
    return nullptr;
  }
  return search->second;
}

}
