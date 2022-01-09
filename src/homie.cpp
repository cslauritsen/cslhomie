#include "homie.hpp"
#include <algorithm>

namespace homie
{

    const std::string DEGREE_SYMBOL = "°";
    const std::string HOMIE_VERSION = "4.0";

    const std::string NODE_NM_WIFI = "wifi";
    const std::string PROP_NM_RSSI = "rssi";
    const std::string PROP_NM_WIFI_SIGNAL = "signal";

    std::string DATA_TYPES[] = {
        "integer", "string", "float", "percent", "boolean", "enum", "dateTime", "duration"};

    std::string LIFECYCLE_STATES[] = {
        "init", "ready", "disconnected", "sleeping", "lost", "alert"};

    Message::Message(std::string topic, std::string payload, bool retained = true, int qos = 1)
    {
        this->topic = topic;
        this->payload = payload;
        this->retained = retained;
        this->qos = qos;
    }

    Device::Device(std::string aid, std::string aVersion, std::string aname)
    {
        id = aid;

        // homie (or mqtt?) says that topic elements must be lower-case
        // do this incase the configurator doesnt know this
        std::transform(
            this->id.begin(),
            this->id.end(),
            this->id.begin(),
            [](unsigned char c)
            { return std::tolower(c); });

        this->version = aVersion;
        name = aname;
        topicBase = std::string("homie/") + id + "/";
        extensions.push_back(std::string("org.homie.legacy-firmware:0.1.1:[4.x]"));
        lifecycleState = INIT;

        this->wifiNode = new Node(this, NODE_NM_WIFI, "WiFi", "WIFI");
        this->rssiProp = new Property(this->wifiNode, PROP_NM_RSSI, "RSSI", homie::INTEGER, false, [this]()
                                      { return to_string(this->getRssi()); });
        this->wifiSignalProp = new Property(this->wifiNode, PROP_NM_WIFI_SIGNAL, "Wifi Signal", homie::INTEGER, false, [this]()
                                            { return to_string(this->getWifiSignalStrength()); });
        this->localIpProp = new Property(this->wifiNode, "localip", "Local IP", homie::STRING, false,
                                         [this]()
                                         { return this->localIp; });
        this->macProp = new Property(this->wifiNode, "mac", "MAC Address", homie::STRING, false,
                                         [this]()
                                         { return formatMac(this->mac); });
    }

    void Device::publishWifi()
    {
        this->rssiProp->publish();
        this->wifiSignalProp->publish();
    }

    void Device::addNode(Node *n)
    {
        nodes[n->getId()] = n;
    }

    Node *Device::getNode(std::string nm)
    {
        auto search = nodes.find(nm);
        if (search == nodes.end())
        {
            return nullptr;
        }
        return search->second;
    }

    std::string formatMac(std::string str)
    {
        std::string tmp;
        // remove all colons  (if any)
        str.erase(std::remove(str.begin(), str.end(), ':'), str.end());
        // put them back
        for (std::string::size_type i = 0; i < str.length(); i += 2)
        {
            if (i > 0)
            {
                tmp += ':';
            }
            tmp += str[i];
            tmp += str[i + 1];
        }
        return tmp;
    }

    void Device::introduce()
    {
        int i;
        this->publish(Message(topicBase + "$homie", HOMIE_VERSION));
        this->publish(Message(topicBase + "$name", name));
        this->publish(Message(topicBase + "$implementation", std::string("cslhomie")));
        this->setLifecycleState(homie::INIT);
        this->publish(getLifecycleMsg());

        std::string exts("");
        i = 0;
        for (auto elm : extensions)
        {
            if (i++ > 0)
            {
                exts += ",";
            }
            exts += elm;
        }
        this->publish(Message(topicBase + "$extensions", exts));

        this->publish(Message(topicBase + "$localip", this->localIpProp->valueFunction()));
        this->publish(Message(topicBase + "$mac", this->macProp->valueFunction()));
        this->publish(Message(topicBase + "$fw/name", id + "-firmware"));
        this->publish(Message(topicBase + "$fw/version", version));

        std::string nodeList("");
        i = 0;
        for (auto elm : nodes)
        {
            if (i++ > 0)
            {
                nodeList += ",";
            }
            nodeList += elm.first;
        }
        this->publish(Message(topicBase + "$nodes", nodeList));

        for (auto e : nodes)
        {
            e.second->introduce();
        }
        this->setLifecycleState(homie::READY);
        this->publish(getLifecycleMsg());
    }

