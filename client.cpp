#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <proxygen/httpclient/samples/curl/CurlClient.h>
#include <proxygen/httpclient/samples/curl/EasyClient.h>
#include <proxygen/httpclient/samples/curl/URL.h>

using namespace proxygen;

void performRequest(const std::string& url, const std::string& certFile, int httpVersion) {
  // Load server certificate
  std::ifstream certStream(certFile);
  if (!certStream) {
    std::cerr << "Failed to open server certificate file: " << certFile << std::endl;
    return;
  }
  std::stringstream certBuffer;
  certBuffer << certStream.rdbuf();
  std::string certData = certBuffer.str();

  // Create CurlClient
  CurlClient client;

  // Set HTTP version
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

  // Set server certificate
  client.setCaPath(certData);

  // Perform the request
  auto response = client.getURL(url);

  if (response.success()) {
    std::cout << "Got response " << response.getErrorCode() << ": " << response.getProto() << " " << response.getBody()
              << std::endl;
  } else {
    std::cerr << "Failed get: " << response.getErrorMessage() << std::endl;
  }
}

int main(int argc, char* argv[]) {
  // Parse command line parameters
  if (argc < 4) {
    std::cerr << "Usage: " << argv[0] << " <url> <cert-file> <http-version>" << std::endl;
    return 1;
  }
  std::string url = argv[1];
  std::string certFile = argv[2];
  int httpVersion = std::stoi(argv[3]);

  // Perform the request
  performRequest(url, certFile, httpVersion);

  return 0;
}