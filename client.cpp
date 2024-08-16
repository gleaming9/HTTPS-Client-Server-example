#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <proxygen/httpclient/samples/curl/CurlClient.h>
#include <proxygen/httpclient/samples/curl/EasyClient.h>
#include <proxygen/httpclient/samples/curl/URL.h>

using namespace proxygen;

void performRequest(const std::string& url, const std::string& certFile, int httpVersion) {
  // 서버 인증서 로드
  std::ifstream certStream(certFile);
  if (!certStream) {
    std::cerr << "Failed to open server certificate file: " << certFile << std::endl;
    return;
  }
  std::stringstream certBuffer;
  certBuffer << certStream.rdbuf();
  std::string certData = certBuffer.str();

  // CurlClient 생성
  CurlClient client;

  // HTTP 버전 설정
  switch (httpVersion) {
    case 1:
      std::cout << "Connect to " << url << " over TLS using HTTP/1.1" << std::endl;
      client.setUseHTTP11(true);
      break;
    case 2:
      std::cout << "Connect to " << url << " over TLS using HTTP/2" << std::endl;
      client.setUseHTTP2(true);
      break;
    default:
      std::cerr << "Invalid HTTP version: " << httpVersion << std::endl;
      return;
  }

  // 서버 인증서 설정
  client.setCaPath(certData);

  // 요청 수행
  auto response = client.getURL(url);

  if (response.success()) {
    std::cout << "Got response " << response.getErrorCode() << ": " << response.getProto() << " " << response.getBody()
              << std::endl;
  } else {
    std::cerr << "Failed get: " << response.getErrorMessage() << std::endl;
  }
}

int main(int argc, char* argv[]) {
  // 각 매개변수 파싱
  if (argc < 4) {
    std::cerr << "Usage: " << argv[0] << " <url> <cert-file> <http-version>" << std::endl;
    return 1;
  }
  std::string url = argv[1];
  std::string certFile = argv[2];
  int httpVersion = std::stoi(argv[3]);

  // 요청 수행
  performRequest(url, certFile, httpVersion);

  return 0;
}
