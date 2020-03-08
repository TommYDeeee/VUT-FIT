import socket
import sys
import re
from urllib.parse import urlparse, parse_qs

# Regulárne výrazy na formálnu kontrolu URL
is_request = re.compile("^(GET|POST) \/\S* HTTP\/(1\.0|1\.1)$")
is_get = re.compile("^GET \/\S* HTTP\/(1\.0|1\.1)$") 
is_post = re.compile("^POST /dns-query HTTP\/(1\.0|1\.1)$") 
correct_post = re.compile("^.*:(PTR|A)$")
http_v = 0

# funkcia získa z prvého riadku požiadavku HTTP hlavičku a vráti prvý riadok s potrebnými informáciami
def parsedata(data):
    global http_v
    text = data.decode('utf-8').split('\r\n') 
    header = str.split(text[0])
    http_v = header[2]
    return text[0]

# funkcia overí či metóda GET má správne formálne parametre a vráti ich
def parse_get(url):
    parse_url = urlparse(url[1])
    if (parse_url.path != "/resolve"):
        return 400
    params = parse_qs(parse_url.query)
    return params

# funkcia spracováva metódu GET, overenie parametrov, vyhladanie odpovedi na požiadavku a jej následné vrátenie
def get_request(request):
    split_url = str.split(request) 
    url = parse_get(split_url)
    if (url == 400):
        data_to_send = http_v + " 400 Bad Request\r\n\r\n"
        return data_to_send
    # dodatočná kontrola správnosti parametrov
    try:
        name = url['name'][0]
    except:
        data_to_send = http_v + " 400 Bad Request\r\n\r\n"
        return data_to_send
    try:
        req_type = url['type'][0]
    except: 
        data_to_send = http_v + " 400 Bad Request\r\n\r\n"
        return data_to_send
    if (req_type == "A"):
        try:
            result = socket.gethostbyname(name)
            if(result == name):
                data_to_send = http_v + " 400 Bad Request\r\n\r\n"
                return data_to_send
        except:
            data_to_send = http_v + " 404 Not Found\r\n\r\n"
            return data_to_send
    elif (req_type == "PTR"):
        #otestujeme či IP adresa na vstupe je formálne správna (využívame funkciu inet_aton)
        try:
            socket.inet_aton(name)
        except:
            data_to_send = http_v + " 400 Bad Request\r\n\r\n"
            return data_to_send            
        try:
            # funkcia na získanie doménového mena z IP adresy
            result = socket.gethostbyaddr(name)
            result = result[0]
            if(result == name):
                data_to_send = http_v + " 400 Bad Request\r\n\r\n"
                return data_to_send
        except:
            data_to_send = http_v + " 404 Not Found\r\n\r\n"
            return data_to_send
    else:
        data_to_send = http_v + " 400 Bad Request\r\n\r\n"
        return data_to_send
    result = name + ":" + req_type + "=" + result
    #uložíme si výsledok do správneho formátu aj s teľom odpovede
    data_to_send = http_v + " 200 OK\r\n\r\n" + result + "\r\n"
    return data_to_send

# funkcia spracováva metódu POST
# preskakujeme prázdne riadky alebo formálne nesprávne, pokial je telo prázdne alebo obsahuje
# iba biele znaky vraciame 404, ak je celé telo chybné vraciame 400 alebo 404 (ak bolo všetko formálne správne ale nezískali sme odpoved)
# ak je aspoň 1 riadok formálne správny a aj podarilo nájsť odpoveď vraciame 200 OK + odpoveď v správnom formáte
def post_request(request, http_v):
    lines = request.split('\n')
    body = ""
    error = " 404 Not Found\r\n\r\n"
    for line in lines:
        line = line.replace(" ","")
        if not line:
            continue
        if not(re.match(correct_post,line)):
            error = " 400 Bad Request\r\n\r\n"
            continue
        split_line = line.split(":")
        if(split_line[1] == "A"):
            try:
                result = socket.gethostbyname(split_line[0])
                if(result == split_line[0]):
                    continue
                result = line + "=" + result + '\r\n' 
                body = body + result
            except:
                continue
        elif(split_line[1] == "PTR"):
            try:
                socket.inet_aton(split_line[0])
            except:
                error = " 400 Bad Request\r\n\r\n" 
                continue
            try:
                result = socket.gethostbyaddr(split_line[0])
                result = result[0]
                if(result == split_line[0]):
                    continue
                result = line + "=" + result + '\r\n' 
                body = body + result
            except:
                continue
    body = body.strip()
    if not body:
            data_to_send = http_v + error
            return data_to_send
    data_to_send = http_v + " 200 OK\r\n\r\n" + body + '\r\n'
    return data_to_send

# kontrola počtu argumentov pri spustení NIE cez makefile
if (len(sys.argv)!=2):
    sys.stderr.write("WRONG ARGUMENTS\n")
    sys.exit(1)
#Priradenie čísla portu z prvého argumentu, vytvorenie socketu a následne pokus o nabindovanie portu
try:
    PORT = int(sys.argv[1])
except:
    sys.stderr.write("WRONG PORT\n")
    sys.exit(1)
SOCKET = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
SOCKET.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

try:
    SOCKET.bind(("127.0.0.1", PORT))
except:
    sys.stderr.write("WRONG PORT\n")
    sys.exit(1)

SOCKET.listen()
# nekonečná sľučka pre chod servera, príjmanie požiadavkov, zistenie metódy  a následné zavolanie funkcii na spracovanie a odoslanie výslednej odpovedi naspäť klientovi
while True:
    try:
        connection, address = SOCKET.accept()
        data = connection.recv(1024)
        text = parsedata(data)
        if re.match(is_request, text):
            if re.match(is_get,text):
                response = get_request(text)
                connection.sendall(response.encode())
                connection.close()
            elif re.match(is_post,text):
                text = data.decode('utf-8').split('\r\n\r\n') 
                body = '\r\n\r\n'.join(text[1:])
                body =  body.rstrip()
                response = post_request(body, http_v)
                connection.sendall(response.encode())
                connection.close()               
            else:
                response = http_v + " 400 Bad Request\r\n\r\n"
                connection.sendall(response.encode())
                connection.close()
        else:
            response = http_v + " 405 Method Not Allowed\r\n\r\n"
            connection.sendall(response.encode())
            connection.close()
    # ukončenie servera pri signále SIGINT
    except KeyboardInterrupt:
        break
