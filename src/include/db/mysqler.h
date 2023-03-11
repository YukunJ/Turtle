/**
 * @file mysqler.h
 * @author Yukun J
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Mar 11 2023
 *
 * This is a header file implementing the mysql connection and query execution
 * It assumes some pre-installment and setup on the localhost's mysql database
 */
#ifndef SRC_INCLUDE_DB_MYSQLER_H_
#define SRC_INCLUDE_DB_MYSQLER_H_


#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include <memory>
#include <future>  // NOLINT
#include <string>
#include <iostream>

#include "mysql_connection.h"

#include "core/utils.h"

namespace TURTLE_DB {

/**
 * Each MySqler is a single mysql connection focused on one database for a given server addr + port + user
 * Query execution might throw SQL exception to be handled by caller
 *
 */
class MySqler{
 public:
    /* upon ctor, connect and create a live connection */
    explicit MySqler(std::string db_address = "127.0.0.1", int db_port = 3306, std::string user = "root",
                     std::string password = "root", std::string db_name = "test_db") throw(sql::SQLException);

    ~MySqler() = default;

    NON_COPYABLE(MySqler);

    /* support move */
    MySqler(MySqler&& rhs);
    MySqler& operator=(MySqler&& rhs);

    /* blocking */
    auto ExecuteBlocking(const std::string& command) throw(sql::SQLException) -> std::unique_ptr<sql::ResultSet>;

    /* non-blocking */
    auto ExecuteNonBlocking(const std::string& command) throw(sql::SQLException)
        -> std::future<std::unique_ptr<sql::ResultSet>>;

 private:
    /* one driver per MySqler lifecycle */
    std::unique_ptr<sql::Driver> driver_;
    std::unique_ptr<sql::Connection> conn_;
};

}  // namespace TURTLE_DB

#endif  // SRC_INCLUDE_DB_MYSQLER_H_
