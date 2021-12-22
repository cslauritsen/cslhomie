#include "homie.hpp"
#include <iostream>
#include <string>
using namespace std;

static void test_homie() {
    homie::Device *d = new homie::Device("device123", "My Device");
    d->setIpAddr("192.168.1.39");

    homie::Node *dht22Node  = new homie::Node(d, "dht22", "DHT22 Temp/RH Sensor", "DHT22");
    d->addNode(dht22Node);

    auto tempProp = new homie::Property(dht22Node, "tempf", "Temperature in Fahrenheit", homie::FLOAT, false);
    tempProp->setUnit(homie::DEGREE_SYMBOL + "F");
    float tempF = 72.0;
    tempProp->setValue(tempF);
    dht22Node->addProperty(tempProp);

    auto rhProp = new homie::Property(dht22Node, "rh", "Temperature in Fahrenheit", homie::FLOAT, false);
    rhProp->setUnit("%");
    float rh = 61.0;
    rhProp->setValue(rh);
    dht22Node->addProperty(rhProp);

    auto doorNode = new homie::Node(d, "doora", "South Garage Door", "door");
    d->addNode(doorNode);

    auto openProp = new homie::Property(doorNode, "isopen", "Is Door Open", homie::BOOLEAN, false);
    doorNode->addProperty(openProp);

    auto relayProp = new homie::Property(doorNode, "relay", "Door Activator", homie::INTEGER, true);
    doorNode->addProperty(relayProp);

    auto introduction = new std::list<homie::Message *>;
    d->introduce(introduction);
    for (auto msg : *introduction) {
        std::cout << msg->topic 
        << "[q:" << msg->qos << ",r:" << msg->retained << ']' 
        << " → " 
        << msg->payload << std::endl;
    }
    delete introduction;
    delete relayProp;
    delete openProp;
    delete rhProp;
    delete tempProp;
    delete dht22Node;
    delete doorNode;
    delete d;
}
int main(int argc, char** argv) {
     test();
}