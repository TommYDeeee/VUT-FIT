import socket
import sys
import re

isrequest = re.compile("^(GET|POST) \/\S* HTTP\/(1\.0|1\.1)$")
isget = re.compile("^GET \/\S* HTTP\/(1\.0|1\.1)$") 
ispost = re.compile("^POST \/\S* HTTP\/(1\.0|1\.1)$") 

def parsedata(data):
    text = data.decode().split('\r\n')  
    text = cut_body(text)
    return text 

def cut_body(text):
    text2 = []
    for line in text:
        if line == '':
            return text2
        text2.append(line)
    return text2

def get_request(request):
    print(request)
    print("\nGET")

def post_request(request):
    print(request)
    print("\nPOST")

if (len(sys.argv)!=2):
    sys.stderr.write("WRONG ARGUMENTS\n")
    sys.exit()

PORT = int(sys.argv[1])
SOCKET = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
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
        if re.match(isrequest, text[0]):
            if re.match(isget,text[0]):
                get_request(text[0])
            elif re.match(ispost,text[0]):
                post_request(text[0])
            else:
                sys.stderr.write("WRONG REQUEST")
    except:
        break

SOCKET.close()



