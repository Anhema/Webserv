import http.client


conn = http.client.HTTPConnection("localhost:8080")
conn.putrequest("POST", "/post", skip_accept_encoding=True)
conn.putheader("Transfer-Encoding", "chunked")
conn.endheaders()

data = b"chunked_data_part_1"
conn.send(f"{len(data):X}\r\n".encode() + data + b"\r\n")

data = b"chunked_data_part_2"
conn.send(f"{len(data):X}\r\n".encode() + data + b"\r\n")
