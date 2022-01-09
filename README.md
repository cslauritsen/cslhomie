# mgos-homie
A simple C++ library to model the homie convention for use with Mongoose OS.

# Rationale
The other implementations I found didn't seem to match well what I wanted to do. I noticed especially with Mongoose OS on ESP8266 that I needed to pace publication. When I tried to send a bunch of messages all at once it didn't work, it seemed to saturate the networking buffer or someothing.

__Update: I learned that increasing the `mqtt.max_queue_length` config setting to match my burst of messages gave much better results and much simpler code than trying to pace message publication with a list or vector. I may still need to consider it for devices that have a larger homie message tree.__

 The purpose of this library is to provide a model to generate and manage a set of homie-style messages. Publishing the messages is left to the caller to coordinate. 

# Requirements
This requires C++ and its standard template libs. It was designed for my Mongoose OS projects, but it has no dependencies on Mongoose OS itself. The github repository layout is defined by the Mongoose OS library requirements.

# Usage
Create a `homie::Device` object, add `homie::Node` objects to the device, and add `homie::Property` objects to the node. Call `homie::Device::introduce` to build a comprehensive list of messages that you'll want to publish initially. 

Have a look at some [sample code](test-src/test.cpp).

# Contributing
Feel free to make pull requests. You can get faster turnaround by building and testing locally with `cmake` :
```shell
mkdir -p build
cd build
cmake ..
make
./test_prog
```