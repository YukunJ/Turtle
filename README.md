<img src="image/ninja.jpeg" alt="TurtleChat Logo" height="250">

-----------------
## TURTLE

[**中文文档** Chinese Version](./README_CN.md)

**Turtle** is a C++17-based lightweight network library for web server on Linux. It abstracts the tedious manipulations on the socket into elegant and reusable classes. It allows a fast server side setup where the custom business logic could be specified for each client TCP connection in the form of a callback function. It now supports HTTP GET/HEAD request and response as well.

For any question, feel free to raise issue or pull request or drop me an [email](mailto:yukunj@andrew.cmu.edu) here.

### Highlight

+ Set non-blocking socket and edge-trigger handling mode to support high concurrency workload.
+ Adopt the 'one reactor per thread' philosophy by [Shuo Chen](https://github.com/chenshuo) with thread pool management.
+ Achieve low coupling and high extensible framework.
+ Allow users to build custom server by only providing 2 callback functions.
+ Support HTTP GET/HEAD request & response.
+ Support Caching mechanism.

### System Diagram

<img src="image/system_architecture_en.png" alt="System Architecture New" height="450">

The above system architecture diagram briefly shows how the **Turtle** framework works on a high level.

1. The basic unit is a **Connection** which contains a **Socket** and a **Buffer** for bytes in-and-out. Users register a **callback** function for each connection.
2. The system starts with an **Acceptor**, which contains one acceptor connection. It builds connection for each new client, and distribute the workload to one of the **Looper**s.
3. Each **Poller** is associated with exactly one **Looper**. It does nothing but epoll, and returns a collection of event-ready connections back to the **Looper**.
4. The **Looper** is the main brain of the system. It registers new client connection into the **Poller**, and upon the **Poller** returns back event-ready connections, it fetches their callback functions and execute them.
5. The **ThreadPool** manages how many **Looper**s are there in the system to avoid over-subscription.
6. Optionally there exists a **Cache** layer using LRU policy with tunable storage size parameters.

### Docker

If you are not a Linux system but still want to try out the Turtle for fun, we provide a Vagrant File to provision the Linux Docker.

1. Install [Vagrant](https://www.vagrantup.com/downloads) and [Docker](https://docs.docker.com/desktop/). For macOS, you may use homebrew to install Vagrant but **do not** use homebrew to install Docker. Instead, download Docker Desktop from the link above.

2. Start the Docker application in the background

3. Drag out the `Vagrantfile` and place it in parallel with the `Turtle` project folder. For example, consider the following file structure:

```text
/Turtle_Wrapper
    - /Turtle
    - /Vagrantfile
```

4. `cd` to the `Turtle_Wrapper` folder and run command `vagrant up --provider=docker`. This step should take a few minutes to build up the environment and install all the necessary tool chains.

5. Enter the docker environment by `vagrant ssh developer`

6. `cd` to the directory `/vagrant/Turtle`. This directory is in sync with the original `./Turtle`folder. You may modify the source code and its effect will be propagated to the docker's folder as well.

7. Follow the steps in next section to build up the project.

### Build

You may build the project using **CMake**.

Once you are at the root directory of this project, execute the followings:

```console
// Build
$ mkdir build
$ cd build
$ cmake .. // because CMakeList.txt is in the root directory
$ make

// Format & Style Check & Line Count
$ make format
$ make cpplint
$ make linecount
```

### Performance Testing

To test the performance of **Turtle** server under high concurrency, we adopt [Webbench](http://cs.uccs.edu/~cs526/webbench/webbench.htm) as the stress testing tool.

The source code of the Webbench is stored under the `./webbench` directory along with a simple testing shell script. 

We fully automated the process so that you can execute the benchmark test in one command:
```console
$ make benchmark

# the above command will
# 1. build the webbench tool
# 2. run the http server in the background at default 20080 port, serving the ~1M index file
# 3. launch the webbench testing with 10500 concurrent clients for 5 seconds
# 4. report back the result to the console
# 5. harvest the background http server process and exit
```

We performed benchmark testing on an Amazon AWS EC2 instance. The details are as follows:

+ **Hardware**: m5.2xlarge instance on **Ubuntu 20.04 LTS** with **8** vCPUs, **32** GiB memory, **50** GiB root storage volume.
+ **QPS**: **49.7**k (no cache) | **50.4**k (with cache)

The performance improvement from **Cache** might not seem significant. Partly because disk I/O is getting faster nowadays, the cost of loading a small `index.html` might be smaller than the mutual exclusive operations in the **Cache**.

Later on, when database connector comes into play, the indispensability of the **Cache** layer will be more obvious.

### Usage


To setup a custom server, user should create an instance of **TurtleServer** and then only needs to provide two callback functions:
1. **OnAccept(Connection \*)**: A function to do extra business logic when accepting a new client connection.
2. **OnHandle(Connection \*)**: A function to serve an existing client's request.

Notice that most of common functionality for accepting a new client connection is already implemented and supported in the [**Acceptor::BaseAcceptCallback**](./src/include/acceptor.h), including socket accept, setup and put it under monitor of the **Poller**.

The function provided in **OnAccept(Connection \*)** by users will be augmented into the base version and called as well. There is no base version for the **OnHandle(Connection \*)**. Users must specify one before they can call **Begin()** on the server.

Let's walk through an example of traditional echo server in less than 20 lines:

```CPP
#include "turtle_server.h"

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
```
The demo of this echo [server](./demo/echo_server.cpp) and [client](./demo/echo_client.cpp) is provided under the `./demo` folder for your reference. In the `build` directory, you can execute the following and try it out.

```console
$ make echo_server
$ make echo_client

// in one terminal
$ ./echo_server

// in another terminal
$ ./echo_client
```

The HTTP server [demo](./src/http/http_server.cpp) is under `./src/http` folder for reference as well. A simple HTTP server could be set up in less than 50 lines with the help of **Turtle** core and http module. 

### Future Work
This repo is under active development and maintainence. New features and fixes are updated periodically as time and skill permit.

The followings are on the **TODO** list:

- ✅ Support serving HTTP GET/HEAD Request & Response
- ✅ Revise according to this [code review](https://codereview.stackexchange.com/questions/282220/tiny-network-web-framework-library-in-c) suggestions
- ✅ Refactor the architecture into multiple Reactor mode to improve concurrency
- ✅ Add performance testing benchmark
- ✅ Add a Cache layer to reduce server load and increase responsiveness
- [ ] Complete unit testing coverage
- [ ] Support timing each client connection and kills inactive ones
- [ ] Support Database connection

We also welcome new feature request. We will review them one by one, and priortize its implementation if applicable. Or direct pull request is also welcomed.

### Reference

Serveral reference books and projects are consulted during the development of **Turtle**. Much appreciation to those excellent references and helps received.

+ [Network Programming Using Internet Sockets](https://beej.us/guide/bgnet/)
+ [Linux Multithreaded Server Programming: Using the Muduo C++ Network Library](http://www.amazon.cn/dp/B00FF1XYJI)
+ [Muduo](https://github.com/chenshuo/muduo)
+ [TinyWebServer](https://github.com/qinguoyi/TinyWebServer)
+ [30dayMakeCppServer](https://github.com/yuesong-feng/30dayMakeCppServer)
+ [Very basic C++ HTTP Parser](https://codereview.stackexchange.com/questions/205704/very-basic-c-http-parser)