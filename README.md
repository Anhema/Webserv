# Webserv

## Index

1. HTTP Server introduction
2. Server
	- Handle Connections
	- Headers
	- GET, POST and DELETE
	- Response
	- Upload files
	- RFC
	- CGI
2. Configuration file
	- Basic info
	- Error pages
	- Upload size limit
	- Rules
		- HTTP Method
		- HTTP Redirections
		- File Routes
		- Directory List
	- Default file
	- CGI Exec
	- CGI Functionality
3. References

## HTTP Server introduction

A **web server** is a computer that stores web server software and a website's component files (for example, HTML documents, images, CSS stylesheets, and JavaScript files). A web server connects to the Internet and supports physical data interchange with other devices connected to the web.

The **web server** includes several parts that control how web users access hosted files. At a minimum, this is an HTTP server. An **HTTP server** is software that understands URLs (web addresses) and HTTP (the protocol your browser uses to view webpages). An **HTTP server** can be accessed through the domain names of the websites it stores, and it delivers the content of these hosted websites to the end user's device.

![Screenshot of a comment on a GitHub issue showing an image, added in the Markdown, of an Octocat smiling and raising a tentacle.](https://static.javatpoint.com/blog/images/web-servers2.png)

A web client, for example, a web browser, sends an **HTTP** request to a web server via a network. The web server which is powered by an **HTTP server**:

    - Receives this request and processes it
    - Determines the appropriate response after doing certain checks
    - Sends this response to the web client.

The response sent by the **HTTP** server may also include data from files (for example, HTML documents) stored on the webserver.

We will be using **TCP** (Transmission Control Protocol) to implement our **HTTP** server.

**TCP** (Transmission Control Protocol) is an important network protocol that lets two hosts connect and exchange data streams. **TCP** guarantees the delivery of data and packets in the same order as they were sent.

Server will use a **TCP/IP** socket registered to an IP address on the computer. It will also have a specific port through which the socket will listen for incoming network connections. Network connections that come into the server will be stored on a Queue of network threads. When the Queue is full, incoming network requests will start to fall through. The server will process each network thread synchronously. The network connection will be used to create a temporary socket through which the server reads data from the client and sends data to the client.

	- Listens for incoming network connections and puts them on a Queue
	- Accepts a network connection from the Queue one at a time
	- Reads the data (Request) sent from the client over the network connection
	- Sends data (Response) to the client over the network connection.

## Server

HTTP communication usually takes place over TCP. A typical HTTP session often consists of three steps: The client and server establish a TCP connection stream, the client sends HTTP request over TCP connection, and then the server processes that request and sends back a reply. The second and third step can be repeated any number of times, until both client and server decide to close the underlying TCP connection. To put it in a simple diagram, this is how the process looks like in the perspective of TCP.
![Screenshot of a comment on a GitHub issue showing an image, added in the Markdown, of an Octocat smiling and raising a tentacle.](https://www.layerstack.com/img/docs/resources/Apachediagram1.jpg)

As a server, these are the main steps that we need to take care of:

    - Create a socket and listen for new connections.
    - Accept incoming client connections.
    - Receive messages, process them and sends some responses to the client. This is where HTTP message exchange happens.
    - When one party wants to close the connection, it will do that by sending an EOF character and closing the socket file descriptor.

### HTTP Messages
HTTP is the most common application layer protocol that serves as the basis for many communications over the web. In a client-server setting, HTTP messages are the requests and responses objects exchanged between the two parties. An HTTP client sends a HTTP request to an HTTP server, and the server will reply with an HTTP response. The messages must follow some format specified in the RFCs. For the small scope of my project, I picked out the most basic components to implement in my program. In short, an HTTP message should consist of:

    - A start line: For an HTTP request, this line includes an HTTP method (GET, POST, HEAD, etc), a request target (URI), and a string that indicates the HTTP version (e.g HTTP/1.1). For an HTTP response, the start line (or status line) will have the HTTP version that the server used, a status code, an optionally, a message describing the status code. The start line of a message should be terminated by a CRLF character.
    - Header fields: A list of key - value pairs that appear right after the start line and contain metadata about the HTTP connection and message. Each field should be on a single line and have the format field-name: field-value
    - Message body: An optional sequence of bytes. The message body is often present in response messages from the server, and sometimes in requests sent by the client, depending on the HTTP method. An HTTP message body can have any format, as long as both client and server have no issue understanding it.

**HTTP Request**
> GET /hello.html HTTP/1.1
> Host: 0.0.0.0
> Accept-Language: en, vi

**HTTP Response**
> HTTP/1.1 200 OK
> Server: Hello
> Content-Length: 13
> Content-Type: text/plain
> Hello, world

## Configuration file

## References

### WebServer
https://developer.mozilla.org/en-US/docs/Learn/Common_questions/Web_mechanics/What_is_a_web_server

https://osasazamegbe.medium.com/showing-building-an-http-server-from-scratch-in-c-2da7c0db6cb7

https://trungams.github.io/2020-08-23-a-simple-http-server-from-scratch/

### RFC
https://www.ietf.org/standards/rfcs/

https://datatracker.ietf.org/doc/html/rfc7230

### CGI
https://diego.com.es/concepto-y-funcionamiento-de-cgi

http://www.mnuwer.dbasedeveloper.co.uk/dlearn/web/session03.htm

