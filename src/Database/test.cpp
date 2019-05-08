/*
 * To Implement :
 *  User quit room check
 */
#include <set>
#include "database.hpp"
#include "gtest/gtest.h"

inline bool operator ==(const Data_user& acc1, const Data_user& acc2)
{
    return (acc1.account==acc2.account)&&(acc1.passwd==acc2.passwd)&&(acc1.nickname==acc2.nickname);
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
TEST_F(FooTest, GetUser) {
    Database data;
    Account acc="12345676";
    Data_user user;
    EXPECT_EQ(data.GetUser(acc,user),::Database::USER_NOT_FOUND);

}

// Tests that Foo does Xyz.
TEST_F(FooTest, AddUser) {
  // Exercises the Xyz feature of Foo.
    Database base;
    Data_user user{"20171402120","qwerfdsa","pzy"};
    Data_user rtn;
    EXPECT_EQ(base.AddUser(user),::Database::SUCCESS);
    EXPECT_EQ(base.GetUser(user.account,rtn),::Database::SUCCESS);
    EXPECT_EQ(rtn,user);
}

TEST_F(FooTest, GetAllUsers)
{
    Database base;
    Data_user user0{"20171402120","qwerfdsa","pzy"};
    EXPECT_EQ(base.AddUser(user0),::Database::SUCCESS);
    std::vector<Data_user> users;
    EXPECT_EQ(base.GetAllUsers(users),::Database::SUCCESS);
    EXPECT_EQ(users.size(),1);
    EXPECT_EQ(users[0],user0);

    Data_user user1{"20171402121","qwerfdsag","pzk"};
    EXPECT_EQ(base.AddUser(user1),::Database::SUCCESS);

    EXPECT_EQ(base.GetAllUsers(users),::Database::SUCCESS);
    EXPECT_EQ(users.size(),2);
    EXPECT_EQ(users[0],user0);
    EXPECT_EQ(users[1],user1);
}

/*
 * less room check for userquit
 */
TEST_F(FooTest, UserQuit)
{
    Database base;
    Data_user user0{"20171402120","qwerfdsa","pzy"};
    EXPECT_EQ(base.AddUser(user0),::Database::SUCCESS);
    std::vector<Data_user> users;
    EXPECT_EQ(base.GetAllUsers(users),::Database::SUCCESS);
    EXPECT_EQ(users.size(),1);
    EXPECT_EQ(users[0],user0);

    Data_user user1{"20171402121","qwerfdsag","pzk"};
    EXPECT_EQ(base.AddUser(user1),::Database::SUCCESS);

    EXPECT_EQ(base.GetAllUsers(users),::Database::SUCCESS);
    EXPECT_EQ(users.size(),2);
    EXPECT_EQ(users[0],user0);
    EXPECT_EQ(users[1],user1);

    EXPECT_EQ(base.UserQuit(user0.account),base.SUCCESS);

    EXPECT_EQ(base.GetAllUsers(users),::Database::SUCCESS);
    EXPECT_EQ(users.size(),1);
    EXPECT_EQ(users[0],user1);
}

/*
 *
 */
TEST_F(FooTest, Rooms)
{
    Database base;
    Data_room room{"1234567","Love","Description","12345","20171402121",{},{}};
    Data_room room0;
    unsigned int status;
    EXPECT_EQ((status=base.AddRoom(room)),base.USER_NOT_FOUND);
    EXPECT_EQ(base.GetRoom(room.id,room0),base.ROOM_NOT_FOUND);

    Data_user user0{"20171402120","qwerfdsa","pzy"};
    Data_user user1{"20171402121","qwerfdsag","pzk"};
    //std::vector<Data_user> users;

    EXPECT_EQ(base.AddUser(user0),::Database::SUCCESS);
    EXPECT_EQ(base.AddUser(user1),::Database::SUCCESS);
    //EXPECT_EQ(base.GetAllUsers(users),::Database::SUCCESS);

    EXPECT_EQ(base.AddRoom(room),base.SUCCESS);
    EXPECT_EQ(base.GetRoom(room.id,room0),base.SUCCESS);

    EXPECT_EQ(room.id,room0.id);
    EXPECT_EQ(room.name,room0.name);
    EXPECT_EQ(room.description,room0.description);
    EXPECT_EQ(room.passwd,room0.passwd);

    EXPECT_EQ(room0.admin,user1.account);
    EXPECT_EQ(base.UserChangeRoomAdmin(user0.account,room.id,user1.account),base.USER_NOT_ADMIN);
    EXPECT_EQ(base.UserChangeRoomAdmin(user1.account,room.id,user0.account),base.SUCCESS);

    EXPECT_EQ(base.GetRoom(room.id,room0),base.SUCCESS);
    EXPECT_EQ(room0.admin,user0.account);

    string str="291231";
    EXPECT_EQ(base.CloseRoom(user0.account,str),base.ROOM_NOT_FOUND);
    EXPECT_EQ(base.CloseRoom(user1.account,room0.id),base.USER_NOT_ADMIN);
    EXPECT_EQ(base.CloseRoom(user0.account,room0.id),base.SUCCESS);
    EXPECT_EQ(base.GetRoom(room.id,room0),base.ROOM_NOT_FOUND);

    EXPECT_EQ(base.AddRoom(room),base.SUCCESS);
    room.id="2345678";
    room.name="Jack";
    EXPECT_EQ(base.AddRoom(room),base.SUCCESS);

    EXPECT_EQ(base.UserJoinRoom(user1.account,room.id),base.ADMIN_CANNOT_BE_MEMBER);
    EXPECT_EQ(base.UserJoinRoom(user0.account,room.id),base.SUCCESS);

    std::vector<Data_user> users;
    EXPECT_EQ(base.GetRoom(room.id,room0),base.SUCCESS);
    EXPECT_EQ(room0.members.size(),1);
    EXPECT_EQ(room0.members[0],user0.account);

    EXPECT_EQ(base.GetUsersInARoom(room0.id,users),base.SUCCESS);
    EXPECT_EQ(users.size(),2);
    EXPECT_EQ(users[0],user0);

    EXPECT_EQ(base.OneUserLeaveRoom(user1.account,room0.id),base.ADMIN_NOT_ALLOWED_LEAVE);
    EXPECT_EQ(base.OneUserLeaveRoom(user0.account,room0.id),base.SUCCESS);
    EXPECT_EQ(base.GetRoom(room.id,room0),base.SUCCESS);
    EXPECT_EQ(room0.members.size(),0);

}  // namespace

TEST_F(FooTest, KickBlacklist)
{
    Database base;
    Data_room room{"1234567","Love","Description","12345","20171402121",{},{}};
    Data_room room0;
    unsigned int status;

    Data_user user0{"20171402120","qwerfdsa","pzy"};
    Data_user user1{"20171402121","qwerfdsag","pzk"};

    EXPECT_EQ(base.AddUser(user0),::Database::SUCCESS);
    EXPECT_EQ(base.AddUser(user1),::Database::SUCCESS);

    EXPECT_EQ(base.AddRoom(room),base.SUCCESS);
    EXPECT_EQ(base.UserJoinRoom(user0.account,room.id),base.SUCCESS);

    EXPECT_EQ(base.KickUserOut(user0.account,room.id,user1.account),base.USER_NOT_ADMIN);
    EXPECT_EQ(base.KickUserOut(user1.account,room.id,user1.account),base.USER_NOT_FOUND);
    EXPECT_EQ(base.KickUserOut(user1.account,room.id,user0.account),base.SUCCESS);
    EXPECT_EQ(base.UserJoinRoom(user0.account,room.id),base.BLACKLISTS);

    EXPECT_EQ(base.GetRoom(room.id,room0),base.SUCCESS);
    EXPECT_EQ(room0.members.size(),0);
}

TEST_F(FooTest,SaveLoad)
{
    Database base;
    Data_room room{"1234567","Love","Description","12345","20171402121",{},{}};
    Data_room room0;
    unsigned int status;
    EXPECT_EQ((status=base.AddRoom(room)),base.USER_NOT_FOUND);
    EXPECT_EQ(base.GetRoom(room.id,room0),base.ROOM_NOT_FOUND);

    Data_user user0{"20171402120","qwerfdsa","pzy"};
    Data_user user1{"20171402121","qwerfdsag","pzk"};
    //std::vector<Data_user> users;

    EXPECT_EQ(base.AddUser(user0),::Database::SUCCESS);
    EXPECT_EQ(base.AddUser(user1),::Database::SUCCESS);
    //EXPECT_EQ(base.GetAllUsers(users),::Database::SUCCESS);
    string sav="database.sav";
    base.Save(sav.c_str());
    Database base1(sav.c_str());
    EXPECT_EQ(base1.AddRoom(room),base1.SUCCESS);
    EXPECT_EQ(base1.GetRoom(room.id,room0),base1.SUCCESS);

    EXPECT_EQ(room.id,room0.id);
    EXPECT_EQ(room.name,room0.name);
    EXPECT_EQ(room.description,room0.description);
    EXPECT_EQ(room.passwd,room0.passwd);

    EXPECT_EQ(room0.admin,user1.account);
    EXPECT_EQ(base1.UserChangeRoomAdmin(user0.account,room.id,user1.account),base1.USER_NOT_ADMIN);
    EXPECT_EQ(base1.UserChangeRoomAdmin(user1.account,room.id,user0.account),base1.SUCCESS);

    EXPECT_EQ(base1.GetRoom(room.id,room0),base1.SUCCESS);
    EXPECT_EQ(room0.admin,user0.account);

    string str="291231";
    EXPECT_EQ(base1.CloseRoom(user0.account,str),base1.ROOM_NOT_FOUND);
    EXPECT_EQ(base1.CloseRoom(user1.account,room0.id),base1.USER_NOT_ADMIN);
    EXPECT_EQ(base1.CloseRoom(user0.account,room0.id),base1.SUCCESS);

    EXPECT_EQ(base1.AddRoom(room),base1.SUCCESS);
    room.id="2345678";
    room.name="Jack";
    EXPECT_EQ(base1.AddRoom(room),base1.SUCCESS);
    EXPECT_EQ(base1.UserJoinRoom(user0.account,room.id),base1.SUCCESS);

    base1.Save(sav.c_str());

    Database base2;
    EXPECT_EQ(base2.Load(sav.c_str()),base.SUCCESS);
    std::vector<Data_user> users;

    EXPECT_EQ(base2.GetRoom(room.id,room0),base1.SUCCESS);
    EXPECT_EQ(base2.GetUsersInARoom(room0.id,users),base2.SUCCESS);


}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
