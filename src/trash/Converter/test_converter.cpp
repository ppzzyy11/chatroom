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

void str2vec(const string str,vector<char>& chs)
{
    chs.clear();
    for(auto ch:str)
    {
        chs.push_back(ch);
    }
    return;
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

TEST_F(FooTest,Register)
{
    Converter con;
    string rtn;
    vector<string> strs;
    vector<char> chs;
    vector<pair<void*,string>> pars;
    void* p0=reinterpret_cast<void*>(1234);
    EXPECT_EQ(con.ConAccept(p0),0);
    strs={"2017140212","1234567890","puzeyu"};
    str2vec(AppMessage(1,strs),chs);

    pars=con.ConRead(p0,chs);

    ASSERT_EQ(pars.size(),1);
    EXPECT_EQ(pars[0].first,p0);
    EXPECT_EQ(pars[0].second,"SUCCESS");

}

TEST_F(FooTest,Login)
{
    Converter con;
    string rtn;
    vector<string> strs;
    vector<char> chs;
    vector<pair<void*,string>> pars;
    void* p0=reinterpret_cast<void*>(1234);
    EXPECT_EQ(con.ConAccept(p0),0);
    strs={"2017140212","1234567890","puzeyu"};
    str2vec(AppMessage(1,strs),chs);
    pars=con.ConRead(p0,chs);

    ASSERT_EQ(pars.size(),1);
    EXPECT_EQ(pars[0].first,p0);
    EXPECT_EQ(pars[0].second,"SUCCESS");

    strs={"2017140212","1234567890"};
    str2vec(AppMessage(2,strs),chs);
    pars=con.ConRead(p0,chs);
    ASSERT_EQ(pars.size(),1);
    EXPECT_EQ(pars[0].first,p0);
    EXPECT_EQ(pars[0].second,"SUCCESS");
}

TEST_F(FooTest,TwoLoginPrivateMessage)
{
    Converter con;
    string rtn;
    vector<string> strs;
    vector<char> chs;
    vector<pair<void*,string>> pars;
    //p0 login head
    void* p0=reinterpret_cast<void*>(1234);
    EXPECT_EQ(con.ConAccept(p0),0);
    strs={"2017140212","1234567890","puzeyu"};
    str2vec(AppMessage(1,strs),chs);
    pars=con.ConRead(p0,chs);

    ASSERT_EQ(pars.size(),1);
    EXPECT_EQ(pars[0].first,p0);
    EXPECT_EQ(pars[0].second,"SUCCESS");

    strs={"2017140212","1234567890"};
    str2vec(AppMessage(2,strs),chs);
    pars=con.ConRead(p0,chs);
    ASSERT_EQ(pars.size(),1);
    EXPECT_EQ(pars[0].first,p0);
    EXPECT_EQ(pars[0].second,"SUCCESS");
    //p0 login tail

    //p1 login head
    void* p1=reinterpret_cast<void*>(7890);
    EXPECT_EQ(con.ConAccept(p1),0);
    strs={"1234567890","qwerfdsa","xiaoJJ"};
    str2vec(AppMessage(1,strs),chs);
    pars=con.ConRead(p1,chs);

    ASSERT_EQ(pars.size(),1);
    EXPECT_EQ(pars[0].first,p1);
    EXPECT_EQ(pars[0].second,"SUCCESS");

    strs={"1234567890","qwerfdsa"};
    str2vec(AppMessage(2,strs),chs);
    pars=con.ConRead(p1,chs);
    ASSERT_EQ(pars.size(),1);
    EXPECT_EQ(pars[0].first,p1);
    EXPECT_EQ(pars[0].second,"SUCCESS");
    //p1 login tail


    //p0 Build Room
    strs={"2017140212","00001","RoomOfLove","A room fufilled with lover.","12345"};
    str2vec(AppMessage(5,strs),chs);
    pars=con.ConRead(p0,chs);
    ASSERT_EQ(pars.size(),1);
    EXPECT_EQ(pars[0].first,p0);
    EXPECT_EQ(pars[0].second,"SUCCESS");

    //p1 Get Rooms
    strs={"1234567890"};
    str2vec(AppMessage(4,strs),chs);
    pars=con.ConRead(p1,chs);
    ASSERT_EQ(pars.size(),1);
    EXPECT_EQ(pars[0].first,p1);
    cout<<pars[0].second;
    strs={"1234567890","00001","12345"};
    str2vec(AppMessage(8,strs),chs);
    pars=con.ConRead(p1,chs);
    ASSERT_EQ(pars.size(),3);
    for(auto par:pars)
    {
        cout<<"bev*:"<<par.first<<"\tmsg:"<<par.second<<"\n";
    }

    //p1 send a messages
    strs={"1234567890","00001","I like viri"};
    str2vec(AppMessage(10,strs),chs);
    pars=con.ConRead(p1,chs);
    for(auto par:pars)
    {
        cout<<"bev*:"<<par.first<<"\tmsg:"<<par.second<<"\n";
    }

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
