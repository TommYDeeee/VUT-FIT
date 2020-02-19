import socket
import sys
import re
from urllib.parse import urlparse, parse_qs

isrequest = re.compile("^(GET|POST) \/\S* HTTP\/(1\.0|1\.1)$")
isget = re.compile("^GET \/\S* HTTP\/(1\.0|1\.1)$") 
ispost = re.compile("^POST \/\S* HTTP\/(1\.0|1\.1)$") 

def parsedata(data):
    text = data.decode().split('\r\n')  
    return text[0]

def parse_get(url):
    split_url = str.split(url)
    parse_url = urlparse(split_url[1])
    params = parse_qs(parse_url.query)
    return params

def get_request(request):
    url = parse_get(request)
    name = url['name'][0]
    name = name.replace("\\","")
    req_type = url['type'][0]
    ip = socket.gethostbyname(name)
    print(name+':'+req_type+'='+ip)

def post_request(request):
    print(request)
    print("\nPOST")

if (len(sys.argv)!=2):
    sys.stderr.write("WRONG ARGUMENTS\n")
    sys.exit()

PORT = int(sys.argv[1])
SOCKET = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
SOCKET.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
print("localhost started on port:", PORT)

try:
    SOCKET.bind(("", PORT))
except:
    sys.stderr.write("WRONG PORT\n")
    sys.exit()

SOCKET.listen(100)
while True:
    try:
        print("waiting...")
        connection, address = SOCKET.accept()
        print("connection from", address)
        data = connection.recv(1024)
        print(data)
        text = parsedata(data)
        if re.match(isrequest, text):
            if re.match(isget,text):
                get_request(text)
            elif re.match(ispost,text):
                post_request(text)
            else:
                sys.stderr.write("WRONG REQUEST")
    except:
        break

SOCKET.close()



