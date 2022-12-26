/**
 * @file echo_server.cpp
 * @author Yukun J
 * @expectation this is the demo echo server for illustration and test purpose
 * @init_date Dec 26 2022
 */
#include <functional>
#include <iostream>
#include <memory>

#include "buffer.h"
#include "connection.h"
#include "looper.h"
#include "net_address.h"
#include "poller.h"
#include "socket.h"
#include "thread_pool.h"
#include "utils.h"

#define BUF_SIZE 2048
namespace TURTLE_SERVER {

void HandleConnection(Connection* client_conn) {
  int from_fd = client_conn->GetFd();
  char buf[BUF_SIZE + 1];
  memset(buf, 0, sizeof(buf));

  // read all available bytes
  ssize_t read = 0;
  while (true) {
    ssize_t curr_read = recv(from_fd, buf, BUF_SIZE, 0);
    if (curr_read > 0) {
      read += curr_read;
      client_conn->WriteToReadBuffer(buf, curr_read);
      memset(buf, 0, sizeof(buf));
    } else if (curr_read == 0) {
      // the client has exit
      client_conn->GetLooper()->DeleteConnection(from_fd);
      std::cout << "Client exits: " << from_fd << std::endl;
      break;
    } else if (curr_read == -1 && errno == EINTR) {
      // normal interrupt
      continue;
    } else if (curr_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      // all data read
      break;
    } else {
      perror("HandleConnection: recv() error");
      exit(EXIT_FAILURE);
    }
  }

  // echo back
  std::cout << "Client " << from_fd << " : " << client_conn->ReadAsString();
  client_conn->WriteToWriteBuffer(client_conn->ReadAsString());
  client_conn->Send();
  client_conn->ClearReadBuffer();
}

void AcceptNewConnection(Connection* server_conn) {
  NetAddress client_address;
  int accept_fd = server_conn->GetSocket()->Accept(client_address);
  std::cout << "New client joins: " << accept_fd << std::endl;
  auto client_sock = std::make_unique<Socket>(accept_fd);
  client_sock->SetNonBlocking();
  auto client_connection = std::make_unique<Connection>(
      server_conn->GetLooper(), std::move(client_sock));
  client_connection->SetEvents(EPOLLIN | EPOLLET);  // edge-trigger for client
  client_connection->SetCallback(HandleConnection);
  server_conn->GetLooper()->AddConnection(std::move(client_connection));
}

class EchoServer {
 public:
  explicit EchoServer(NetAddress server_address)
      : pool_(std::make_unique<ThreadPool>()),
        looper_(std::make_unique<Looper>(pool_.get())) {
    auto server_sock = std::make_unique<Socket>();
    server_sock->SetReusable();
    server_sock->Bind(server_address);
    server_sock->Listen();
    auto server_connection =
        std::make_unique<Connection>(looper_.get(), std::move(server_sock));
    server_connection->SetEvents(EPOLLIN);  // not edge-trigger for listener
    server_connection->SetCallback(AcceptNewConnection);
    looper_->AddConnection(std::move(server_connection));
  }

  void Begin() { looper_->Loop(); }

 private:
  std::unique_ptr<ThreadPool> pool_;
  std::unique_ptr<Looper> looper_;
};
}  // namespace TURTLE_SERVER

int main() {
  TURTLE_SERVER::NetAddress local_address("0.0.0.0", 20080);
  TURTLE_SERVER::EchoServer echo_server(local_address);
  echo_server.Begin();
  return 0;
}