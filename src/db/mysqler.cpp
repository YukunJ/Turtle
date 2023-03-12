/**
 * @file mysqler.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Mar 11 2023
 *
 * This is an implementation file implementing the mysql connection and query
 * execution It assumes some pre-installment and setup on the localhost's mysql
 * database
 */

#include <cppconn/exception.h>
#include <cppconn/statement.h>

#include "db/mysqler.h"

namespace TURTLE_SERVER::DB {

/*
 * Simple SQL Exception Logging
 * Outside try-catch block should rethrow
 */
void LogMySqlError(const sql::SQLException &e) {
  using std::cerr, std::endl;
  cerr << "# ERR: " << e.what() << endl;
  cerr << " (MySQL error code: " << e.getErrorCode();
  cerr << ", SQLState: " << e.getSQLState() << " )" << endl;
}

std::string ConcatenateAddress(const std::string &protocol, const std::string &network_addr, int port) {
  return protocol + "://" + network_addr + ":" + std::to_string(port);
}

MySqler::MySqler(const std::string &db_address, int db_port, const std::string &user, const std::string &password,
                 const std::string &db_name) {
  try {
    driver_ = get_driver_instance();
    std::string address = ConcatenateAddress(TCP_PROTOCOL, db_address, db_port);
    conn_ = std::unique_ptr<sql::Connection>(driver_->connect(address, user, password));
    conn_->setSchema(db_name);  // choose a specific database to stick this connection with
    valid_ = true;
  } catch (sql::SQLException &e) {
    LogMySqlError(e);
    throw;
  }
}

MySqler::MySqler(MySqler &&rhs) noexcept {
  driver_ = rhs.driver_;
  rhs.driver_ = nullptr;
  conn_ = std::move(rhs.conn_);
  rhs.valid_ = false;
}

auto MySqler::operator=(MySqler &&rhs) noexcept -> MySqler & {
  driver_ = rhs.driver_;
  rhs.driver_ = nullptr;
  conn_.reset(rhs.conn_.release());
  valid_ = rhs.valid_;
  return *this;
}

auto MySqler::ExecuteQueryBlocking(const std::string &command) -> std::unique_ptr<sql::ResultSet> {
  try {
    auto stmt = std::unique_ptr<sql::Statement>(conn_->createStatement());
    return std::unique_ptr<sql::ResultSet>(stmt->executeQuery(command));
  } catch (sql::SQLException &e) {
    LogMySqlError(e);
    throw;
  }
}

auto MySqler::ExecuteQueryNonBlocking(const std::string &command) -> std::future<std::unique_ptr<sql::ResultSet>> {
  return (std::async([&] { return ExecuteQueryBlocking(command); }));
}

auto MySqler::ExecuteBlocking(const std::string &command) -> bool {
  try {
    auto stmt = std::unique_ptr<sql::Statement>(conn_->createStatement());
    return stmt->execute(command);
  } catch (sql::SQLException &e) {
    LogMySqlError(e);
    throw;
  }
}

auto MySqler::ExecuteNonBlocking(const std::string &command) -> std::future<bool> {
  return (std::async([&] { return ExecuteBlocking(command); }));
}

auto MySqler::IsValid() -> bool { return valid_; }

}  // namespace TURTLE_SERVER::DB