    int Device::getWifiSignalStrength()
    {
        auto rssi = getRssi();
        if (rssi <= -100)
            return 0;
        if (rssi >= -50)
            return 100;
        return 2 * (rssi + 100);
    }

    Message Device::getLwt()
    {
        return Message(getLifecycleTopic(), LIFECYCLE_STATES[(int)homie::LOST]);
    }

    std::string Device::getLifecycleTopic()
    {
        return topicBase + "$state";
    }

    Message Device::getLifecycleMsg()
    {
        return Message(getLifecycleTopic(), LIFECYCLE_STATES[(int)lifecycleState]);
    }

    Node::Node(Device *d, std::string aid, std::string aname, std::string nodeType)
    {
        device = d;
        id = aid;
        name = aname;
        type = nodeType;
        topicBase = device->getTopicBase() + id + "/";
        device->addNode(this);
    }

    void Node::addProperty(Property *p)
    {
        if (p)
        {
            properties[p->getId()] = p;
        }
    }

    void Node::introduce()
    {
        // homie/super-car/engine/$name → "Car engine"
        // homie/super-car/engine/$type → "V8"
        // homie/super-car/engine/$properties → "speed,direction,temperature"
        this->device->publish(Message(topicBase + "$name", name));
        this->device->publish(Message(topicBase + "$type", type));
        std::string propList;
        int i = 0;
        for (auto e : properties)
        {
            if (i++ > 0)
                propList += ',';
            propList += e.first;
        }
        this->device->publish(Message(topicBase + "$properties", propList));
        for (auto e : properties)
        {
            e.second->introduce();
        }
    }

    Property *Node::getProperty(std::string nm)
    {
        auto search = properties.find(nm);
        if (search == properties.end())
        {
            return nullptr;
        }
        return search->second;
    }

    Property::Property(Node *anode, std::string aid,
                       std::string aname, DataType aDataType, bool asettable, std::function<std::string(void)> acquireFunc)
    {
        id = aid;
        name = aname;
        node = anode;
        dataType = aDataType;
        settable = asettable;
        pubTopic = node->getTopicBase() + this->id;
        subTopic = node->getTopicBase() + this->id + "/set";
        node->addProperty(this);
        this->retained = true;
        this->valueFunction = acquireFunc;
    }

    void Property::introduce()
    {

        // The validator fails unless there's a value message posted
        // before the metadata attributes
        // homie/super-car/engine/temperature → "21.5"
        // homie/super-car/engine/temperature/$name → "Engine temperature"
        // homie/super-car/engine/temperature/$settable → "false"
        // homie/super-car/engine/temperature/$datatype → "float"
        // homie/super-car/engine/temperature/$unit → "°C"
        // homie/super-car/engine/temperature/$format → "-20:120"
        this->node->getDevice()->publish(Message(pubTopic + "/$name", name));
        this->node->getDevice()->publish(Message(pubTopic + "/$settable", settable ? "true" : "false"));
        this->node->getDevice()->publish(Message(pubTopic + "/$datatype", DATA_TYPES[(int)dataType]));
        if (unit.length() > 0)
        {
            this->node->getDevice()->publish(Message(pubTopic + "/$unit", unit));
        }
        if (format.length() > 0)
        {
            this->node->getDevice()->publish(Message(pubTopic + "/$format", format));
        }
        this->publish();
    }

    void Property::publish(int qos)
    {
        Message m(this->getPubTopic(), this->valueFunction(), qos, this->retained);
        this->node->getDevice()->publish(m);
    }

    std::string to_string(bool v)
    {
        return std::string(v ? "true" : "false");
    }

    template <typename T>
    std::string to_string(const T &t)
    {
        std::ostringstream stm;
        stm << t;
        return stm.str();
    }
}
