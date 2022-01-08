#include "homie.hpp"
#include <algorithm>

namespace homie
{

    const std::string DEGREE_SYMBOL = "°";
    const std::string HOMIE_VERSION = "4.0";

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

    Device::Device(std::string aid, std::string aVersion, std::string aname, std::string aLocalIp, std::string aMac)
    {
        id = aid;
        this->version = aVersion;
        name = aname;
        this->localIp = aLocalIp;
        this->setMac(aMac);
        topicBase = std::string("homie/") + id + "/";
        this->computePsk();
        extensions.push_back(std::string("org.homie.legacy-firmware:0.1.1:[4.x]"));
        lifecycleState = INIT;
    }

    Device::~Device()
    {
        std::cerr << "deleting device " << name << std::endl;
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

    void Device::setMac(std::string str)
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
        std::cout << "setting mac: " << tmp << std::endl;
        this->mac = tmp;
    }

    void Device::introduce()
    {

        int i;
        Message m(topicBase + "$homie", HOMIE_VERSION);
        this->publish(m);
        this->publish(m = Message(topicBase + "$name", name));
        this->publish(m = Message(topicBase + "$implementation", std::string("cslhomie")));
        this->setLifecycleState(homie::INIT);
        this->publish(m = getLifecycleMsg());

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
        this->publish(m = Message(topicBase + "$extensions", exts));

        this->publish(m = Message(topicBase + "$localip", localIp));
        this->publish(m = Message(topicBase + "$mac", mac));
        this->publish(m = Message(topicBase + "$fw/name", id + "-firmware"));
        this->publish(m = Message(topicBase + "$fw/version", version));

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
        this->publish(m = Message(topicBase + "$nodes", nodeList));

        for (auto e : nodes)
        {
            e.second->introduce();
        }
        this->setLifecycleState(homie::READY);
        this->publish(m = getLifecycleMsg());
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

    Node::~Node()
    {
        std::cerr << "deleting node " << id << std::endl;
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
        Message m(topicBase + "$name", name);
        this->device->publish(m);
        this->device->publish(m = Message(topicBase + "$type", type));
        std::string propList;
        int i = 0;
        for (auto e : properties)
        {
            if (i++ > 0)
                propList += ',';
            propList += e.first;
        }
        this->device->publish(m = Message(topicBase + "$properties", propList));
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
                       std::string aname, DataType aDataType, bool asettable)
    {
        id = aid;
        name = aname;
        node = anode;
        dataType = aDataType;
        settable = asettable;
        pubTopic = node->getTopicBase() + this->id;
        subTopic = node->getTopicBase() + this->id + "/set";
        node->addProperty(this);
    }

    Property::~Property()
    {
        std::cerr << "deleting property " << id << std::endl;
    }
    void Property::setValue(bool v)
    {
        value = v ? "true" : "false";
    }
    void Property::setValue(float f)
    {
        value = to_string(f);
    }
    void Property::setValue(int i)
    {
        value = to_string(i);
    }
    void Property::setValue(std::string s)
    {
        value = s;
    }
    std::string Property::getValue()
    {
        return this->value;
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
        Message m(pubTopic, value);
        this->node->getDevice()->publish(m);
        this->node->getDevice()->publish(m = Message(pubTopic + "/$name", name));
        this->node->getDevice()->publish(m = Message(pubTopic + "/$settable", settable ? "true" : "false"));
        this->node->getDevice()->publish(m = Message(pubTopic + "/$datatype", DATA_TYPES[(int)dataType]));
        if (unit.length() > 0)
        {
            this->node->getDevice()->publish(m = Message(pubTopic + "/$unit", unit));
        }
        if (format.length() > 0)
        {
            this->node->getDevice()->publish(m = Message(pubTopic + "/$format", format));
        }
    }

    void Property::publish(int qos, bool retain) {
        Message m(this->getPubTopic(), this->getValue(), qos, retain);
        this->node->getDevice()->publish(m);
    }
}
