#pragma once

#include <string>
#include <list>
#include <map>
#include <tuple>
#include <iostream>
#include <sstream>

extern "C" {
#include <mbedtls/sha512.h>
}

namespace homie
{
    class Attribute;
    class Device;
    class Node;
    class Property;

    extern const std::string DEGREE_SYMBOL;
    extern const std::string HOMIE_VERSION;

    extern std::string DATA_TYPES[];
    extern std::string LIFECYCLE_STATES[];

    enum DataType
    {
        INTEGER = 0,
        STRING,
        FLOAT,
        PERCENT,
        BOOLEAN,
        ENUM,
        COLOR,
        DATETIME,
        DURATION
    };

    enum LifecycleState
    {
        INIT = 0,
        READY,
        DISCONNECTED,
        SLEEPING,
        LOST,
        ALERT
    };

    class Message
    {
    public:
        std::string topic;
        std::string payload;
        int qos;
        bool retained;
        Message(std::string topic, std::string payload, bool retained, int qos);
    };

    /**
     * @brief  Models a homie device. A homie device has 0 or many nodes, and  has exactly one mqtt connection.
     *
     */
    class Device
    {
    private:
        // required attributes:
        std::string id;
        std::string name;
        LifecycleState lifecycleState;
        std::map<std::string, Node *> nodes;
        std::list<std::string> extensions;
        std::string localIp;
        std::string mac;
        std::string version;

        std::string topicBase;
        std::list<Message> introductions;

        void computePsk();

    public:
        Device(std::string aid, std::string aVersion, std::string aname, std::string aLocalIp, std::string aMac);
        ~Device();

        std::string getTopicBase() { return topicBase; }
        void addNode(Node *n);

        void setLocalIp(std::string s) { localIp = s; }
        std::string getLocalIp() { return localIp; }

        std::string getMac() { return mac; }
        void setMac(std::string);

        LifecycleState getLifecycleState() { return lifecycleState; }
        void setLifecycleState(LifecycleState lcs) { lifecycleState = lcs; }

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
        std::list<Message> &introduce();

        std::string getLifecycleTopic();
        Message getLwt();
        Message getLifecycleMsg();
    };

    class Node
    {
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
        ~Node();

        std::string getId() { return id; }

        std::string getName() { return name; }
        std::string getType() { return type; }

        std::string getTopicBase() { return topicBase; }

        void addProperty(Property *p);
        Property *getProperty(std::string nm);
        void introduce(std::list<Message> &l);
    };

    class Property
    {
    private:
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
        Property(Node *anode, std::string id, std::string name, DataType dataType, bool settable);
        ~Property();

        std::string getId() { return id; }
        std::string getName() { return name; }
        std::string getDataTypeString() { return DATA_TYPES[(int)dataType]; }
        std::string getSubTopic() { return subTopic; }
        std::string getPubTopic() { return pubTopic; }

        std::string getFormat() { return format; }
        void setFormat(std::string f) { this->format = f; }
        void setValue(bool v);
        void setValue(float f);
        void setValue(int i);
        void setValue(std::string s);
        std::string getValue();

        std::string getUnit() { return unit; };
        void setUnit(std::string s) { unit = s; };

        bool getRetained() { return retained; }
        void setRetained(bool b) { retained = b; }

        void introduce(std::list<Message> &l);
    };

    template <typename T>
    std::string to_string(const T &n)
    {
        std::ostringstream stm;
        stm << n;
        return stm.str();
    }
}