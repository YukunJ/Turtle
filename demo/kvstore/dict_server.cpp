/**
 * @file dict_server.cpp
 * @author Yukun J
 * @expectation this is the demo kvstore dict server for illustration and test purpose
 * @init_date Jun 25 2023
 */

#include <map>
#include <set>
#include <string>
#include <vector>
#include "core/turtle_server.h"
#include "http/http_utils.h"

static const std::set<std::string> VALID_METHOD = {"GET", "SET", "DEL"};
static std::map<std::string, std::string> STORE;
static const std::string ERR_MSG = "Command Error: [GET,SET,DEL]\n";
static const std::string OK_MSG = "OK.\n";
static const std::string NULL_MSG = "(nil)\n";
static const std::string NEW_LINE = "\n";

auto ProcessQuery(const std::string &query) -> std::string {
  using TURTLE_SERVER::HTTP::Split;
  using TURTLE_SERVER::HTTP::Trim;
  std::vector<std::string> tokens = Split(query);
  int tokens_size = tokens.size();
  if (tokens.empty() || VALID_METHOD.find(tokens[0]) == VALID_METHOD.end()) {
    return ERR_MSG;
  }
  std::string method = tokens[0];
  if (method == "GET") {
    if (tokens_size != 2) {
      return "Argument Length Error: GET [key]\n";
    }
    std::string key = Trim(tokens[1], "\n");
    if (STORE.find(key) == STORE.end()) {
      return NULL_MSG;
    }
    return STORE[key] + NEW_LINE;
  }

  if (method == "SET") {
    if (tokens_size != 3) {
      return "Argument Length Error: SET [key] [val]\n";
    }
    STORE[tokens[1]] = Trim(tokens[2], "\n");
    return OK_MSG;
  }

  if (method == "DEL") {
    if (tokens_size != 2) {
      return "Argument Length Error: DEL [key]\n";
    }
    std::string key = Trim(tokens[1], "\n");
    if (STORE.find(key) == STORE.end()) {
      return NULL_MSG;
    }
    STORE.erase(key);
    return OK_MSG;
  }

  return "Error: Unknown Command " + method + "\n";
}

int main() {
  TURTLE_SERVER::NetAddress local_address("0.0.0.0", 20080);
  TURTLE_SERVER::TurtleServer dict_server(local_address);
  dict_server
      .OnHandle([&](TURTLE_SERVER::Connection *client_conn) {
        int from_fd = client_conn->GetFd();
        auto [read, exit] = client_conn->Recv();
        if (exit) {
          client_conn->GetLooper()->DeleteConnection(from_fd);
          // client_conn ptr is invalid below here, do not touch it again
          return;
        }
        if (read) {
          auto query_option = client_conn->FindAndPopTill("\n");
          if (query_option.has_value()) {
            client_conn->WriteToWriteBuffer(ProcessQuery(query_option.value()));
            client_conn->Send();
          }
        }
      })
      .Begin();
  return 0;
}
