#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fstream>
#include <netdb.h>
#include <cerrno>
#include <filesystem>
#include "sockets.hpp"
#include "config.hpp"

namespace fs = filesystem;

u32 sendFileToServer(string &path, u32 size) {
    Config conf = Config::load();
    fs::path fpath(path);

    if (conf.destination_id == "undefined") {
        cout << "destination_id is not set in config" << endl;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        cout << "Socket init failed: " << strerror(errno) << endl;
        return -1;
    }

	struct hostent *host = gethostbyname(conf.host.c_str());
	if (host == nullptr || host->h_addr == nullptr) {
        cout << "DNS resolve failed (" << conf.host << "): " << strerror(errno) << endl;
        return -1;
    }

	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(conf.port);
	memcpy(&addr.sin_addr, host->h_addr, host->h_length);

    if (connect(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        close(sockfd);
        cout << "Connect failed: " << strerror(errno) << endl;
        return -1;
    }

	cout << "Building request" << endl;

	stringstream ss;
	ss << "POST /upload/" << conf.destination_id << "/?filename=" << fpath.filename().string() << " HTTP/1.1\r\n"
	   << "Host: " << conf.host << ":" << conf.port << "\r\n"
	   << "Content-Type: application/octet-stream\r\n"
	   << "Content-Length: " << size << "\r\n"
	   << "Content-Transfer-Encoding: binary\r\n"
	   << "Connection: close\r\n"
	   << "\r\n";
	string request = ss.str();

	cout << "Sending request" << endl << request;

	u32 bytesSent = send(sockfd, request.c_str(), request.length(), 0);
	if (bytesSent != request.length()) {
	    close(sockfd);
        cout << "Send failed: " << strerror(errno) << endl;
        return -1;
    }

    ifstream f(fpath, ios::binary | ios::ate);
    char *buf = new char[0x2000];
    f.seekg(0, ios::beg);
    for (u32 pos=0; pos < size; pos += 0x2000u) {
        u32 readSize = min(0x2000u, size - pos);
        f.read(buf, readSize);
        bytesSent += send(sockfd, buf, readSize, 0);
    }

    int bytesRecv = recv(sockfd, buf, 0x2000u - 1, 0);
    buf[bytesRecv] = 0;

    f.close();
    delete[] buf;

	close(sockfd);

	cout << "Request sent" << endl;

    return bytesSent;
}
