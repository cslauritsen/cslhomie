#include "homie.hpp"
#include <iostream>
#include <string>
using namespace std;

static void test_homie() {
    homie::Device *d = new homie::Device("device123", "1.2.3", "My Device", "192.168.1.69", "aa:bb:cc:dd:ee:ff");
    d->setLocalIp("192.168.1.39");

    homie::Node *dht22Node  = new homie::Node(d, "dht22", "DHT22 Temp/RH Sensor", "DHT22");
    d->addNode(dht22Node);

    auto tempProp = new homie::Property(dht22Node, "tempf", "Temperature in Fahrenheit", homie::FLOAT, false);
    tempProp->setUnit(homie::DEGREE_SYMBOL + "F");
    float tempF = 72.0;
    tempProp->setValue(tempF);
    dht22Node->addProperty(tempProp);

    auto rhProp = new homie::Property(dht22Node, "rh", "Relative Humidity", homie::FLOAT, false);
    rhProp->setUnit("%");
    float rh = 61.0;
    rhProp->setValue(rh);
    dht22Node->addProperty(rhProp);

    auto doorNode = new homie::Node(d, "doora", "South Garage Door", "door");
    d->addNode(doorNode);

    auto openProp = new homie::Property(doorNode, "isopen", "Door Contact", homie::BOOLEAN, false);
    doorNode->addProperty(openProp);

    auto relayProp = new homie::Property(doorNode, "relay", "Door Activator", homie::INTEGER, true);
    doorNode->addProperty(relayProp);

    for (auto msg : d->introduce()) {
        std::cout 
        //<< "[q:" << msg.qos << ",r:" << msg.retained << "] "
        << msg.topic 
        //<< " → " 
        << " " 
        << msg.payload << std::endl;
    }

    std::cout << "relay cmd topic: " << relayProp->getSubTopic() << std::endl;
    homie::Message lwt = d->getLwt();
    std::cout << "LWT: " << lwt.topic << " " << lwt.payload << std::endl;

    delete relayProp;
    delete openProp;
    delete rhProp;
    delete tempProp;
    delete dht22Node;
    delete doorNode;
    delete d;
}
int main(int argc, char** argv) {
     test_homie();
}