#pragma once

#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <iostream>
#include <sstream>
#include <functional>

#ifndef NO_MBEDTLS
extern "C"
{
#include <mbedtls/sha512.h>
}
#endif

namespace homie
{
    class Device;
    class Node;

    extern const std::string DEGREE_SYMBOL;
    extern const std::string HOMIE_VERSION;

    extern const std::string NODE_NM_WIFI;
    extern const std::string PROP_NM_RSSI;
    extern const std::string PROP_NM_WIFI_SIGNAL;

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

    class Property
    {
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
        Property(Node *anode, std::string id, std::string name, DataType dataType, bool settable);
        virtual ~Property();

        /**
         * @brief A function that gets ths property's value as a string.
         *
         */
        std::function<std::string(void)> valueFunction;

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

        Node *getNode() { return node; }

        void introduce();
        void publish(int qos = 1);
    };

    /**
     * @brief  Models a homie device. A homie device has 0 or many nodes, and  has exactly one mqtt connection.
     *
     */
    class Device
    {
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

        Node *wifiNode;
        Property *rssiProp;
        Property *wifiSignalProp;
        Property *localIpProp;
        Property *macProp;

    public:
        Device(std::string aid, std::string aVersion, std::string aname);
        virtual ~Device();

        virtual void publish(Message);

        void setLocalIp(std::string s) { this->localIp = s;}
        std::string getLocalIp() { return this->localIp; }

        void setMac(std::string s) { this->mac = s;}
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

    std::string to_string(bool v);

    template <typename T>
    std::string to_string(const T &n);

    std::string formatMac(std::string mac);
}