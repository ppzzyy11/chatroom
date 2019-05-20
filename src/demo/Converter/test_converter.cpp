/*
 */
#include <set>
#include "converter.hpp"
#include "gtest/gtest.h"

using namespace std;

inline bool operator ==(const Data_user& acc1, const Data_user& acc2)
{
    return (acc1.account==acc2.account)&&(acc1.passwd==acc2.passwd)&&(acc1.nickname==acc2.nickname);
}
ostream& operator<<(ostream& os, const vector<pair<string,string>>& rtn)
{
    for(auto tmp:rtn)
    {
        cout<<tmp.first<<'\n'<<tmp.second<<endl<<endl;
        cout<<'\n';
    }
    return os;
}

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

// Tests that the Foo::Bar() method does Abc.
TEST_F(FooTest,ConAcceptAndConError ) {
    Converter converter;
    void* p0=reinterpret_cast<void*>(1234);
    void* p1=reinterpret_cast<void*>(5678);
    EXPECT_EQ(converter.ConError(p0),-1);
    EXPECT_EQ(converter.ConAccept(p0),0);
    EXPECT_EQ(converter.ConError(p1),-1);
    EXPECT_EQ(converter.ConError(p0),0);
}

TEST_F(FooTest,aa)
{

}



int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
