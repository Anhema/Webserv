# Webserv

## Index

1. HTTP Server introduction
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

A web server is a computer that stores web server software and a website's component files (for example, HTML documents, images, CSS stylesheets, and JavaScript files). A web server connects to the Internet and supports physical data interchange with other devices connected to the web.

The web server includes several parts that control how web users access hosted files. At a minimum, this is an HTTP server. An HTTP server is software that understands URLs (web addresses) and HTTP (the protocol your browser uses to view webpages). An HTTP server can be accessed through the domain names of the websites it stores, and it delivers the content of these hosted websites to the end user's device.

![Screenshot of a comment on a GitHub issue showing an image, added in the Markdown, of an Octocat smiling and raising a tentacle.](https://static.javatpoint.com/blog/images/web-servers2.png)

A web client, for example, a web browser, sends an HTTP request to a web server via a network. The web server which is powered by an HTTP server:

    - Receives this request and processes it
    - Determines the appropriate response after doing certain checks
    - Sends this response to the web client.

The response sent by the HTTP server may also include data from files (for example, HTML documents) stored on the webserver.

Now that we’re done with the base introduction — let’s dive right into the design for our server.

We will be using TCP (Transmission Control Protocol) to implement our HTTP server.

TCP (Transmission Control Protocol) is an important network protocol that lets two hosts connect and exchange data streams. TCP guarantees the delivery of data and packets in the same order as they were sent.

## Configuration file

## References

### WebServer
https://developer.mozilla.org/en-US/docs/Learn/Common_questions/Web_mechanics/What_is_a_web_server
https://osasazamegbe.medium.com/showing-building-an-http-server-from-scratch-in-c-2da7c0db6cb7

### CGI
https://diego.com.es/concepto-y-funcionamiento-de-cgi

http://www.mnuwer.dbasedeveloper.co.uk/dlearn/web/session03.htm

