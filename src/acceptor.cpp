/**
 * @file acceptor.cpp
 * @author Yukun J
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Jan 03 2023
 *
 * This is an implementation file implementing the Acceptor that accepts all the
 * incoming new client connections and set up customer handle functions for new
 * clients
 */
#include "acceptor.h"

#include <cstdlib>
#include <utility>

#include "connection.h"
#include "looper.h"
#include "net_address.h"
#include "poller.h"
#include "socket.h"

namespace TURTLE_SERVER {

Acceptor::Acceptor(Looper *listener, std::vector<Looper *> reactors,
                   NetAddress server_address)
    : reactors_(std::move(reactors)) {
  auto acceptor_sock = std::make_unique<Socket>();
  acceptor_sock->Bind(server_address, true);
  acceptor_sock->Listen();
  acceptor_conn = std::make_unique<Connection>(std::move(acceptor_sock));
  acceptor_conn->SetEvents(POLL_ADD);  // not edge-trigger for listener
  acceptor_conn->SetLooper(listener);
  listener->AddAcceptor(acceptor_conn.get());
  SetCustomAcceptCallback([](Connection *) {});
  SetCustomHandleCallback([](Connection *) {});
}

/*
 * basic functionality for accepting new connection
 * provided to the acceptor by default
 */
void Acceptor::BaseAcceptCallback(Connection *server_conn) {
  NetAddress client_address;
  int accept_fd = server_conn->GetSocket()->Accept(client_address);
  if (accept_fd == -1) {
    return;
  }
  auto client_sock = std::make_unique<Socket>(accept_fd);
  client_sock->SetNonBlocking();
  auto client_connection = std::make_unique<Connection>(std::move(client_sock));
  client_connection->SetEvents(POLL_ADD | POLL_ET);  // edge-trigger for client
  client_connection->SetCallback(GetCustomHandleCallback());
  // randomized distribution. uniform in long term.
  int idx = rand() % reactors_.size();  // NOLINT
  client_connection->SetLooper(reactors_[idx]);
  reactors_[idx]->AddConnection(std::move(client_connection));
}

void Acceptor::SetCustomAcceptCallback(
    std::function<void(Connection *)> custom_accept_callback) {
  custom_accept_callback_ = std::move(custom_accept_callback);
  acceptor_conn->SetCallback([this](auto &&PH1) {
    BaseAcceptCallback(std::forward<decltype(PH1)>(PH1));
    GetCustomAcceptCallback()(std::forward<decltype(PH1)>(PH1));
  });
}

void Acceptor::SetCustomHandleCallback(
    std::function<void(Connection *)> custom_handle_callback) {
  custom_handle_callback_ = std::move(custom_handle_callback);
}

auto Acceptor::GetCustomAcceptCallback() const noexcept
    -> std::function<void(Connection *)> {
  return custom_accept_callback_;
}

auto Acceptor::GetCustomHandleCallback() const noexcept
    -> std::function<void(Connection *)> {
  return custom_handle_callback_;
}

auto Acceptor::GetAcceptorConnection() noexcept -> Connection * {
  return acceptor_conn.get();
}

}  // namespace TURTLE_SERVER
