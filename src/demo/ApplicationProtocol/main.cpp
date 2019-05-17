/*
 */
#include <set>
#include "applicationprotocol.hpp"
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
TEST_F(FooTest,REGISTER ) {

    ApplicationProtocol ap;
    vector<string> strs={"2017140212","1234567","JackMa"};
    string str=AppMessage(1,strs);

    //Exec(string command)
    vector<pair<string,string>> rtn=ap.Exec(str);

    EXPECT_EQ(rtn[0].first,"2017140212");
    EXPECT_EQ(rtn[0].second,"SUCCESS");
}

TEST_F(FooTest,Room)
{
    ApplicationProtocol ap;
    vector<string> strs;
    vector<pair<string,string>> rtn;
    string str;

    //add user0
    strs={"2017140212","1234567","JackMa"};
    str=AppMessage(1,strs);
    rtn=ap.Exec(str);

    //add user1
    strs={"1234567890","wobugaosuni","Pzk"};
    str=AppMessage(1,strs);
    rtn=ap.Exec(str);

    //add room0
    strs={"1234567890","10570","RoomOfLove","A wonderful place where you should spend time with your lover.","love"};
    str=AppMessage(5,strs);
    rtn=ap.Exec(str);


    //add room1
    strs={"2017140212","64565","Hell","A hell filled with liers.","Fraud"};
    str=AppMessage(5,strs);
    rtn=ap.Exec(str);


    //get all rooms
    strs={"2017140212"};
    str=AppMessage(4,strs);
    rtn=ap.Exec(str);
    cout<<rtn<<endl;

    //get room0
    strs={"2017140212","10570"};
    str=AppMessage(16,strs);
    rtn=ap.Exec(str);
    cout<<rtn<<endl;

}



int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
