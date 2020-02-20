import socket
import sys
import re
from urllib.parse import urlparse, parse_qs

isrequest = re.compile("^(GET|POST) \/\S* HTTP\/(1\.0|1\.1)$")
isget = re.compile("^GET \/\S* HTTP\/(1\.0|1\.1)$") 
ispost = re.compile("^POST \/\S* HTTP\/(1\.0|1\.1)$") 
correct_post = re.compile("^.*:(PTR|A)$")
http_v = 0

def parsedata(data):
    global http_v
    text = data.decode('utf-8').split('\r\n') 
    header = str.split(text[0])
    http_v = header[2]
    return text[0]

def parse_get(url):
    parse_url = urlparse(url[1])
    if (parse_url.path != "/resolve"):
        return 400
    params = parse_qs(parse_url.query)
    return params

def get_request(request):
    split_url = str.split(request)
    url = parse_get(split_url)
    if (url == 400):
        data_to_send = http_v + " 400 Bad Request\r\n"
        return data_to_send
    try:
        name = url['name'][0]
    except:
        data_to_send = http_v + " 400 Bad Request\r\n"
        return data_to_send
    req_type = url['type'][0]
    if (req_type == "A"):
        try:
            result = socket.gethostbyname(name)
        except:
            data_to_send = http_v + " 404 Not Found\r\n"
            return data_to_send
    elif (req_type == "PTR"):
        try:
            result = socket.gethostbyaddr(name)
            result = result[0]
        except:
            data_to_send = http_v + " 404 Not Found\r\n"
            return data_to_send
    else:
        data_to_send = http_v + " 400 Bad Request\r\n"
        return data_to_send
    result = name + ":" + req_type + "=" + result
    data_to_send = http_v + " 200 OK\r\n\r\n" + result + "\r\n"
    return data_to_send

def post_request(request, http_v):
    lines = request.split('\n')
    body = ""
    for line in lines:
        if not(re.match(correct_post,line)):
            data_to_send = http_v + " 400 Bad Request\r\n"
            return data_to_send
        split_line = line.split(":")
        if(split_line[1] == "A"):
            result = socket.gethostbyname(split_line[0])
            result = line + "=" + result + '\n' 
            body = body + result
        elif(split_line[1] == "PTR"):
            result = socket.gethostbyaddr(split_line[0])
            result = result[0]
            result = line + "=" + result + '\n' 
            body = body + result
    body = body.strip()
    data_to_send = http_v + " 200 OK\r\n\r\n" + body + '\r\n'
    return data_to_send

if (len(sys.argv)!=2):
    sys.stderr.write("WRONG ARGUMENTS\n")
    sys.exit()

PORT = int(sys.argv[1])
SOCKET = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
SOCKET.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
#print("localhost started on port:", PORT)

try:
    SOCKET.bind(("", PORT))
except:
    sys.stderr.write("WRONG PORT\n")
    sys.exit()

SOCKET.listen(100)
while True:
    try:
        #print("waiting...")
        connection, address = SOCKET.accept()
        #print("connection from", address)
        data = connection.recv(1024)
        #print(data)
        text = parsedata(data)
        if re.match(isrequest, text):
            if re.match(isget,text):
                response = get_request(text)
                #print(repr(response))
                connection.send(response.encode())
                connection.close()
            elif re.match(ispost,text):
                text = data.decode('utf-8').split('\r\n\r\n') 
                body = '\r\n\r\n'.join(text[1:])
                body =  body.rstrip()
                response = post_request(body, http_v)
                #print(repr(response))
                connection.send(response.encode())
                connection.close()               
            else:
                sys.stderr.write("WRONG REQUEST")
        else:
            response = http_v + " 405 Method Not Allowed\r\n"
            connection.send(response.encode())
            connection.close()
    except:
        break




