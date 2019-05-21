/*
 */
#include <set>
#include "buffer.hpp"
#include "gtest/gtest.h"

using namespace std;

namespace {

// The fixture for testing class Foo.
class FooTest : public ::testing::Test {
 protected:
  // You can remove any or all of the following functions if its body
  // is empty.

  FooTest() {
     // You can do set-up work for each test here.
  }

  ~FooTest() override {
     // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  void SetUp() override {
     // Code here will be called immediately after the constructor (right
     // before each test).
  }

  void TearDown() override {
     // Code here will be called immediately after each test (right
     // before the destructor).
  }

  // Objects declared here can be used by all tests in the test case for Foo.
};
};

string AppMessage(unsigned char oper, vector<string> strs)
{
    string rtn="";

    rtn.push_back(oper);

    for(auto str:strs)
    {
        rtn=rtn+str+'\n';
    }

    int len=rtn.size();
    char len0=len%256;
    char len1=(len/256)%256;
    rtn=len0+rtn;
    rtn=len1+rtn;

    return rtn;

}
ostream& operator<<(ostream& os, const vector<string>& rtn)
{
    for(auto tmp:rtn)
    {
        cout<<tmp<<'\n';
    }
    return os;
}
TEST_F(FooTest,ShortString ) {
    Buffer buf;
    vector<string> strs;
    string str;
    vector<string> rtn;
    strs={"123","345"};
    str=AppMessage(1,strs);
    rtn=buf.Pour(str.substr(0,str.size()-1));
    ASSERT_EQ(rtn.size(),0);
    rtn=buf.Pour(str.substr(str.size()-1));
    ASSERT_EQ(rtn.size(),1);
    EXPECT_EQ(rtn[0],str);
}

TEST_F(FooTest,TwoStringsATime ) {
    Buffer buf;
    vector<string> strs;
    string str;
    vector<string> rtn;
    strs={"123","345"};
    str=AppMessage(1,strs);
    rtn=buf.Pour(str+str);

    ASSERT_EQ(rtn.size(),2);
    EXPECT_EQ(rtn[0],str);
    EXPECT_EQ(rtn[1],str);
}

TEST_F(FooTest,TwoStringsTwoTime ) {
    Buffer buf;
    vector<string> strs;
    string str;
    vector<string> rtn;
    strs={"123","345"};
    str=AppMessage(1,strs);
    rtn=buf.Pour(str.substr(0,4));
    ASSERT_EQ(rtn.size(),0);


    rtn=buf.Pour(str.substr(4)+str);
    ASSERT_EQ(rtn.size(),2);
    EXPECT_EQ(rtn[0],str);
    EXPECT_EQ(rtn[1],str);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
