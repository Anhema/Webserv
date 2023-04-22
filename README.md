# Webserv


fnctl() -> on OSX   #include <unistd.h> #include <fcntl.h>

CGI -> executes external program. A typical use case occurs when a web user submits a web form on a web page that uses CGI. The form's data is sent to the web server within an HTTP request with a URL denoting a CGI script. The web server then launches the CGI script in a new computer process, passing the form data to it. The output of the CGI script, usually in the form of HTML, is returned by the script to the Web server, and the server relays it back to the browser as its response to the browser's request.[3]

NGINX example config -> https://www.nginx.com/resources/wiki/start/topics/examples/full/

HTTP WEB SERVER github miniguide -> https://github.com/Dungyichao/http_server

socket library -> #include <sys/socket.h>
socket domain -> AF_INET IPv4 Internet protocols ip(7) (man socket page)
socket type -> SOCK_STREAM Provides sequenced, reliable, two-way, connection-based byte streams.  An out-of-band data transmission mechanism may be supported. (man socket page)
socket return values -> On success, a file descriptor for the new socket is returned.  On error, -1 is returned, and errno is set appropriately.


struct sockaddr_in -> sin_zero

This member variable has no “real” utility in the sockaddr_in structure. It is a buffer used to adjust the size of the sockaddr_in in cases where you need to cast it to a sockaddr type. The sockaddr type is larger than the sockaddr_in type, so the sin_zero is used to make up the difference when the need to be interchanged. It’s not very relevant for our present architecture, so it can be left as is. It is however common practice to zero-out the bytes in this field when not used.

htons() doc -> https://www.ibm.com/docs/en/zos/2.4.0?topic=lf-htons-translate-unsigned-short-integer-into-network-byte-order -> convierte un integer en formato little endian (Host Byte Order) a big endian (Network Byte Order) formato necesario para la estructura sockaddr_in

host byte order and network byte order -> https://www.ibm.com/docs/ja/zvm/7.2?topic=domains-network-byte-order-host-byte-order
