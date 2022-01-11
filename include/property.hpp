#pragma once
#include "homie.hpp"

namespace homie {
class Property {
private:
  /** a function or lambda to provide values. */
  std::string pubTopic;
  std::string subTopic;
  /** Required unique id of property */
  std::string id;
  /** Required friendly name of property */
  std::string name;
  /** Required data type of property */
  DataType dataType;

  /**
      * @brief  Format:

  For integer and float: Describes a range of payloads e.g. 10:15

  For enum: payload,payload,payload for enumerating all valid payloads.

  For color:

  rgb to provide colors in RGB format e.g. 255,255,0 for yellow.
  hsv to provide colors in HSV format e.g. 60,100,100 for yellow.

*
*/
  std::string format;
  bool settable;
  bool retained;
  std::string unit;
  std::string value;

  /** Node owning this property */
  Node *node;

public:
  Property(Node *anode, std::string id, std::string name, DataType dataType,
           bool settable);
  virtual ~Property();

  /**
   * @brief A function that gets ths property's value as a string.
   *
   */
  std::function<std::string(void)> readerFunc;

  /**
   * @brief A function that accepts a string value and does something with the
   * hardware in response. Only called when settable==true.
   *
   */
  std::optional<std::function<void(std::string)>> writerFunc;

  std::string getId() { return id; }
  std::string getName() { return name; }
  std::string getDataTypeString() { return DATA_TYPES[(int)dataType]; }
  std::string getSubTopic() { return subTopic; }
  std::string getPubTopic() { return pubTopic; }

  std::string getValue() { return value; }
  void setValue(std::string v) { this->value = v; }

  std::string getFormat() { return format; }
  void setFormat(std::string f) { this->format = f; }

  std::string getUnit() { return unit; };
  void setUnit(std::string s) { unit = s; };

  bool getRetained() { return retained; }
  void setRetained(bool b) { retained = b; }

  bool isSettable() { return settable; }

  Node *getNode() { return node; }

  void introduce();
  void publish(int qos = 1);
};
} // namespace homie
