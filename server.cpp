#include <iostream>
#include <fstream>
#include <string>

#include <proxygen/httpserver/HTTPServerOptions.h>
#include <proxygen/httpserver/HTTPServer.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>

using namespace proxygen;

class MyHandlerFactory : public RequestHandlerFactory {
public:
  // 서버 시작 시 호출되는 메서드
  void onServerStart(folly::EventBase* /*evb*/) noexcept override {
    std::cout << "Serving on https://localhost:8000" << std::endl;
  }

  void onServerStop() noexcept override {}

  // 요청을 처리하기 위해 RequestHandler를 생성하는 메서드
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
    // 요청 프로토콜을 로그로 출력
    std::cout << "Got connection: " << txn_->getVersionString() << " from " << txn_->getPeerAddress().describe() << std::endl;

    // 클라이언트에게 응답
    responseHandler_->sendHeaders(HTTPMessage(200, "OK"));
    responseHandler_->sendBody(folly::IOBuf::copyBuffer("Protocol: " + txn_->getVersionString()));
    responseHandler_->sendEOM();
  }

  void onUpgrade(UpgradeProtocol /*protocol*/) noexcept override {}

  void requestComplete() noexcept override { delete this; }

  void onError(ProxygenError /*err*/) noexcept override { delete this; }
};

int main() {
  // HTTPServerOptions 생성
  HTTPServerOptions options;
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

  // SSL 컨텍스트에 서버 인증서 설정
  options.sslContextConfig = std::make_shared<wangle::SSLContextConfig>();
  options.sslContextConfig->setCertificate(certData, "");

  // HTTPServer 생성 및 시작
  HTTPServer server(std::make_unique<MyHandlerFactory>());
  server.bind(8000, std::move(options));
  server.start();

  // 이벤트 루프
  server.waitForShutdown();

  return 0;
}
