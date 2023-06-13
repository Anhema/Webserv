#!/bin/bash

docker build -t webserv-nginx-docker .

docker run -it --rm -d  -p 8080:80 --name nginx-server webserv-nginx-docker