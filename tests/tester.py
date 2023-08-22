import os
import unittest
import http.client
import logging
import time
from html.parser import HTMLParser

server_host = "localhost:8080"
server_path = "/"

servers = [
    "localhost:8080",
    "localhost:80",
    "localhost:42",
]

crazy_servers = [
    "localhost:9090",
    "localhost:4242",
]

# Configure the logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

# Create a handler for output to the console
console_handler = logging.StreamHandler()
console_handler.setLevel(logging.INFO)
formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
console_handler.setFormatter(formatter)


class HtmlExtractor(HTMLParser):
    def __init__(self):
        super().__init__()
        self.title_found = False
        self.title_content = ""

    def handle_starttag(self, tag, attrs):
        if tag == 'title':
            self.title_found = True

    def handle_data(self, data):
        if self.title_found:
            self.title_content += data

    def handle_endtag(self, tag):
        if tag == 'title':
            self.title_found = False


def extract_title_from_html(html_content):
    parser = HtmlExtractor()
    parser.feed(html_content)
    return parser.title_content


class Response:
    def __init__(self, status, body, title):
        self.status = status
        self.body = body
        # self.head = head
        self.title = title


def get_response(method, host, path):
    try:
        connection = http.client.HTTPConnection(host)
        connection.request(method, path)
        connection_response = connection.getresponse()

        status = connection_response.status
        body = connection_response.read().decode('utf-8', errors='replace')
        connection.close()
        title = extract_title_from_html(body)

        # print("Body: " + body)
        # print("Title: " + title)

        return Response(status, body, title)

    except http.client.HTTPException as e:
        logging.error("exception connecting")


class TestServer(unittest.TestCase):

    def test_server_connection(self):
        print("")
        logging.info("**** Basic availability test ****")

        for server in servers:
            try:
                conn = http.client.HTTPConnection(server)
                conn.request("GET", "/")
                conn.close()
                logging.info(server + " OK")

            except http.client.HTTPException as e:
                self.fail(f"Server connection test failed: {e}")
                logging.error("Failed connection to server: ")

        logging.info("**** Successful ****\n")

    def test_not_found(self):

        logging.info("**** Error 404 Test ****")

        for server in servers:
            response = get_response("GET", server, "/doesntexist")

            self.assertEqual(response.status, 404, server + " failed")
            self.assertEqual(response.title, "404 Not Found")

            logging.info(server + " OK")
        print("")

    def test_not_allowed(self):
        logging.info("**** Error 405 Test ****")

        for server in servers:
            response = get_response("POST", server, "/noaccess")

            self.assertEqual(response.status, 405, server + " failed")
            self.assertEqual(response.title, "405 Method Not Allowed")
            logging.info(server + " OK")
        print("")

    def test_redirection(self):
        logging.info("**** Redirection Test ****")

        for server in servers:
            response = get_response("GET", server, "/redirection")

            self.assertEqual(response.status, 301)

            logging.info(server + " OK")
        print("")

    def test_rooted_index(self):
        logging.info("**** Rooted Index Test ****")

        for server in servers:
            response = get_response("GET", server, "/rooted_index")

            self.assertEqual(response.status, 200, server + " failed")
            self.assertEqual(response.title, "Nested Html")

            logging.info(server + " OK")
        print("")

    def test_rooted_file(self):
        for server in servers:

            response = get_response("GET", server, "/rooted/nested_index.html")
            self.assertEqual(response.status, 200, server + " failed")
            self.assertEqual(response.title, "Nested Html")
            logging.info(server + " OK")
        print("")


    def test_put(self):
        logging.info("**** Put Test ****")

        for server in servers:
            response = get_response("PUT", server, "/unimplemented")

            self.assertEqual(response.status, 405, server + " failed")
            self.assertEqual(response.title, "405 Method Not Allowed")
            logging.info(server + " PUT OK")
        print("")

    def test_head(self):
        logging.info("**** Head Test ****")

        for server in servers:
            response = get_response("HEAD", server, "/unimplemented")

            self.assertEqual(response.status, 405, server + " failed")
            logging.info(server + " HEAD OK")
        print("")

    def test_autoindex(self):
        logging.info("**** Autoindex Test ****")

        for server in servers:
            response = get_response("GET", server, "/autoindex")

            self.assertEqual(response.status, 200, server + " failed")
            self.assertEqual(response.title, "Directory Links")
            logging.info(server + " OK")
        print("")
    def test_directory_location(self):
        for server in servers:
            response = get_response("GET", server, "/directory")

            self.assertEqual(response.status, 200, server + " failed")
            self.assertEqual(response.title, "Directory Links")
            logging.info(server + " OK")


    def test_go_crazy(self):

        logging.info("**** Going Crazy ****")

        for server in crazy_servers:
            response = get_response("GET", server, "/directory/oulalala")

            self.assertEqual(response.status, 404, server + " failed")
    def test_cgi_path(self):
        for server in servers:
            response = get_response("GET", server, "/CGI/add.html")

            self.assertEqual(response.status, 200, server + " failed")
            logging.info(server + " OK")

if __name__ == "__main__":
    logging.info("Starting Test Server")
    os.system("pkill webserv")
    os.system("./webserv_sani tests/test.conf  & ")
    time.sleep(1)
    logging.info("Initializing tests\n")
    unittest.main()
