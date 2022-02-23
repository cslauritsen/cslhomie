#include "homie.hpp"
#include <algorithm>
#include <gtest/gtest.h>
#include <list>
#include <string>
#include <string>

using Msg = homie::Message;
class TestDevice : public homie::Device {
public:
  TestDevice() : homie::Device("testdevice", "1.0", "TestDevice") {
    extensions.push_back("com.planetlauritsen.test:0.0.1:[4.x]");
  }
  std::list<homie::Message> publications;
  int rssi = -58;

  int getRssi() override { return rssi; }
  std::vector<std::string> getExtensions() { return extensions; }

protected:
  void publish(homie::Message m) override {
    std::cerr << "publishing to " << m.topic << std::endl;
    publications.push_back(m);
  }
};

class PropertyTest : public ::testing::Test {
protected:
  void SetUp() override {
    d = new TestDevice();
    n = new homie::Node(d, "node1", "Node1", "generic");
    p = new homie::Property(n, "prop1", "Prop1", homie::INTEGER, isWritable(),
                            []() { return "s1"; });
    p->setWriterFunc([this](std::string s) { this->capture.push_back(s); });
  }

  void TearDown() override { delete d; }

  virtual bool isWritable() { return false; }

  TestDevice *d;
  homie::Node *n;
  homie::Property *p;
  std::list<std::string> capture;
};

class WritablePropertyTest : public PropertyTest {
protected:
  bool isWritable() override { return true; }
};

TEST(HomieSuite, ToFahrenheit) {
  EXPECT_FLOAT_EQ(homie::to_fahrenheit(28.0), 82.4);
}

TEST(HomieSuite, formatMac) {
  EXPECT_EQ("aa:bb:cc:dd:ee:ff", homie::formatMac("aabb:ccdd:eeff"));
}

TEST(HomieSuite, bool_to_string) {
  EXPECT_EQ("false", homie::to_string(false));
  EXPECT_EQ("true", homie::to_string(true));
}

TEST(HomieSuite, splitString) {
  std::vector<std::string> res;
  homie::split_string("a/b/c", "/", res);
  EXPECT_EQ(3, res.size());
  EXPECT_EQ("a", res[0]);
  EXPECT_EQ("b", res[1]);
  EXPECT_EQ("c", res[2]);
}

