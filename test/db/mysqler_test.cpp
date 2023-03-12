/**
 * @file mysqler_test.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Mar 12 2023
 *
 * This is the unit test file for db/mysqler class
 */

#include "db/mysqler.h"
#include "catch2/catch_test_macros.hpp"

/* for convenience reason */
using TURTLE_SERVER::DB::MySqler;

/*
 * Make sure you have already executed the mysql setup script under setup/setup.sql
 * by terminal command 'sudo mysql < setup.sql' for example
 */
TEST_CASE("[db/mysqler]") {
  SECTION("mysqler read") {
    auto mysqler = MySqler("127.0.0.1", 3306, "tester", "tester", "test_db");
    CHECK(mysqler.IsValid());
    std::string command = "SELECT username, passwd FROM user;";
    auto res1 = mysqler.ExecuteQueryBlocking(command);
    auto res2 = mysqler.ExecuteQueryNonBlocking(command).get();
    CHECK(res1->rowsCount() == 3);
    CHECK(res2->rowsCount() == 3);
    std::vector<std::string> usernames{"Jason", "Tommy", "Mary"};
    std::vector<std::string> passwds{"123", "456", "789"};
    auto res_list = std::vector<sql::ResultSet *>{res1.get(), res2.get()};
    for (auto res : res_list) {
      int count = 0;
      while (res->next()) {
        CHECK(res->getString("username") == usernames[count]);
        CHECK(res->getString("passwd") == passwds[count]);
        count++;
      }
    }
  }

  /* this may have side effect if failed halfway, need to re-execute setup.sql if so */
  SECTION("mysqler insert") {
    auto mysqler = MySqler("127.0.0.1", 3306, "tester", "tester", "test_db");
    CHECK(mysqler.IsValid());
    std::string command_insert = "INSERT INTO user (username, passwd) VALUES (\"someone\", \"123\");";
    mysqler.ExecuteBlocking(command_insert);
    std::string command_query = "SELECT username, passwd FROM user;";
    auto res = mysqler.ExecuteQueryBlocking(command_query);
    CHECK(res->rowsCount() == 4);
  }

  /* this may have side effect if failed halfway, need to re-execute setup.sql if so */
  SECTION("mysqler delete") {
    auto mysqler = MySqler("127.0.0.1", 3306, "tester", "tester", "test_db");
    CHECK(mysqler.IsValid());
    std::string command_delete = "DELETE FROM user where username=\"someone\";";
    mysqler.ExecuteBlocking(command_delete);
    std::string command_query = "SELECT username, passwd FROM user;";
    auto res = mysqler.ExecuteQueryBlocking(command_query);
    CHECK(res->rowsCount() == 3);
  }
}
