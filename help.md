
### struct to for IPv4 only (see sockaddr_in6 for IPv6)
```c
struct sockaddr_in {
    sa_family_t    sin_family;
    in_port_t      sin_port;
    struct in_addr sin_addr;
};

struct in_addr {
    uint32_t       s_addr;
};
```
### struct for IPv6 only (see sockaddr_in for IPv4)

```C
struct sockaddr_in6 {
    sa_family_t     sin6_family;
    in_port_t       sin6_port;
    uint32_t        sin6_flowinfo;
    struct in6_addr sin6_addr;
    uint32_t        sin6_scope_id;
};
struct in6_addr {
    unsigned char   s6_addr[16];
};
```
### Converting an IP Address to an Integer
```C
struct sockaddr_in sa;// IPV 4
int inet_pton(int af, const char * src, void *dst);
```
- **af** : an integer that represents the address’ family of Internet protocols. For an IPv4 address, we will want AF_INET here; for an IPv6 address, AF_INET6.
- **src**: a string containing the IPv4 or IPv6 address to convert.
- **dst**: a pointer to an in_addr (IPv4) or in6_addr (IPv6) structure in which to store the result of the conversion.

```C
struct sockaddr_in6 sa;// IPv6 only
inet_pton(AF_INET6, "2001:db8:0:85a3::ac1f:8001", &(sa.sin6_addr));
```

## Automatically Fill In the IP Address with getaddrinfo()

```C
int getaddrinfo(const char *node, const char *service,
                const struct addrinfo *hints,
                struct addrinfo **res);
```

- **node** : a string representing an IP address (IPv4 or IPv6), or a domain name like “www.example.com”. Here, we can also indicate ``NULL`` if we supply the right flags to the hints structure described below (for example, if we want to automatically fill the localhost address).
- **service**: a string representing the port we’d like to connect to, such as “80”, or the name of the service to connect to, like “http”. On a Unix system, the list of available services and their ports can be found in ``/etc/services``. This list is also available online on iana.org .
- **hints** : a pointer to an ``addrinfo`` structure where we can supply more information about the connection we want to establish. The hints we provide here act as a kind of search filter for ``getaddrinfo()``. We will examine this structure more closely below.
- **res** : a pointer to a linked list of ``addrinfo`` structures where ``getaddrinfo()`` can store its result(s).

### The addrinfo Structure
```C
struct addrinfo {
    int              ai_flags;
    int              ai_family;
    int              ai_socktype;
    int              ai_protocol;
    size_t           ai_addrlen;
    struct sockaddr *ai_addr;
    char            *ai_canonname;
    struct addrinfo *ai_next;
};
```

- **i_flags**: flags containing options that we can combine with a bitwise OR. Here, the most interesting flag that we might want to use is ``AI_PASSIVE``, which indicates that the socket we will create will be used to listen for and accept connections in the context of a socket. In this case, we won’t need to indicate an IP address when we call ``getaddrinfo()``, since the IP will be that of the machine. A list of all of the available flags can be found on the manual page for ``getaddrinfo()``.

- **ai_family**: the address family of Internet protocols. To force an IPv4 address, we can indicate ``AF_INET``; to force an IPv6 address, ``AF_INET6``. One advantage here is that we can make our code IP-agnostic by indicating ``AF_UNSPEC``. In this case, ``getaddrinfo()`` will return IPv4 and IPv6 addresses.

- **ai_socktype**: the type of socket we’d like to create with the address. Two constants are available here: ``SOCK_STREAM``, which uses TCP, and ``SOCK_DGRAM``, which uses UDP. We can also indicate 0 here, to tell ``getaddrinfo()`` that it can return any type of socket address.

- **ai_protocol**: the protocol of the socket address. In general, there is only one valid protocol for each socket type - TCP for a stream socket, UDP for a datagram socket -, so we can safely put 0 here to tell ``getaddrinfo()`` that is can return any type of addresses.

- **ai_addrlen**: ``getaddrinfo()`` will indicate the length of the address here.

- **ai_addr**: a pointer to a ``sockaddr_in`` or ``sockaddr_in6``, which we’ve seen previously, which ``getaddrinfo()`` will fill out for us.

- **ai_canonname**: only used if the ``AI_CANONNAME`` flag is set in **ai_flags**.
    
- **ai_next**: a pointer to the next element in the linked list.

## Preparing Sockets
```C
#include <sys/socket.h>
int socket(int domain, int type, int protocol);
```

-    **domain**: the socket’s protocol family, generally ``PF_INET`` or ``PF_INET6``. ``PF_INET`` exists for historical reasons and is, in practice, identical to ``AF_INET``. The same is true for ``PF_INET6``.
-   **type**: the type of socket, generally ``SOCK_STREAM`` or ``SOCK_DGRAM``.
-   **protocol**: the protocol to use with the socket. In general, there is only one    valid protocol by socket type, TCP for a stream socket and UDP for a datagram socket, which means we can safely put 0 here. 
---

- Connect the socket to a remote address with ``connect()``. This will allow it to work as a **client**, able to make requests to a remote server.
- Connect the socket to a local address with ``bind()``. In this case, it will work as a **server**, able to accept connections from remote clients.

![Socket architecture](https://www.codequoi.com/images/socket-c/sockets-en.drawio.png "socket")

## Client Side: Connecting to a Server via a Socket

 ```C
 #include <sys/socket.h>
 int connect(int sockfd, const struct sockaddr *serv_addr,
            socklen_t addrlen);
```
- **sockfd**: the socket file descriptor we got from our call to ``socket()``,
- **serv_addr**: a pointer to the structure containing the connection information. This will either be ``sockaddr_in`` for an IPv4 address, or ``sockaddr_in6`` for an IPv6 address.
- **addrlen**: the size in bytes of the previous structure, ``serv_addr``


## Server Side: Accepting Client Connections via a Socket

### Binding the Socket to the Local Address
```C
#include <sys/socket.h>
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

- **sockfd**: the socket file descriptor we got with our call to ``socket()``.
- **addr**: a pointer to the structure containing the connection information. This is either a ``sockaddr_in`` for an IPv4 address, or a ``sockaddr_in6`` for an IPv6 address.
- **addrlen**: the size in bytes of the previous structure, ``addr``.
