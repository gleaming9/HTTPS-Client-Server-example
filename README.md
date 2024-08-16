# HTTPS Client-Server Example

## 프로젝트 개요
 KHU 풀스택 서비스 네트워킹의 http2 server-client를 구현하는 텀프로젝트입니다. 오픈소스 라이브러리인 Proxygen을 사용하여 구현된 간단한 HTTPS 클라이언트와 서버의 예제입니다. 클라이언트와 서버 사이의 송수신을 구현했습니다. HTTPS를 사용한 통신을 수행하며, HTTP/1.1과 HTTP/2 프로토콜을 지원합니다.

## 파일 구성
- client.cpp: 클라이언트 프로그램으로, 서버에 HTTPS 요청을 보냅니다. HTTP/1.1 또는 HTTP/2 프로토콜을 사용할 수 있으며, 서버 인증서를 확인하여 TLS 연결을 설정합니다.
- server.cpp: 서버 프로그램으로, 클라이언트 요청을 성공적으로 처리했는지 확인합니다. 요청된 프로토콜과 클라이언트의 주소를 로그로 출력합니다.

## 요구 사항
- C++ 컴파일러 (C++11 이상)
- Proxygen 라이브러리
- Folly 라이브러리
- Wangle 라이브러리
- OpenSSL (TLS 연결을 위한 SSL/TLS 인증서 필요)
