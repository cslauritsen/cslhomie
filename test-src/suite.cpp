#include <gtest/gtest.h>
#include "homie.hpp"
#include <list>
#include <string>

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}

TEST(HomieSuite, ToFahrenheit) {
  ASSERT_FLOAT_EQ(homie::to_fahrenheit(28.0), 82.4);
}

TEST(HomieSuite, DeviceHasNode) {
    homie::Device *d = new homie::Device("unittest", "1.0", "unittest");
    homie::Node *n = new homie::Node(d, "node1", "Node1", "generic");
    EXPECT_TRUE(d->getNode("node1") == n);
    delete d;
}

TEST(HomieSuite, NodeHasProperty) {
    homie::Device *d = new homie::Device("unittest", "1.0", "unittest");
    homie::Node *n = new homie::Node(d, "node1", "Node1", "generic");
    homie::Property *p = new homie::Property(n, "prop1", "Prop1", homie::INTEGER, false, [](){ return "s1"; } );
    EXPECT_TRUE(n->getProperty("prop1") == p);
    delete d;
}

TEST(HomieSuite, NodeReadValue) {
    homie::Device *d = new homie::Device("unittest", "1.0", "unittest");
    homie::Node *n = new homie::Node(d, "node1", "Node1", "generic");
    homie::Property *p = new homie::Property(n, "prop1", "Prop1", homie::INTEGER, false, [](){ return "s1"; } );
    EXPECT_FALSE(p->isSettable());
    EXPECT_EQ("s1", p->read());
    delete d;
}

TEST(HomieSuite, SettableNodeWritesValue) {
    homie::Device *d = new homie::Device("unittest", "1.0", "unittest");
    homie::Node *n = new homie::Node(d, "node1", "Node1", "generic");
    homie::Property *p = new homie::Property(n, "prop1", "Prop1", homie::INTEGER, true, [](){ return "s1"; } );
    std::list<std::string> capture;
    p->setWriterFunc([&capture](std::string s) { capture.push_back(s); });
    EXPECT_TRUE(p->isSettable());
    EXPECT_EQ("s1", p->read());
    EXPECT_EQ(1, capture.size());
    p->setValue("wooga");
    EXPECT_EQ(2, capture.size());
    auto iter = capture.begin();
    EXPECT_EQ("s1",  *iter++);
    EXPECT_EQ("wooga",  *iter++);
    delete d;
}

TEST(HomieSuite, NonSettableNodeIgnoresWriterFunc) {
    homie::Device *d = new homie::Device("unittest", "1.0", "unittest");
    homie::Node *n = new homie::Node(d, "node1", "Node1", "generic");
    homie::Property *p = new homie::Property(n, "prop1", "Prop1", homie::INTEGER, false, [](){ return "s1"; } );
    std::list<std::string> capture;
    p->setWriterFunc([&capture](std::string s) { capture.push_back(s); });
    EXPECT_FALSE(p->isSettable());
    EXPECT_EQ("s1", p->read());
    EXPECT_EQ(0, capture.size());
    p->setValue("wooga");
    EXPECT_EQ(0, capture.size());
    auto iter = capture.begin();
    EXPECT_EQ(iter, capture.end());
    delete d;
}