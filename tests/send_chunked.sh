#/bin/bash

curl -X POST -H "Host: localhost:8080" -H "Transfer-Encoding: chunked" -H 'Content-Disposition: form-data; name="file"; filename="test.txt"' -H "Content-Type: application/octet-stream" --data-binary "@tests/resources/text.txt" http://localhost:8080/