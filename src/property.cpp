#include "homie.hpp"
namespace homie {
Property::Property(Node *anode, std::string aid, std::string aname,
                   DataType aDataType, bool asettable,
                   std::function<std::string(void)> areaderFunc) {
  id = aid;
  name = aname;
  node = anode;
  dataType = aDataType;
  settable = asettable;
  pubTopic = node->getTopicBase() + this->id;
  subTopic = node->getTopicBase() + this->id + "/set";
  node->addProperty(this);
  this->retained = true;
  this->hasWriterFunc = false;
  this->readerFunc = areaderFunc;
}

void Property::introduce() {
  // The validator fails unless there's a value message posted
  // before the metadata attributes
  // homie/super-car/engine/temperature → "21.5"
  // homie/super-car/engine/temperature/$name → "Engine temperature"
  // homie/super-car/engine/temperature/$settable → "false"
  // homie/super-car/engine/temperature/$datatype → "float"
  // homie/super-car/engine/temperature/$unit → "°C"
  // homie/super-car/engine/temperature/$format → "-20:120"
  this->node->getDevice()->publish(Message(pubTopic + "/$name", name));
  this->node->getDevice()->publish(
      Message(pubTopic + "/$settable", settable ? "true" : "false"));
  this->node->getDevice()->publish(
      Message(pubTopic + "/$datatype", DATA_TYPES[(int)dataType]));
  if (unit.length() > 0) {
    this->node->getDevice()->publish(Message(pubTopic + "/$unit", unit));
  }
  if (format.length() > 0) {
    this->node->getDevice()->publish(Message(pubTopic + "/$format", format));
  }
  this->publish();
}

void Property::publish(int qos) {
  Message m(this->getPubTopic(), this->readerFunc(), qos, this->retained);
  this->node->getDevice()->publish(m);
}
void Property::setWriterFunc(std::function<void(std::string)> f) {
  this->writerFunc = f;
  this->hasWriterFunc = true;
}

void Property::setValue(std::string v) {
  this->value = v;
  if (this->settable && this->hasWriterFunc) {
    this->writerFunc(v);
  }
}

std::string Property::read() {
  this->setValue(this->readerFunc());
  return this->getValue();
}

} // namespace homie
