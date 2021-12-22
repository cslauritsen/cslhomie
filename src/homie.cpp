#include "homie.hpp"

namespace homie
{

    const std::string DEGREE_SYMBOL = "°";
    const std::string HOMIE_VERSION = "3.0";

    std::string DATA_TYPES[] = {
        "integer", "string", "float", "percent", "boolean", "enum", "dateTime", "duration"};

    std::string LIFECYCLE_STATES[] = {
        "init", "ready", "disconnected", "sleeping", "lost", "alert"};

    Message::Message(std::string topic, std::string payload)
    {
        this->topic = topic;
        this->payload = payload;
        this->qos = 1;
        this->retained = true;
    }

    Message::Message(std::string topic, std::string payload, bool retained)
    {
        this->topic = topic;
        this->payload = payload;
        this->retained = retained;
        this->qos = 1;
    }

    Message::Message(std::string topic, std::string payload, bool retained, int qos)
    {
        this->topic = topic;
        this->payload = payload;
        this->retained = retained;
        this->qos = qos;
    }

    Device::Device(std::string aid, std::string aname)
    {
        id = aid;
        name = aname;
        topicBase = std::string("homie/") + id + "/";
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

    void Device::introduce(std::list<Message *> *l)
    {
        if (!l)
            return;
        for (auto e : *l)
        {
            delete e;
        }
        l->clear();

        int i;
        l->push_back(new Message(topicBase + "$homie", HOMIE_VERSION));
        l->push_back(new Message(topicBase + "$name", name));
        l->push_back(new Message(topicBase + "$state", LIFECYCLE_STATES[(int)lifecycleState]));
        if (macAddr.length() > 0) {
            l->push_back(new Message(topicBase + "$mac-addr", macAddr));
        }
        if (ipAddr.length() > 0) {
            l->push_back(new Message(topicBase + "$ip-addr", ipAddr));
        }

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
        l->push_back(new Message(topicBase + "$nodes", nodeList));

        for (auto e : nodes)
        {
            e.second->introduce(l);
        }
    }

    Node::Node(Device *d, std::string aid, std::string aname, std::string nodeType)
    {
        device = d;
        id = aid;
        name = aname;
        type = nodeType;
        topicBase = device->getTopicBase() + id + "/";
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

    void Node::introduce(std::list<Message *> *l)
    {
        // homie/super-car/engine/$name → "Car engine"
        // homie/super-car/engine/$type → "V8"
        // homie/super-car/engine/$properties → "speed,direction,temperature"
        l->push_back(new Message(topicBase + "$name", name));
        l->push_back(new Message(topicBase + "$type", type));
        std::string propList;
        int i = 0;
        for (auto e : properties)
        {
            if (i++ > 0)
                propList += ',';
            propList += e.first;
        }
        l->push_back(new Message(topicBase + "$properties", propList));
        for (auto e : properties) {
            e.second->introduce(l);
        }
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

    void Property::introduce(std::list<Message *> *l)
    {
        // homie/super-car/engine/temperature/$name → "Engine temperature"
        // homie/super-car/engine/temperature/$settable → "false"
        // homie/super-car/engine/temperature/$datatype → "float"
        // homie/super-car/engine/temperature/$unit → "°C"
        // homie/super-car/engine/temperature/$format → "-20:120"
        // homie/super-car/engine/temperature → "21.5"
        l->push_back(new Message(pubTopic + "/$name", name));
        l->push_back(new Message(pubTopic + "/$settable", settable ? "true" : "false"));
        l->push_back(new Message(pubTopic + "/$datatype", DATA_TYPES[(int)dataType]));
        if (unit.length() > 0) {
            l->push_back(new Message(pubTopic + "/$unit", unit));
        }
        if (format.length() > 0) {
            l->push_back(new Message(pubTopic + "/$format", format));
        }
        if (value.length() > 0) {
            l->push_back(new Message(pubTopic, value));
        }
    }

}


bool mgos_cslhomie_init(void) {
  return true;
}