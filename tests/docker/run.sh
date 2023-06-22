#!/bin/bash

docker build -t nginx .

docker run -it -d -p 8080:80 --name nginx nginx
docker exec -it nginx /bin/bash