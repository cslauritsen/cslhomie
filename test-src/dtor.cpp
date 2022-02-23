#include "homie.hpp"
#include <iostream>

const bool dtor_debug = false;

homie::Device::~Device() {
  for (auto node : this->nodes) {
    delete node.second;
  }
}

homie::Node::~Node() {
  if (dtor_debug)
    std::cerr << " . Deleting node " << this->id << std::endl;
  for (auto prop : this->properties) {
    delete prop.second;
  }
}

homie::Property::~Property() {
  if (dtor_debug)
    std::cerr << " .   Deleting prop " << this->id << std::endl;
}