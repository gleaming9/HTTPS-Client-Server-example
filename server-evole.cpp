#include <iostream>
#include <fstream>
#include <string>

#include <proxygen/httpserver/HTTPServerOptions.h>
#include <proxygen/httpserver/HTTPServer.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>

using namespace proxygen;

class MyHandlerFactory : public RequestHandlerFactory {
public:
  void onServerStart(folly::EventBase* /*evb*/) noexcept override {
    std::cout << "Serving on https://localhost:8000" << std::endl;
  }

  void onServerStop() noexcept override {}

  RequestHandler* onRequest(RequestHandler* /*handler*/,
                            HTTPMessage* /*message*/) noexcept override {
    return new MyHandler();
  }
};

class MyHandler : public RequestHandler {
public:
  void onRequest(std::unique_ptr<HTTPMessage> /*headers*/) noexcept override {}

  void onBody(std::unique_ptr<folly::IOBuf> /*body*/) noexcept override {}

  void onEOM() noexcept override {
    // Log the request protocol
    std::cout << "Got connection: " << txn_->getVersionString() << " from " << txn_->getPeerAddress().describe() << std::endl;

    // Send a message back to the client
    responseHandler_->sendHeaders(HTTPMessage(200, "OK"));
    responseHandler_->sendBody(folly::IOBuf::copyBuffer("Protocol: " + txn_->getVersionString()));
    responseHandler_->sendEOM();
  }

  void onUpgrade(UpgradeProtocol /*protocol*/) noexcept override {}

  void requestComplete() noexcept override { delete this; }

  void onError(ProxygenError /*err*/) noexcept override { delete this; }
};

int main() {
  // Create HTTPServerOptions
  HTTPServerOptions options;
  options.threads = 1;

  // Load server certificate
  std::ifstream certStream("server.crt");
  if (!certStream) {
    std::cerr << "Failed to open server certificate file" << std::endl;
    return 1;
  }
  std::stringstream certBuffer;
  certBuffer << certStream.rdbuf();
  std::string certData = certBuffer.str();

  // Configure SSL context with server certificate
  options.sslContextConfig = std::make_shared<wangle::SSLContextConfig>();
  options.sslContextConfig->setCertificate(certData, "");

  // Create and start the HTTPServer
  HTTPServer server(std::make_unique<MyHandlerFactory>());
  server.bind(8000, std::move(options));
  server.start();

  // Event loop
  server.waitForShutdown();

  return 0;
}