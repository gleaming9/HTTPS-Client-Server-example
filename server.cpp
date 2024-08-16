#include <iostream>
#include <fstream>
#include <string>

#include <proxygen/httpserver/HTTPServerOptions.h>
#include <proxygen/httpserver/HTTPServer.h>
#include <proxygen/httpserver/samples/hq/HQHandlerFactory.h>

using namespace proxygen;

class MyHandlerFactory : public HQHandlerFactory {
public:
  void onServerStart(folly::EventBase* /*evb*/) noexcept override {
    std::cout << "Serving on https://localhost:8000" << std::endl;
  }

  void onServerStop() noexcept override {}

  proxygen::RequestHandler* onRequest(proxygen::RequestHandler* /*handler*/,
                                      proxygen::HTTPMessage* /*message*/) noexcept override {
    return new MyHandler();
  }
};

class MyHandler : public proxygen::RequestHandler {
public:
  void onRequest(std::unique_ptr<proxygen::HTTPMessage> /*headers*/) noexcept override {}

  void onBody(std::unique_ptr<folly::IOBuf> /*body*/) noexcept override {}

  // 요청의 끝을 수신했을 때 호출되는 메서드
  void onEOM() noexcept override {
    // 요청된 프로토콜을 로그로 출력
    std::cout << "Got connection: " << txn_->getVersionString() << " from " << txn_->getPeerAddress().describe() << std::endl;

    // Send a message back to the client
    responseHandler_->sendHeaders(proxygen::HTTPMessage(200, "OK"));
    responseHandler_->sendBody(proxygen::folly::IOBuf::copyBuffer("Hello"));
    responseHandler_->sendEOM();
  }

  void onUpgrade(proxygen::UpgradeProtocol /*protocol*/) noexcept override {}

  void requestComplete() noexcept override { delete this; }

  void onError(proxygen::ProxygenError /*err*/) noexcept override { delete this; }
};

int main() {
  // HTTPServerOptions 생성
  proxygen::HTTPServerOptions options;
  options.threads = 1;

  // 서버 인증서 로드
  std::ifstream certStream("server.crt");
  if (!certStream) {
    std::cerr << "Failed to open server certificate file" << std::endl;
    return 1;
  }
  std::stringstream certBuffer;
  certBuffer << certStream.rdbuf();
  std::string certData = certBuffer.str();

  // 서버 인증서를 사용하여 SSL 컨텍스트 설정
  options.sslContextConfig = std::make_shared<proxygen::wangle::SSLContextConfig>();
  options.sslContextConfig->setCertificate(certData, "");

  // HTTPServer 생성 및 시작
  proxygen::HTTPServer server(std::make_unique<MyHandlerFactory>());
  server.bind(8000, std::move(options));
  server.start();

  // Event loop
  server.waitForShutdown();

  return 0;
}
