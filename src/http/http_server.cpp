/**
 * @file http_server.cpp
 * @author Yukun J
 * @expectation this is the http server for illustration and test purpose
 * @init_date Dec 31 2022
 */

#include "http/http_utils.h"
#include "http/request.h"
#include "http/response.h"
#include "turtle_server.h"
namespace TURTLE_SERVER::HTTP {

class HttpServer : public TurtleServer {
 public:
  explicit HttpServer(NetAddress server_address,
                      const std::string &serving_directory)
      : TurtleServer(server_address) {
    if (!IsDirectoryExists(serving_directory)) {
      std::string error_message =
          "The directory " + serving_directory + " does not exist";
      throw std::logic_error(error_message);
    }
    serving_directory_ = serving_directory + "/";
  }

  void OnAccept(Connection *server_conn) final {}

  void OnHandle(Connection *client_conn) final {
    // edge-trigger, first read all available bytes
    int from_fd = client_conn->GetFd();
    auto [read, exit] = client_conn->Recv();
    if (exit) {
      GetLooper()->DeleteConnection(from_fd);
      // client_conn ptr is invalid below here, do not touch it again
      return;
    }
    // check if there is any complete http request ready
    bool no_more_parse = false;
    std::optional<std::string> request_op =
        client_conn->GetReadBuffer()->FindAndPop("\r\n\r\n");
    while (request_op != std::nullopt) {
      Request request{request_op.value()};
      std::vector<unsigned char> response_buf;

      if (!request.IsValid()) {
        auto response = Response::Make400Response();
        no_more_parse = true;
        response.Serialize(response_buf);
      } else {
        std::string resource_full_path =
            serving_directory_ + request.GetResourceUrl();
        if (!IsFileExists(resource_full_path)) {
          auto response = Response::Make404Response();
          no_more_parse = true;
          response.Serialize(response_buf);
        } else {
          auto response = Response::Make200Response(request.ShouldClose(),
                                                    resource_full_path);
          no_more_parse = request.ShouldClose();
          response.Serialize(response_buf);
        }
      }
      // send out the response
      client_conn->WriteToWriteBuffer(response_buf.data(), response_buf.size());
      client_conn->Send();
      if (no_more_parse) {
        break;
      }
      request_op = client_conn->GetReadBuffer()->FindAndPop("\r\n\r\n");
    }
    if (no_more_parse) {
      GetLooper()->DeleteConnection(from_fd);
      // client_conn ptr is invalid below here, do not touch it again
      return;
    }
  }

 private:
  std::string serving_directory_;
};

}  // namespace TURTLE_SERVER::HTTP

int main(int argc, char *argv[]) {
  TURTLE_SERVER::NetAddress local_address("0.0.0.0", 20080);
  std::string directory = "../http_dir";  // the default http serving directory
  if (argc == 2) {
    directory = argv[1];
  }
  TURTLE_SERVER::HTTP::HttpServer http_server(local_address, directory);
  http_server.Begin();
  return 0;
}
