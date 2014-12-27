
#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>

class Socket
{
public:
  Socket();
  virtual ~Socket();

  // Server initialization
  bool create();
  bool bind ( const int port );
  bool listen(int maxConnections) const;
  bool accept ( Socket& ) const;

  // Client initialization
  bool connect ( const std::string host, const int port );

  // Data Transimission
  bool send ( const std::string ) const;
  int  recv ( std::string& ) const;
  void set_non_blocking ( const bool );
  bool is_valid() const { return m_sock != -1; }
private:
  int                 m_sock;
  struct sockaddr_in  m_addr; 
};

#endif /* __SOCKET_H__ */

