// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

// - Access pb services via HTTP
//   ./http_client http://www.foo.com:8765/EchoService/Echo -d
//   '{"message":"hello"}'
// - Access builtin services
//   ./http_client http://www.foo.com:8765/vars/rpc_server*
// - Access www.foo.com
//   ./http_client www.foo.com

#include <brpc/channel.h>
#include <brpc/progressive_reader.h>
#include <butil/logging.h>
#include <gflags/gflags.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

DEFINE_string(d, "", "POST this data to the http server");
DEFINE_string(load_balancer, "", "The algorithm for load balancing");
DEFINE_int32(timeout_ms, 200000, "RPC timeout in milliseconds");
DEFINE_int32(max_retry, 3, "Max retries(not including the first RPC)");
DEFINE_string(protocol, "http", "Client-side protocol");

namespace brpc {
DECLARE_bool(http_verbose);
}

class MyProgressiveReader : public brpc::ProgressiveReader {
public:
  MyProgressiveReader(){};
  ~MyProgressiveReader(){};
  void init() {
    filepath = "./1";
    fd = open("./1", O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
    if (fd < 0) {
      fprintf(stderr, "%s", strerror(errno));
    }
    finished = false;
  }
  virtual butil::Status OnReadOnePart(const void *data, size_t length) {
    butil::Status status;
    int ret = write(fd, data, length);
    if (ret < 0) {
      fprintf(stderr, "%s", strerror(errno));
      status.set_error(errno, "%s", strerror(errno));
    }
    return status;
  }
  virtual void OnEndOfMessage(const butil::Status &status) {
    if (!status.ok()) {
      fprintf(stderr, "something wrong happend");
    }
    finished = true;
    close(fd);
    return;
  }
  int fd;
  std::string filepath;
  bool finished;
};

int main(int argc, char *argv[]) {
  // Parse gflags. We recommend you to use gflags as well.
  GFLAGS_NS::ParseCommandLineFlags(&argc, &argv, true);

  if (argc != 2) {
    LOG(ERROR) << "Usage: ./http_client \"http(s)://www.foo.com\"";
    return -1;
  }
  char *url = argv[1];

  // A Channel represents a communication line to a Server. Notice that
  // Channel is thread-safe and can be shared by all threads in your program.
  brpc::Channel channel;
  brpc::ChannelOptions options;
  options.protocol = FLAGS_protocol;
  options.timeout_ms = FLAGS_timeout_ms /*milliseconds*/;
  options.max_retry = FLAGS_max_retry;

  // Initialize the channel, NULL means using default options.
  // options, see `brpc/channel.h'.
  if (channel.Init(url, FLAGS_load_balancer.c_str(), &options) != 0) {
    LOG(ERROR) << "Fail to initialize channel";
    return -1;
  }

  // We will receive response synchronously, safe to put variables
  // on stack.
  brpc::Controller cntl;

  cntl.http_request().uri() = url;
  if (!FLAGS_d.empty()) {
    cntl.http_request().set_method(brpc::HTTP_METHOD_POST);
    cntl.request_attachment().append(FLAGS_d);
  }

  cntl.response_will_be_read_progressively();
  // Because `done'(last parameter) is NULL, this function waits until
  // the response comes back or error occurs(including timedout).
  channel.CallMethod(NULL, &cntl, NULL, NULL, NULL);
  // if (cntl.Failed()) {
  //     std::cerr << cntl.ErrorText() << std::endl;
  //     return -1;
  // }
  // If -http_verbose is on, brpc already prints the response to stderr.
  auto reader = new MyProgressiveReader();
  reader->init();
  cntl.ReadProgressiveAttachmentBy(reader);
  //if (!brpc::FLAGS_http_verbose) {
  //  std::cout << cntl.response_attachment() << std::endl;
  //  int fd = open("./1", O_CREAT | O_APPEND);
  //  if (fd < 0) {
  //    fprintf(stderr, "%s", strerror(errno));
  //  }
  //  cntl.response_attachment().cut_into_file_descriptor(fd);
  //}
  while(true){
    if(!reader->finished){
      sleep(1);
    }
    else{
      break;
    }
  }
  return 0;
}
