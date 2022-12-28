/**
 * @file echo_server.cpp
 * @author Yukun J
 * @expectation this is the demo echo server for illustration and test purpose
 * @init_date Dec 26 2022
 */

#include "turtle_server.h"

namespace TURTLE_SERVER {

class EchoServer : public TurtleServer {
 public:
  explicit EchoServer(NetAddress server_address)
      : TurtleServer(server_address) {}

  void OnAccept(Connection *server_conn) final {}

  void OnHandle(Connection *client_conn) final {
    int from_fd = client_conn->GetFd();
    auto [read, exit] = client_conn->Recv();
    if (exit) {
      client_conn->GetLooper()->DeleteConnection(from_fd);
      // client_conn ptr is destoryed and invalid below here, do not touch it
      // again
      return;
    }
    if (read) {
      client_conn->WriteToWriteBuffer(client_conn->ReadAsString());
      client_conn->Send();
      client_conn->ClearReadBuffer();
    }
  }
};

}  // namespace TURTLE_SERVER

int main() {
  TURTLE_SERVER::NetAddress local_address("0.0.0.0", 20080);
  TURTLE_SERVER::EchoServer echo_server(local_address);
  echo_server.Begin();
  return 0;
}
