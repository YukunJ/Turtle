/**
 * @file mysqler.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Mar 11 2023
 *
 * This is an implementation file implementing the mysql connection and query execution
 * It assumes some pre-installment and setup on the localhost's mysql database
 */

#include "db/mysqler.h"

/* Standard C++ includes */
#include <stdlib.h>
/*GRANT ALL PRIVILEGES ON test_db.user TO 'tester'@'localhost'; */
/*g++ -Wall -I/usr/include/cppconn -o example example.cpp -L/usr/lib -lmysqlcppconn */
/*g++ -Wall  -o example example.cpp -lmysqlcppconn */
#include <iostream>
#include <memory>

/*
  Include directly the different
  headers from cppconn/ and mysql_driver.h + mysql_util.h
  (and mysql_connection.h). This will reduce your build time!
*/
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include "mysql_connection.h"

using namespace std;

int main(void) {
    try {
        sql::Driver *driver;
        sql::Connection *con;
        sql::Statement *stmt;
        sql::ResultSet *res;

        /* Create a connection */
        driver = get_driver_instance();
        con = driver->connect("tcp://127.0.0.1:3306", "tester", "tester");
        /* Connect to the MySQL test database */
        con->setSchema("test_db");

        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT username, password from USER");
        while (res->next()) {
            cout << "\t... MySQL replies: ";
            /* Access column data by alias or column name */
            cout << "User:" << res->getString("username") << endl;
            cout << "Password:" << res->getString("password") << endl;
        }
        std::unique_ptr<sql::ResultSet> set_mem(stmt->executeQuery("SELECT username, password from USER"));
        while (set_mem->next()) {
            cout << "\t... MySQL replies: ";
            /* Access column data by alias or column name */
            cout << "User:" << set_mem->getString("username") << endl;
            cout << "Password:" << set_mem->getString("password") << endl;
        }
        delete res;
        delete stmt;
        delete con;

    } catch (sql::SQLException &e) {
        cout << "# ERR: SQLException in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }

    cout << endl;

    return EXIT_SUCCESS;
}
