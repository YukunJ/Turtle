/**
 * @file http_server.cpp
 * @author Yukun J
 * @expectation this is the http server for illustration and test purpose
 * @init_date Jan 3 2023
 */

#include "turtle_server.h"

namespace TURTLE_SERVER::HTTP {


void ProcessHttpRequest(TurtleServer &server,  // NOLINT
                        const std::string &serving_directory,
                        Connection *client_conn) {
  // edge-trigger, first read all available bytes
  int from_fd = client_conn->GetFd();
  auto [read, exit] = client_conn->Recv();
  if (exit) {
    server.GetLooper()->DeleteConnection(from_fd);
    // client_conn ptr is invalid below here, do not touch it again
    return;
  }
  // check if there is any complete http request ready
  bool no_more_parse = false;
  std::optional<std::string> request_op =
      client_conn->FindAndPopTill("\r\n\r\n");
  while (request_op != std::nullopt) {
    Request request{request_op.value()};
    std::vector<unsigned char> response_buf;

    if (!request.IsValid()) {
      auto response = Response::Make400Response();
      no_more_parse = true;
      response.Serialize(response_buf);
    } else {
      std::string resource_full_path =
          serving_directory + request.GetResourceUrl();
      if (!IsFileExists(resource_full_path)) {
        auto response = Response::Make404Response();
        no_more_parse = true;
        response.Serialize(response_buf);
      } else {
        auto response = Response::Make200Response(request.ShouldClose(),
                                                  resource_full_path);
        response.SetShouldTransferContent(request.GetMethod() != Method::HEAD);
        no_more_parse = request.ShouldClose();
        response.Serialize(response_buf);
      }
    }
    // send out the response
    client_conn->WriteToWriteBuffer(std::move(response_buf));
    client_conn->Send();
    if (no_more_parse) {
      break;
    }
    request_op = client_conn->FindAndPopTill("\r\n\r\n");
  }
  if (no_more_parse) {
    server.GetLooper()->DeleteConnection(from_fd);
    // client_conn ptr is invalid below here, do not touch it again
    return;
  }
}
}  // namespace TURTLE_SERVER::HTTP

int main(int argc, char *argv[]) {
  const std::string usage =
      "Usage: \n"
      "./http_server [optional: port default=20080] [optional: directory "
      "default=../http_dir/] \n";
  if (argc > 3) {
    std::cout << "argument number error\n";
    std::cout << usage;
    exit(EXIT_FAILURE);
  }
  TURTLE_SERVER::NetAddress address("0.0.0.0", 20080);
  std::string directory = "../http_dir/";
  if (argc >= 2) {
    auto port = static_cast<uint16_t>(std::strtol(argv[1], nullptr, 10));
    if (port == 0) {
      std::cout << "port error\n";
      std::cout << usage;
      exit(EXIT_FAILURE);
    }
    address = {"0.0.0.0", port};
    if (argc == 3) {
      directory = argv[2];
      if (!TURTLE_SERVER::HTTP::IsDirectoryExists(directory)) {
        std::cout << "directory error\n";
        std::cout << usage;
        exit(EXIT_FAILURE);
      }
    }
  }
  TURTLE_SERVER::TurtleServer http_server(address);
  http_server
      .OnHandle([&](TURTLE_SERVER::Connection *client_conn) {
        TURTLE_SERVER::HTTP::ProcessHttpRequest(http_server, directory,
                                                client_conn);
      })
      .Begin();
  return 0;
}
