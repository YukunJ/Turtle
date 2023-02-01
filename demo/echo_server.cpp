/**
 * @file echo_server.cpp
 * @author Yukun J
 * @expectation this is the demo echo server for illustration and test purpose
 * @init_date Dec 26 2022
 */

#include "core/turtle_server.h"

int main() {
  TURTLE_SERVER::NetAddress local_address("0.0.0.0", 20080);
  TURTLE_SERVER::TurtleServer echo_server(local_address);
  echo_server
      .OnHandle([&](TURTLE_SERVER::Connection* client_conn) {
        int from_fd = client_conn->GetFd();
        auto [read, exit] = client_conn->Recv();
        if (exit) {
          client_conn->GetLooper()->DeleteConnection(from_fd);
          // client_conn ptr is invalid below here, do not touch it again
          return;
        }
        if (read) {
          client_conn->WriteToWriteBuffer(client_conn->ReadAsString());
          client_conn->Send();
          client_conn->ClearReadBuffer();
        }
      })
      .Begin();
  return 0;
}
