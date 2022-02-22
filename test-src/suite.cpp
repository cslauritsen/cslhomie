#include "homie.hpp"
#include <gtest/gtest.h>
#include <list>
#include <string>

class PropertyTest : public ::testing::Test {
protected:
  void SetUp() override {
    d = new homie::Device("unittest", "1.0", "unittest");
    n = new homie::Node(d, "node1", "Node1", "generic");
    p = new homie::Property(n, "prop1", "Prop1", homie::INTEGER, isWritable(),
                            []() { return "s1"; });
    p->setWriterFunc([this](std::string s) { this->capture.push_back(s); });
  }

  void TearDown() override { delete d; }

  virtual bool isWritable() { return false; }

  homie::Device *d;
  homie::Node *n;
  homie::Property *p;
  std::list<std::string> capture;
};

class WritablePropertyTest : public PropertyTest {
protected:
  bool isWritable() override { return true; }
};

TEST(HomieSuite, ToFahrenheit) {
  ASSERT_FLOAT_EQ(homie::to_fahrenheit(28.0), 82.4);
}

TEST_F(WritablePropertyTest, NodeIsPresent) {
  EXPECT_TRUE(d->getNode("node1") == n);
}

TEST_F(PropertyTest, PropertyIsPresent) {
  EXPECT_TRUE(n->getProperty("prop1") == p);
}

TEST_F(PropertyTest, PropertyIsRead) {
  EXPECT_FALSE(p->isSettable());
  EXPECT_EQ("s1", p->read());
}

TEST_F(WritablePropertyTest, WritablePropertyWritesValue) {
  EXPECT_TRUE(p->isSettable());
  EXPECT_EQ("s1", p->read());
  EXPECT_EQ(1, capture.size())
      << "Captured writes should have 1 member from read()";
  p->setValue("wooga");
  EXPECT_EQ(2, capture.size())
      << "Captured writes should have 2 members after setValue()";
  auto iter = capture.begin();
  EXPECT_EQ("s1", *iter++)
      << "Captured writes first member should be from read()";
  EXPECT_EQ("wooga", *iter++)
      << "Capture writes second member should be from setValue()";
}

TEST_F(PropertyTest, NonWritablePropertyIgnoresWriterFunc) {
  EXPECT_FALSE(p->isSettable());
  EXPECT_EQ("s1", p->read());
  EXPECT_EQ(0, this->capture.size()) << "Captured writes list should be empty";
  p->setValue("wooga");
  EXPECT_EQ(0, this->capture.size())
      << "Captured writes list should be empty, even after setValue()";
  auto iter = this->capture.begin();
  EXPECT_EQ(iter, this->capture.end())
      << "Captured writes list should not iterate";
}