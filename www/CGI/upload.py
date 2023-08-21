#!/usr/bin/python3

import cgi
import os

# Set the content type to HTML
print("Content-Type: text/html")
print()

# HTML form for file upload
print("<html>")
print("<head><title>File Upload</title></head>")
print("<body>")
# Check if a file was uploaded
form = cgi.FieldStorage()

if "upload" in form:
    fileitem = form["upload"]

    # Check if the file was uploaded successfully
    if fileitem.filename:
        # Get the filename and save the file to a specific location
        filename = os.path.basename(fileitem.filename)
        with open("/path/to/uploaded/files/" + filename, "wb") as f:
            f.write(fileitem.file.read())
        print("<p>File '{}' has been uploaded.</p>".format(filename))
    else:
        print("<p>No file was uploaded.</p>")

print("</body>")
print("</html>")