TEST(HomieSuite, f2s) {
    float f = 1.22222;
  auto x = homie::f2s(f);
  ASSERT_EQ("1.2", x) << f << " % %.1f => 100";
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

TEST_F(PropertyTest, PropertyPublish) {
  p->publish();
  EXPECT_EQ(1, d->publications.size()) << "Publication count should be " << 1;
}

TEST_F(PropertyTest, PropertyIntroduce) {
  p->introduce();
  EXPECT_GT(d->publications.size(), 3)
      << "After introduction, publication count should be > 3";
}

TEST_F(PropertyTest, PropertyIntroduceUnit) {
  p->setUnit("jigawatts");
  p->introduce();
  auto count = std::count_if(
      d->publications.begin(), d->publications.end(), [](homie::Message m) {
        return m.topic.find("$unit") != std::string::npos;
      });
  EXPECT_EQ(1, count)
      << "There should be one and only one $unit topic emitted.";
}

TEST_F(PropertyTest, PropertyIntroduceFormat) {
  p->setFormat("1:9");
  p->introduce();
  auto l = d->publications;
  auto count = std::count_if(l.begin(), l.end(), [](Msg m) {
    return m.topic.find("$format") != std::string::npos;
  });
  EXPECT_EQ(1, count)
      << "There should be one and only one $format topic emitted.";
  auto count2 = std::count_if(l.begin(), l.end(),
                              [](Msg m) { return m.payload == "1:9"; });
  EXPECT_EQ(1, count2)
      << "There should be one and only one $format payload emitted.";
}

TEST_F(PropertyTest, NodePropertyNotFound) {
  EXPECT_EQ(nullptr, n->getProperty("wooga"))
      << "Nonexistent property should return nullptr";
}

TEST_F(PropertyTest, NodeSinglePropertyIntroduction) {
  n->introduce();
  auto count = std::count_if(d->publications.begin(), d->publications.end(), [](Msg m) {
    return m.topic.find("$properties") != std::string::npos;
  });
  auto count2 = std::count_if(d->publications.begin(), d->publications.end(), [](Msg m) {
    return m.payload.find(",") != std::string::npos;
  });
  EXPECT_EQ(1, count)
      << "only one $properties message should be published";
  EXPECT_EQ(0, count2)
      << "no commas in single properties";
}

TEST_F(PropertyTest, NodeMultiplePropertyIntroduction) {
  new homie::Property(n, "prop2", "Prop2", homie::INTEGER, isWritable(),
                            []() { return "s2"; });
  n->introduce();
  auto count = std::count_if(d->publications.begin(), d->publications.end(), [](Msg m) {
    return m.topic.find("$properties") != std::string::npos;
  });
  auto count2 = std::count_if(d->publications.begin(), d->publications.end(), [](Msg m) {
    return m.payload.find(",") != std::string::npos;
  });
  EXPECT_EQ(1, count)
      << "only one $properties message should be published";
  EXPECT_EQ(1, count2)
      << "single comma in properties";
}

TEST_F(PropertyTest, WifiSignalStrength) {
  d->rssi = -100;
  EXPECT_EQ(0, d->getWifiSignalStrength());

  d->rssi = -49;
  EXPECT_EQ(100, d->getWifiSignalStrength());

  d->rssi = -58;
  EXPECT_EQ(84, d->getWifiSignalStrength());
  d->introduce();
}

TEST_F(PropertyTest, PublishWifi) {
  d->rssi = -49;// -> signal strength "100"
  d->publishWifi();
  EXPECT_EQ(1, std::count_if(d->publications.begin(), d->publications.end(), [this](Msg m){ return m.topic.find(homie::PROP_NM_RSSI) != std::string::npos && m.payload == std::to_string(this->d->rssi); }));
  EXPECT_EQ(1, std::count_if(d->publications.begin(), d->publications.end(), [](Msg m){ return m.topic.find("/signal") != std::string::npos && m.payload == "100"; }));
}

TEST_F(PropertyTest, PublishLocalIp) {
  d->setLocalIp("1.2.3.4");
  d->introduce();
  EXPECT_EQ(1, std::count_if(d->publications.begin(), d->publications.end(), [this](Msg m){ return m.topic.find("/localip") != std::string::npos && m.payload == this->d->getLocalIp(); }));
}

TEST_F(PropertyTest, PublishMac) {
  d->setMac("fe:ed:fa:ce:de:ad");
  d->introduce();
  EXPECT_EQ(1, std::count_if(d->publications.begin(), d->publications.end(), [this](Msg m){ return m.topic.find("/mac") != std::string::npos && m.payload == this->d->getMac(); }));
}

TEST_F(WritablePropertyTest, CheckSubTopic) {
  EXPECT_EQ(p->getPubTopic() + "/set", p->getSubTopic());
}

TEST_F(PropertyTest, CheckMissingNodeReturnsNullPtr) {
  EXPECT_EQ(nullptr, d->getNode("fjdfkdlkff0dfj00-0l"));
}

TEST_F(PropertyTest, CheckLwtIsLost) {
  EXPECT_EQ("lost", d->getLwt().payload);
}

TEST_F(WritablePropertyTest, CheckInputMessage) {
  auto inputMsg = Msg("homie/" + d->getId() + "/" + n->getId() + "/" + p->getId() + "/set", "awesome new value");
  d->onMessage(inputMsg);
  EXPECT_EQ(inputMsg.payload, p->getValue());
}

TEST_F(PropertyTest, InputMessageIgnoredForNonWritableProperty) {
  auto inputMsg = Msg("homie/" + d->getId() + "/" + n->getId() + "/" + p->getId() + "/set", "woo-hoo");
  p->setValue("previous");
  d->onMessage(inputMsg);
  EXPECT_EQ("previous", p->getValue());
}

TEST_F(PropertyTest, CheckExtensions) {
  EXPECT_EQ(2, d->getExtensions().size());
}