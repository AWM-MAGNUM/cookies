#!/usr/bin/python3

import cgi
import traceback
import sys

def handle_error():
    print("Content-type: text/html\r\n\r\n")
    print("<html>")
    print("<head>")
    print("<title>Error</title>")
    print("<style>")
    print("body { font-family: Arial, sans-serif; }")
    print(".container { width: 50%; margin: 0 auto; }")
    print(".error { padding: 20px; border: 1px solid #f00; background-color: #fee; }")
    print("</style>")
    print("</head>")
    print("<body>")
    print("<header><h1>Error</h1></header>")
    print("<div class='container'>")
    print("<div class='error'>")
    print("<h2>An error occurred</h2>")
    print("<pre>")
    traceback.print_exc(file=sys.stdout)
    print("</pre>")
    print("</div>")
    print("</div>")
    print("</body>")
    print("</html>")

try:
    print("Content-type: text/html\r\n\r\n")

    form = cgi.FieldStorage()

    name = form.getvalue('name', 'N/A')
    email = form.getvalue('email', 'N/A')
    subject = form.getvalue('subject', 'N/A')
    message = form.getvalue('message', 'N/A')

    print("<html>")
    print("<head>")
    print("<title>Form Result</title>")
    print("<style>")
    print("body { font-family: Arial, sans-serif; }")
    print(".container { width: 50%; margin: 0 auto; }")
    print(".result { padding: 20px; border: 1px solid #ccc; }")
    print("</style>")
    print("</head>")
    print("<body>")
    print("<header><h1>Form Submission Result</h1></header>")
    print("<div class='container'>")
    print("<div class='result'>")
    print("<h2>Form Result</h2>")
    print(f"<p>Name: {name}</p>")
    print(f"<p>Email: {email}</p>")
    print(f"<p>Subject: {subject}</p>")
    print(f"<p>Message: {message}</p>")
    print("</div>")
    print("</div>")
    print("</body>")
    print("</html>")
except Exception:
    handle_error()