/**
 * @file dict_client.cpp
 * @author Yukun J
 * @expectation this is the demo kvstore client for illustration and test purpose
 * @init_date Jun 25 2023
 */

#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

#include <memory>

#include "core/connection.h"
#include "core/net_address.h"
#include "core/socket.h"
#include "core/thread_pool.h"

#define BUF_SIZE 2048

namespace TURTLE_SERVER {
class DictClient {
 public:
  explicit DictClient(NetAddress server_address) {
    auto client_socket = std::make_unique<Socket>();
    client_socket->Connect(server_address);
    client_connection = std::make_unique<Connection>(std::move(client_socket));
    std::cout << "DictClient: Connected with " << server_address.ToString() << std::endl;
  }

  void Begin() {
    Usage();
    char buf[BUF_SIZE + 1];
    memset(buf, 0, sizeof(buf));
    int fd = client_connection->GetFd();
    while (true) {
      // scan from user keyboard
      auto actual_read = read(STDIN_FILENO, buf, BUF_SIZE);
      if (strncmp(buf, "quit", strlen("quit")) == 0 || strncmp(buf, "q", strlen("q")) == 0) {
        // client indicates to exit
        exit(0);
      }
      send(fd, buf, actual_read, 0);
      memset(buf, 0, sizeof(buf));
      // blocking wait for server's response
      auto actual_recv = recv(fd, buf, BUF_SIZE, 0);
      write(STDOUT_FILENO, buf, actual_recv);
      memset(buf, 0, sizeof(buf));
    }
  }

  void Usage() {
    std::cout << "Usage:\n"
              << "\tGET [key]\n"
              << "\tSET [key] [value]\n"
              << "\tDEL [key]\n"
              << "\tquit(q)\n\n";
  }

 private:
  std::unique_ptr<Connection> client_connection;
};
}  // namespace TURTLE_SERVER

int main() {
  TURTLE_SERVER::NetAddress local_address("0.0.0.0", 20080);
  TURTLE_SERVER::DictClient dict_client(local_address);
  dict_client.Begin();

  return 0;
}
