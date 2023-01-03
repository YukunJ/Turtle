/**
 * @file http_server.cpp
 * @author Yukun J
 * @expectation this is the http server for illustration and test purpose
 * @init_date Jan 3 2023
 */

#include "turtle_server.h"

namespace TURTLE_SERVER {

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
    HTTP::Request request{request_op.value()};
    std::vector<unsigned char> response_buf;

    if (!request.IsValid()) {
      auto response = HTTP::Response::Make400Response();
      no_more_parse = true;
      response.Serialize(response_buf);
    } else {
      std::string resource_full_path =
          serving_directory + request.GetResourceUrl();
      if (!HTTP::IsFileExists(resource_full_path)) {
        auto response = HTTP::Response::Make404Response();
        no_more_parse = true;
        response.Serialize(response_buf);
      } else {
        auto response = HTTP::Response::Make200Response(request.ShouldClose(),
                                                        resource_full_path);
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
}  // namespace TURTLE_SERVER

int main(int argc, char *argv[]) {
  TURTLE_SERVER::NetAddress local_address("0.0.0.0", 20080);
  std::string directory = "../http_dir/";  // the default http serving directory
  if (argc == 2) {
    directory = argv[1];
  }
  TURTLE_SERVER::TurtleServer http_server(local_address);
  http_server
      .OnHandle([&](TURTLE_SERVER::Connection *client_conn) {
        TURTLE_SERVER::ProcessHttpRequest(http_server, directory, client_conn);
      })
      .Begin();
  return 0;
}
