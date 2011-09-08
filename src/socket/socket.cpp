/**
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#include "socket.h"
#include "../logger.h"
#include <cstring>
#include <cerrno>
using namespace std;


namespace DM {

Socket::Socket()
  : m_sockfd(-1) {
  memset( &m_hints, 0, sizeof( m_hints ) );
}


Socket::~Socket() {
  close();
}


bool Socket::open_server(const string host, const string port) {
  addrinfo *server_addrinfo, *p;
  m_hints.ai_family = AF_UNSPEC;  // both IPv4 and IPv6
  m_hints.ai_socktype = SOCK_STREAM;
  m_hints.ai_flags = AI_PASSIVE;

  if ( host.compare("*") == 0 ) {
    getaddrinfo( NULL, port.c_str(), &m_hints, &server_addrinfo );
  }
  else {
    getaddrinfo( host.c_str(), port.c_str(), &m_hints, &server_addrinfo );
  }

  for ( p = server_addrinfo; p != NULL; p = p->ai_next ) {
    if ((m_sockfd = socket( p->ai_family, p->ai_socktype, p->ai_protocol )) < 0) {
      continue;
    }
    if (bind( m_sockfd, p->ai_addr, p->ai_addrlen ) < 0) {
      close();
      continue;
    }
    break;
  }
  freeaddrinfo(server_addrinfo);

  if (p == NULL) {
    DM::Logger::error() << "cannot open listening socket on port " << port << endl;
    close();
    return false;
  }

  if (listen( m_sockfd, TCP_MAX_CONNECTIONS ) < 0) {
    Logger::error() << "cannot listen on port " << port << endl;
    close();
    return false;
  }

  Logger::info() << "listening on port " << port << endl;
  return true;
}


bool Socket::open_client(const string host, const string port) {
  addrinfo *server_addrinfo, *p;
  m_hints.ai_family = AF_UNSPEC;
  m_hints.ai_socktype = SOCK_STREAM;

  getaddrinfo( host.c_str(), port.c_str(), &m_hints, &server_addrinfo );
  for (p = server_addrinfo; p != NULL; p = p->ai_next) {
    if ((m_sockfd = socket( p->ai_family, p->ai_socktype, p->ai_protocol )) < 0) {
      Logger::debug() << "error on socket()" << endl;
      continue;
    }
    if (connect( m_sockfd, p->ai_addr, p->ai_addrlen ) < 0) {
      close();
      Logger::debug() << "error on connect()" << endl;
      continue;
    }
    // if we reach this point we have a valid socket
    break;
  }
  if (p == NULL) {
    Logger::debug() << "error connecting " << host << ":" << port << ": " << strerror(errno) << endl;
    close();
    return false;
  }
  freeaddrinfo(server_addrinfo);

  // set socket TIMEOUT
  timeval timeout;
  timeout.tv_sec = TCP_TIMEOUT;
  timeout.tv_usec = 0;
  int res = setsockopt( m_sockfd, SOL_SOCKET, SO_RCVTIMEO, (timeval*) &timeout, sizeof(timeval) );
  res = setsockopt( m_sockfd, SOL_SOCKET, SO_SNDTIMEO, (timeval*) &timeout, sizeof(timeval) );

  return true;
}


bool Socket::send(const string s) const {
  int status = ::send( m_sockfd, s.c_str(), s.size(), 0 );
  return (status != -1);
}


int Socket::recv(string& s) const {
  char buffer[ TCP_BUFFER_SIZE ];
  s.clear();

  int size = ::recv( m_sockfd, (void*) buffer, TCP_BUFFER_SIZE-1, 0 );

  if ( size < 0 ) {
      return 0;
  }
  else if ( size == 0 ) {
    return 0;
  }
  else {
    buffer[size] = 0;
    s = buffer;
    return size;
  }
}


bool Socket::close() {
  if ( is_valid() ) {
    ::close( m_sockfd );
    m_sockfd = -1;
    return true;
  }
  return false;
}

} // namespace DM