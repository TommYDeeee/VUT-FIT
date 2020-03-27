import sys
import argparse
import re
import xml.etree.ElementTree as ET

#Globalne premenné a regulárne výrazy
pattern = re.compile(r'\s+')
var_pattern = re.compile(r'^(LF|TF|GF)@[a-zA-Z_\-$&%*!?][a-zA-Z0-9_\-$&%*?!]*$')
sym_pattern = re.compile(r'^int@[-+]?[0-9]+$|^bool@true$|^bool@false$|^nil@nil$|^float@+')
label_pattern = re.compile(r'^[a-zA-Z_\-$&%*!?][a-zA-Z0-9_\-$&%*!?]*$')
escape_pattern = re.compile(r'\\[0-9]{3}')
invalid_string = re.compile(r'[#\s]')
insts_count = vars_count = 0
labels = {}
functions = []
data = []
TF = None
GF = {}
LF = []

zero_ins = ["CREATEFRAME", "PUSHFRAME", "POPFRAME", "RETURN", "BREAK", "CLEARS", "ADDS", "SUBS", "MULS", "DIVS", "IDIVS", "LTS", "GTS", "EQS", "ANDS", "ORS", "NOTS", "STRI2INTS", "INT2CHARS"]
one_ins = ["DEFVAR", "CALL", "PUSHS", "POPS", "WRITE", "LABEL", "JUMP", "EXIT", "DPRINT", "JUMPIFEQS", "JUMPIFNEQS"]
two_ins = ["MOVE", "INT2CHAR", "READ", "STRLEN", "TYPE", "NOT", "INT2FLOAT", "FLOAT2INT"]
three_ins = ["ADD", "SUB", "MUL", "IDIV", "LT", "GT", "EQ", "AND", "OR", "STRI2INT", 
    "CONCAT", "GETCHAR", "SETCHAR", "JUMPIFEQ", "JUMPIFNEQ", "DIV"]

#Funkcie na vracanie chyboivej návratovej hodnoty
def exit_xml():
    sys.stderr.write('xml is invalid\n')
    sys.exit(32)     

def exit_semantics():
    sys.stderr.write('Semantic error\n')
    sys.exit(52)

def exit_operands():
    sys.stderr.write('Wrong operands\n')
    sys.exit(53)

def exit_non_var():
    sys.stderr.write('Variable is not existing!\n')
    sys.exit(54)    

def exit_non_frame():
    sys.stderr.write('Frame is not existing!\n')
    sys.exit(55)    

def exit_none_var():
    sys.stderr.write('Missing variable value\n')
    sys.exit(56)

def exit_bad_operand():
    sys.stderr.write("Bad operand given\n")
    sys.exit(57)

def exit_string():
    sys.stderr.write("String error\n")
    sys.exit(58)

#Vypísanie nápovedy
def print_help():
    print("interpreter.py")
    print("Napoveda:")
    print("Autor: Tomas Duris (xduris05)\n")
    print("Skript interpreter.py nacita XML reprezentaciu programu a tento program s vyuzitim vstupu na zaklade parametrov interpretuje a vygeneruje vystup")
    print("--------------------------------------------------------------------------------------------------------")
    print("Pouzitie:")
    print("--help    pre vypisanie napovedy")
    print("--source=file  vstupny subor sd XML reprezentaciou zdrojoveho kodu")
    print("--input= file  subor so vstupmi pre samotnu interpretaciu zdrojoveho kodu")
    print("Musi byt zadany aspoň jeden z parametrov --source=file, --input=file inak chyba! Ak jeden chyba, zodpovedajuce data su nacitane zo standardneho vstupu")
    sys.exit(0)

#Kontrola XML hlavičky a jej atribútov, kontrola nadbytočné textu v elementoch a odstránenie bielych znakov
def check_xml_root(xml):
    if not 'language' in xml.attrib:
        exit_xml()
    if (xml.tag != 'program' or xml.attrib['language'] != "IPPcode20"):
        exit_xml()
    for attrib in xml.attrib:
        if(attrib not in ('name', 'language', 'description')):
            exit_xml()
    if not (list(xml)):
        sys.exit(0)
    try:
        xml.text = re.sub(pattern, '', xml.text)
    except:
        pass
    try:
        xml.tail = re.sub(pattern, '', xml.tail)
    except:
        pass
    if (xml.text):
        exit_xml()
    if (xml.tail):
        exit_xml()
    xml = check_xml_ins(xml)
    return xml

#Prechod XML inštrukciami, kontrola ich atribútov a ich vzostupné zoradenie na základe "opcode order" (nemusí byť súvislá postupnosť)
def check_xml_ins(xml):
    order = 0
    sort_ins = []
    for ins in xml:
        try:
            ins.text = re.sub(pattern, '', ins.text)
        except:
            pass
        try:
            ins.tail = re.sub(pattern, '', ins.tail)
        except:
            pass     
        if(ins.text):
            exit_xml()
        if(ins.tail):
            exit_xml()
        if(ins.tag != 'instruction'):
            exit_xml()
        for key in ins.attrib.keys():
            if key not in ['order', 'opcode']:
                exit_xml()
        if ('order' not in ins.attrib or 'opcode' not in ins.attrib):
            exit_xml()
        order = int(ins.attrib['order'])
        sort_ins.append((order,ins))
    try:
        sort_ins.sort()
    except:
        exit_xml()
    xml[:] = [item[-1] for item in sort_ins]
    execute_ins(xml)
    return xml

#Kontrola premennej
def var_check(text):
    if not (re.match(var_pattern, text)):
        exit_xml()

#Kontrola konštant a escape sekvencii pri konštante string
def sym_check(type, text):
    if (type == 'var'):
        if not (re.match(var_pattern, text)):
            exit_xml()
    elif (type == 'string'):
        if(text == None):
            text= ""
        if(invalid_string.search(text) != None):
            exit_xml()
        backslash = re.findall(r'\\', text)
        if(backslash):
            escape = re.findall(r'\\[0-9]{3}', text)
            if (len(escape)!= len(backslash)):
                exit_xml()
    else:
        value = text
        text = type + '@' + text
        if not (re.match(sym_pattern, text)):
            exit_xml()
        if(type == "float"):
            try:
                float.fromhex(value)
            except:
                exit_xml()

#Kontrola návestí
def label_check(text):
    if not (re.match(label_pattern, text)):
        exit_xml()

#Kontrola typov
def type_check(text):
    if not (re.match(r'^int$|^bool$|^string$|^float$', text)):
        exit_xml()

#Kontrola inštrukcii s 0 argumentmi
def zero_arg_check(ins):
    if(len(ins) != 0):
        exit_xml()

#Kontrola inštrukcii s 1 argumentom a pokiaľ sa jedná o inštrukciu LABEL, uloženie definovaných návestí aj s poradím skoku
def one_arg_check(ins, i):
    if(len(ins) != 1):
        exit_xml()
    for arg in ins:
        try:
            arg.tail = re.sub(pattern, '', arg.tail)
        except:
            pass
        if (arg.tail):
            exit_xml()
        if(len(arg) != 0):
            exit_xml()
        if(len(arg.attrib) != 1 or 'type' not in arg.attrib or arg.tag != 'arg1'):
            exit_xml()
    ins_name = ins.attrib['opcode'].upper()
    if ins_name in ["DEFVAR", "POPS"]:
        if(arg.attrib['type'] == 'var'):
            var_check(arg.text)
        else:
            exit_xml()
    elif ins_name in ["PUSHS","WRITE","EXIT","DPRINT"]:
        if(arg.attrib['type'] in ["int", "bool", "string", "var", "nil", "float"]):
            sym_check(arg.attrib['type'], arg.text)
        else:
            exit_xml()
    elif ins_name in ["CALL","LABEL","JUMP"]:
        if(arg.attrib['type'] == 'label'):
            label_check(arg.text)
            if(ins_name == "LABEL"):
                if(arg.text in labels):
                    exit_semantics()
                else:
                    labels[arg.text] = i
        else:
            exit_xml()

#Kontrola inštrukcii s 2 argumentmi
def two_arg_check(ins):
    if(ins[0].tag not in ['arg1', 'arg2'] or ins[1].tag not in ['arg1', 'arg2']):
        exit_xml()
    if(ins[0].tag == ins[1].tag):
        exit_xml()
    if(ins[0].tag != 'arg1'):
        ins[0], ins[1] = ins[1], ins[0]
    if(len(ins) != 2):
        exit_xml()
    for arg in ins:
        try:
            arg.tail = re.sub(pattern, '', arg.tail)
        except:
            pass    
        if (arg.tail):
            exit_xml()
        if(len(arg) != 0):
            exit_xml()
        if (len(arg.attrib) != 1 or 'type' not in arg.attrib):
            exit_xml()
        if(arg.tag == 'arg1' ):
            if(arg.attrib['type'] == 'var'):
                var_check(arg.text)
            else:
                exit_xml()
        elif(arg.tag == 'arg2'):
            if((ins.attrib['opcode'].upper()) == 'READ'):
                if(arg.attrib['type']  == 'type'):
                    type_check(arg.text)
                else:
                    exit_xml()
            else:
                if(arg.attrib['type'] in ["int", "bool", "string", "var", "nil", "float"]):
                    sym_check(arg.attrib['type'], arg.text)
                else:
                    exit_xml()

#Kontrola inštrukcii s 3 argumentmi, kontrola všetkých možných kombinácii poradia argumentov a ich zoradenie 
def three_arg_check(ins):
    if((ins[0].tag == ins[1].tag) or (ins[0].tag == ins[2].tag) or (ins[1].tag == ins[2].tag)):
        exit_xml()
    if(ins[0].tag == 'arg2' and ins[1].tag == 'arg3' and ins[2].tag == 'arg1'):
        ins[0], ins[1], ins[2] = ins[2], ins[0], ins[1]
    if(ins[0].tag == 'arg3' and ins[1].tag == 'arg2' and ins[2].tag == 'arg1'):
        ins[0], ins[1], ins[2] = ins[2], ins[1], ins[0]
    if(ins[0].tag == 'arg1' and ins[1].tag == 'arg3' and ins[2].tag == 'arg2'):
        ins[0], ins[1], ins[2] = ins[0], ins[2], ins[1]
    if(ins[0].tag == 'arg2' and ins[1].tag == 'arg1' and ins[2].tag == 'arg3'):
        ins[0], ins[1], ins[2] = ins[1], ins[0], ins[2]
    if(ins[0].tag == 'arg3' and ins[1].tag == 'arg1' and ins[2].tag == 'arg2'):
        ins[0], ins[1], ins[2] = ins[1], ins[2], ins[0]
    if(len(ins)!=3):
        exit_xml()
    for arg in ins:
        try:
            arg.tail = re.sub(pattern, '', arg.tail)
        except:
            pass
        if (arg.tail):
            exit_xml()
        if(len(arg)!=0):
            exit_xml
        if(len(arg.attrib) !=1 or 'type' not in arg.attrib):
            exit_xml()
        if(arg.tag == 'arg1'):
            if(ins.attrib['opcode'].upper() in ['JUMPIFEQ', 'JUMPIFNEQ']):
                if(arg.attrib['type'] == 'label'):
                    label_check(arg.text)
                else:
                    exit_xml()
            else:
                if(arg.attrib['type'] == 'var'):
                    var_check(arg.text)
                else:
                    exit_xml()
        elif(arg.tag == 'arg2' or arg.tag == 'arg3'):
            if(arg.attrib['type'] in ["int", "bool", "string", "var", "nil", "float"]):
                sym_check(arg.attrib['type'], arg.text)
            else:
                    exit_xml()

#Cyklus cez všetky inštrukcie a kontrola argumentov na základe očakávaného počtu argumentov
def execute_ins(xml):
    i = 0
    for ins in xml:
        ins_name = ins.attrib['opcode'].upper()
        args = get_args(ins_name)
        if(args == 0):
            zero_arg_check(ins)
        if(args == 1):
            one_arg_check(ins, i)
        if(args == 2):
            two_arg_check(ins)
        if (args == 3):
            three_arg_check(ins)
        i+=1

#Zistenie očakávaného počtu argumentov
def get_args(ins):
    if ins in zero_ins:
        return 0
    if ins in one_ins:
        return 1
    if ins in two_ins:
        return 2
    if ins in three_ins:
        return 3
    else:
        exit_xml()

#Prevod bool stringu na bool
def convert_to_bool(value1, value2):
    if(value1 == 'true' or value1 == True):
        value1 = True
    else:
        value1= False
    if(value2 == 'true' or value2 == True):
        value2= True
    else:
        value2 = False
    return value1, value2

#Zistenie hodnoty typu
def get_type_val(value1):
    try:
        if(value1['nil'] == None):
            return 'nil'
    except:
        pass
    try:
        if(value1['read']):
            return 'read'
    except:
        pass
    if type(value1) is bool:
       return 'bool'
    if type(value1) is int:
        return 'int'
    if(type(value1) is float):
        return 'float'
    if type(value1) is str:
        return 'string'

#Zistenie typu zo vstupu
def get_type(value):
    if(value['type'] == 'int'):
        return 'int'
    if(value['type'] == 'bool'):
        return 'bool'
    if(value['type'] == 'string'):
        return 'string'
    if(value['type'] == 'var'):
        return 'var'
    if(value['type'] == 'nil'):
        return 'nil'
    if(value['type'] == 'float'):
        return 'float'

#Prevod escape sekvencie na string 
def interpret_escape(escape):
    if(escape == None):
        return ""
    def replace(match):
        return chr(int(match.group()[1:]))
    escape = re.sub(escape_pattern, replace, escape)
    return escape

#Odelenie typu rámca od názvu rámca
def var_values(var):
    frame, var_value = var.split("@", 1)
    return frame, var_value

#Kontrola premennej, získanie hodnoty a typu
def check_var(type1, value1):
    value1 = symb_from_var(value1)
    if(value1 == None):
        exit_none_var()
    type1 = get_type_val(value1)
    return(type1, value1)

#Uloženie hodnoty do rámca
def save_to_var(frame, var , symb, symb_type):
    if(symb_type == 'str'):
        symb_type == 'string'
    if (frame == "TF"):
        if TF == None:
            exit_non_frame()
        elif var not in TF.keys():
            exit_non_var()
        else:
            if(symb_type == 'int'):
                TF[var] = int(symb)
            elif(symb_type == 'float'):
                TF[var] = float(symb)
            elif(symb_type == 'string'):
                TF[var] = symb
            elif(symb_type == 'bool'):
                if(symb == 'true' or symb == True):
                    TF[var] = True
                else:
                    TF[var] = False
            elif(symb_type == 'nil'):
                TF[var] = {'nil': None}
            elif(symb_type == 'read'):
                TF[var] = symb
    elif (frame == "LF"):
        if not LF:
            exit_non_frame()
        elif var not in LF[-1].keys():
            exit_non_var()
        else:
            if(symb_type == 'int'):
                LF[-1][var]  = int(symb)
            elif(symb_type == 'float'):
                LF[-1][var]  = float(symb)
            elif(symb_type == 'string'):
                LF[-1][var]  = symb
            elif(symb_type == 'bool'):
                if(symb == 'true' or symb == True):
                    LF[-1][var] = True
                else:
                    LF[-1][var] = False
            elif(symb_type == 'nil'):
                LF[-1][var] = {'nil': None}
            elif(symb_type == 'read'):
                LF[-1][var] = symb
    else:
        if var not in GF.keys():
            exit_non_var()
        else:
            if(symb_type == 'int'):
                GF[var] = int(symb)
            elif(symb_type == 'float'):
                GF[var] = float(symb)
            elif(symb_type == 'string'):
                GF[var] = symb
            elif(symb_type == 'bool'):
                if(symb == 'true' or symb == True):
                    GF[var] = True
                else:
                    GF[var] = False
            elif (symb_type == 'nil' or symb_type == None):
                GF[var] = {'nil': None}
            elif(symb_type == 'read'):
                GF[var] = symb

#Vytvorenie premennej v rámci
def init_var(frame, var):
    if (frame == "TF"):
        if TF == None:
            exit_non_frame()
        elif var in TF.keys():
            exit_semantics()
        else:
            TF[var] = None
    elif (frame == "LF"):
        if not LF:
            exit_non_frame()
        elif var in LF[-1].keys():
            exit_semantics()
        else:
            LF[-1][var] = None
    else:
        if var in GF.keys():
            exit_semantics()
        else:
            GF[var] = None

#Získanie hodnoty z rámca
def symb_from_var(symb):
    frame, var_value = var_values(symb)
    if(frame == "TF"):
        if (TF == None):
            exit_non_frame()
        if var_value not in TF.keys():
            exit_non_var()
        else:
            return TF.get(var_value)
    elif(frame == "LF"):
        if not LF:
            exit_non_frame()
        if var_value not in LF[-1].keys():
            exit_non_var()
        else:
            return LF[-1].get(var_value)
    else:
        if var_value not in GF.keys():
            exit_non_var()
        else:
            return GF.get(var_value)

#Oddelenie string hodnoty od string hodnoty načítanej prostredníctvom READ (pri READ neprevádzame escape sekvencie)
def read_vs_string(type1, value1, type2, value2):
    if(type1 == 'string'):
        value1 = interpret_escape(value1)
    if(type2 == 'string'):
        value2 = interpret_escape(value2)
    if(type1 == 'read'):
        value1 = value1.get('read')
    if(type2 == 'read'):
        value2 = value2.get('read')
    return value1, value2    

#INŠTRUKCIE
#Inštrukcia MOVE, uloženie hodnoty do premennej
def r_Move(var, symb, var_value, symb_value):
    symb = get_type(symb)
    if (symb == 'var'):
        symb, symb_value = check_var(symb, symb_value)
    else:
        if(symb == 'string'):
            symb_value = interpret_escape(symb_value)
        if(symb == 'bool'):
            if(symb_value == 'true'):
                symb_value = True
            else:
                symb_value = False
        if(symb == 'nil'):
            symb_value = {'nil': None}
        if (symb == 'int'):
            symb_value = int(symb_value)
        if (symb == 'float'):
            try:
                symb_value = float.fromhex(symb_value)
            except:
                symb_value = float(symb_value)
    frame, var_value = var_values(var_value)
    save_to_var(frame, var_value, symb_value, get_type_val(symb_value))

#Inštrukcia CREATEFRAME, vytvorenie dočasného rámca (TF)
def r_Createframe():
    global TF
    TF = {}

#Inštrukcia PUSHFRAME, presunutie hodnoty dočasného rámca (TF) do lokálneho rámca (LF), ak TF existuje
def r_Pushframe():
    global TF, LF
    if TF == None:
        exit_non_frame()
    LF.append(TF)
    TF = None

#Inštrukcia POPFRAME, presunutie hodnoty lokálneho rámca (LF) do dočasného rámca (TF)
def r_Popframe():
    global TF, LF
    try:
        TF = LF.pop()
    except:
        exit_non_frame()

#inštrukcia DEFVAR, vytvorenie premennej v rámci
def r_Defvar(var_value):
    frame, var_value = var_values(var_value)
    init_var(frame, var_value)

#Inštrukcia CALL, kontrola návestia, prevedenie skoku na existujúce návestie (uloženie aktuálnej pozície pre návrat z funkcie)
def r_Call(label, position):
    global functions, insts_count
    if label in labels:
        functions.append(position)
        insts_count += 1
        return labels.get(label)
    else:
        exit_semantics()

#Inštrukcia RETURN, Návrat z funkcie
def r_Return(position):
    global functions
    if not functions:
        exit_none_var()
    return functions.pop()

#Inštrukcia PUSHS, uloženie hodnoty na zásobník
def r_Pushs(symb, symb_value):
    global data
    symb = get_type(symb)
    if(symb == 'var'):
        symb, symb_value = check_var(symb, symb_value)
    if(symb == 'int'):
        data.append(int(symb_value))
    elif(symb == 'float'):
        try:
            data.append(float.fromhex(symb_value))
        except:
            data.append(symb_value)
    elif(symb == 'bool'):
        if(symb_value == 'true'):
            data.append(True)
        else:
            data.append(False)
    elif(symb == 'nil'):
        data.append(None)
    else:
        data.append(symb_value)

#Inštrukcia POPS, uloženie hodnoty zo zásobníka do definovanej premennej
def r_Pops(var, var_value):
    global data
    if(len(data) == 0):
        exit_non_frame
    if(len(data) == 0):
        exit_none_var()
    value = data.pop()
    frame, var_value = var_values(var_value)
    save_to_var(frame, var_value, value, get_type_val(value))

#Aritmetické inštrukcie ADD, SUB, MUL, IDIV a DIV (rozšírenie FLOAT), kontrola typov a uloženie výsledku operácie
def r_Arithmetics(var, type1, value1, type2, value2, sign):
    type1 = get_type(type1)
    type2 = get_type(type2)
    if(type1== 'var'):
        type1, value1 = check_var(type1, value1)
    if (type2 == 'var'):
        type2, value2 = check_var (type2, value2)
    if (type1 not in ['int', 'float'] or type2 not in ['int', 'float']):
        exit_operands()
    if(type1 == 'int' and type2 == 'int'):
        value1 = int(value1)
        value2 = int(value2)
    elif(type1 == 'float' and type2 == 'float'):
        try:
            value1 = float.fromhex(value1)
        except:
            pass
        try:
            value2 = float.fromhex(value2)
        except:
            pass
    else:
        exit_operands()
    if(sign == '+'):
        result = value1 + value2
    elif(sign == '-'):
        result = value1 - value2
    elif(sign == '*'):
        result = value1 * value2
    elif(sign == '//'):
        if(type1 != 'int' and type2 != 'int'):
            exit_operands()
        if (value2 == "0" or value2 == 0):
            exit_bad_operand()
        result = value1 // value2
    elif(sign == '/'):
        if(type1 != 'float' and type2 != 'float'):
            exit_operands()
        if (value2 == "0" or value2 == 0):
            exit_bad_operand()
        result = value1 / value2
    frame, var_value = var_values(var)
    save_to_var(frame, var_value, result, get_type_val(result))

#Relačné inštrukcie LT,GT,EQ, kontrola typov, prevedenie inštrukcií a uloženie výsledku operácie
def r_Relational_op(var, type1, value1, type2, value2, op):
    type1 = get_type(type1)
    type2 = get_type(type2)
    if(type1== 'var'):
        type1, value1 = check_var (type1, value1)     
    if (type2 == 'var'):
        type2, value2 = check_var (type2, value2)     
    if(type1 == 'nil' or type2 == 'nil'):
        if(op == "=="):
            if(value1 == value2):
                result = True
            else:
                result = False
        else:
            exit_operands()
    elif(type1== 'int'):
        if(type2 == 'int'):
            if(op == "<"):
                if(int(value1) < int(value2)):
                    result = True
                else:
                    result = False
            elif(op == ">"):
                if(int(value1) > int(value2)):
                    result = True
                else:
                    result = False
            else:
                if(int(value1) == int(value2)):
                    result = True
                else:
                    result = False
        else:
            exit_operands()
    elif(type1 == 'float'):
        if(type2 == 'float'):
            if(op == "<"):
                try:
                    value1 = float.fromhex(value1)
                except:
                    pass
                try:
                    value2 = float.fromhex(value2)
                except:
                    pass
                if(value1 < value2):
                    result = True
                else:
                    result = False
            elif(op == ">"):
                if(value1 > value2):
                    result = True
                else:
                    result = False
            else:
                if(value1 == value2):
                    result = True
                else:
                    result = False
        else:
            exit_operands()    
    elif(type1 == 'bool'):
        if(type2 == 'bool'):
            value1, value2 = convert_to_bool(value1,value2)
            if(op == "<"):
                if(value1 == False):
                    if(value2 == True):
                        result = True
                    else:
                        result = False
                else:
                    result = False
            elif(op == ">"):
                if(value1 == True):
                    if(value2 == False):
                        result = True
                    else:
                        result = False
                else:
                    result = False      
            else:
                if(value1 == value2):
                    result = True
                else:
                    result = False                      
        else:
            exit_operands()
    elif(type1 in ['string', 'read'] and type2 in ['string', 'read'] ):
        value1, value2 = read_vs_string(type1, value1, type2, value2)
        if(op == "<"):
            if(value1 < value2):
                result = True
            else:
                result = False
        elif(op == ">"):
            if(value1 > value2):
                result = True
            else:
                result = False
        else:
            if(value1 == value2):
                result = True
            else:
                result = False        
    else:
        exit_operands()
    frame, var_value = var_values(var)
    save_to_var(frame, var_value, result, get_type_val(result))  

#Logické inštrukcie AND a OR, kontrola typov, prevedenie inštrukcie a uloženie výsledku do definovanej premennej
def r_Logical_op(var, type1, value1, type2, value2, op):
    type1 = get_type(type1)
    type2= get_type(type2)
    if(type1== 'var'):
        type1, value1 = check_var (type1, value1)     
    if (type2 == 'var'):
        type2, value2 = check_var (type2, value2)     
    if(type1 not in ['bool', 'var'] or type2 not in ['bool', 'var']): 
        exit_operands()
    value1, value2 = convert_to_bool(value1, value2)  
    if(op == "&&"):
        result = value1 and value2
    elif(op == "||"):
        result = value1 or value2
    frame, var_value = var_values(var)
    save_to_var(frame, var_value, result, get_type_val(result))  

#Inštrukcia NOT, kontrola typu, prevedenie inštrukcie a uloženie výsledku do definovanej premennej
def r_Not(var, type1, value1):
    type1 = get_type(type1)
    if(type1== 'var'):
        type1, value1 = check_var (type1, value1)     
    if(type1 not in ['bool', 'var']): 
        exit_operands()
    value1, value2 = convert_to_bool(value1, None)
    if(type(value1) is not bool):
        exit_operands()
    result = not value1
    frame, var_value = var_values(var)
    save_to_var(frame, var_value, result, get_type_val(result))

#Inštrukcia INT2CHAR, prevedenie čísla do jeho jednoznakovej podoby (podla UNICODE) a uloženie do premennej
def r_Int2char(var, type1, value1):
    type1 = get_type(type1)
    if(type1== 'var'):
        type1, value1 = check_var (type1, value1)     
    if(type1 not in ['int', 'var']):
        exit_operands()
    try:
        result = chr(int(value1))
    except:
        exit_string()
    frame, var_value = var_values(var)
    save_to_var(frame, var_value, result, get_type_val(result))

#uloženie ordinálnej hodnoty znaku v reťazci value1, na pozícii value2 do premennej var
def r_Stri2int(var, type1, value1, type2, value2):
    type1 = get_type(type1)
    type2= get_type(type2)
    if(type1== 'var'):
        type1, value1 = check_var (type1, value1)     
    if (type2 == 'var'):
        type2, value2 = check_var (type2, value2)     
    if(type1 not in ['string', 'var', 'read'] or type2 != 'int'):
        exit_operands()
    value1, unused =  read_vs_string(type1, value1, None, None)
    if 0<= int(value2) <len(value1):
        char_ord = [ord(i) for i in value1]
        result = char_ord[int(value2)]
    else:
        exit_string()        
    frame, var_value = var_values(var)
    save_to_var(frame, var_value, result, get_type_val(result))

#Inštrukcia READ, načítanie hodnoty na základe definovaného typu a uloženie do premennej. Použitie vstavanej funkcie input()
def r_Read(var, type1, type_val):
    if not arg_input:
        inputi = input()
    else:
        inputi = input_f.readline()
    try:
        if (inputi[len(inputi)-1] == "\n"):
            inputi = inputi[:-1]
        if(type_val == 'int'):
            try:
                result = int(inputi)
            except:
                result = None
        elif(type_val == 'float'):
            try:
                result = float.fromhex(inputi)
            except:
                result = None
        elif(type_val == 'string'):
            try:
                result = str(inputi)
                if(result):
                    result = {'read': result}
            except:
                print("t")
                result = None
        elif(type_val == 'bool'):
            if(inputi.upper()!= "TRUE"):
                result = False
            else:
                result = True    
    except:
        result = None
    frame, var_value = var_values(var)
    save_to_var(frame, var_value, result, get_type_val(result))

#Inštrukcia WRITE,  vypísanie hodnoty na STDOUT, využitie doplňujúceho parametra end="", kvôli zamedzaniu odriadkovania
def r_Write(type1, type_value):
    type1 = get_type(type1)
    if(type1== 'var'):
        type1, type_value = check_var (type1, type_value)
    if(type1 == 'bool'):
        type_value, type_value2 = convert_to_bool(type_value, None)
        if(type_value == True):
            print("true", end="")
        else:
            print("false", end="")
    elif(type1 == "int"):
        print(int(type_value), end="")
    elif(type1 == "float"):
        if(type(type_value)is not float):
            type_value = float.fromhex(type_value)
        print(float.hex(type_value), end="")
    elif(type1 == "string"):
        type_value = interpret_escape(type_value)
        print(type_value, end="")
    elif(type1 == "nil"):
        print("", end="")
    elif(type1 == 'read'):
        print(type_value.get('read'), end="")
    else:
        exit_bad_operand()

#Inštrukcia CONCAT, spojenie 2 reťazcov a uloženie do definovanej premennej
def r_Concat(var, type1, value1, type2, value2):
    type1 = get_type(type1)
    type2 = get_type(type2)
    if(type1== 'var'):
        type1, value1 = check_var (type1, value1)     
    if (type2 == 'var'):
        type2, value2 = check_var (type2, value2)     
    if(type1 not in ['string', 'read'] or type2 not in ['string', 'read']):
        exit_operands()
    if(value1 == None):
        value1 = ""
    if(value2 == None):
        value2 = ""
    value1, value2 = read_vs_string(type1, value1, type2, value2)
    result = value1 + value2 
    frame, var_value = var_values(var)
    save_to_var(frame, var_value, result, get_type_val(result))

#Inštrukcia STRLEN, zistenie počtu znakov v reťazci a uloženie výsledku (celé číslo) do definovanej premennej
def r_Strlen(var, type1, value1):
    type1 = get_type(type1)
    if(type1== 'var'):
        type1, value1 = check_var (type1, value1)     
    if(type1 not in ['string', 'read']):
        exit_operands()
    if (value1 == None):
        value1 = ""
    value1, value2 = read_vs_string(type1, value1, None, None)
    result = len(value1)     
    frame, var_value = var_values(var)
    save_to_var(frame, var_value, result, get_type_val(result))

#Inštrukcia GETCHAR, uloženie jednoznakového reťazca v reťazci value1 na pozícii value2 do definovanej premennej
def r_Getchar(var, type1, value1, type2, value2):
    type1 = get_type(type1)
    type2 = get_type(type2)
    if(type1== 'var'):
        type1, value1 = check_var (type1, value1)    
    if (type2 == 'var'):
        type2, value2 = check_var (type2, value2)    
    if(type1 not in ['string', 'read'] or type2 != 'int'):
        exit_operands()
    value1, unused = read_vs_string(type1, value1, None, None)
    if 0 <= int(value2) <len(value1):
        result = list(value1)
        result = result[int(value2)]
    else:
        exit_string()
    frame, var_value = var_values(var)
    save_to_var(frame, var_value, result, get_type_val(result))

#Inštrukcia SETCHAR, modifikovanie znaku reťazca var_value, na pozícii value1 na znak v reťazci value2 (ak obsahuje viac znakov, tak prvý znak) a uloženie do definovanej premennej
def r_Setchar(var_type, var_value, type1, value1, type2, value2):
    type1 = get_type(type1)
    type2 = get_type(type2)
    var_type = get_type(var_type)
    if(type1== 'var'):
        type1, value1 = check_var (type1, value1)    
    if (type2 == 'var'):
        type2, value2 = check_var (type2, value2)    
    if(var_type == 'var'):
        var_value_result = symb_from_var(var_value)
        if(var_value_result == None):
            exit_none_var()
        var_type = get_type_val(var_value_result)
    else:
        exit_operands()
    if(type1 != 'int' or type2 not in ['string', 'read'] or var_type not in ['string', 'read']):
            exit_operands()
    if(var_value_result == "" or value2 == "" or value2 == None):
        exit_string()
    value2, var_value_result = read_vs_string(type2, value2, var_type, var_value_result)
    if (0<= int(value1) <len(var_value_result)):
        result = var_value_result[:int(value1)]+value2[0]+var_value_result[int(value1) + 1:]
    else:
        exit_string()
    frame, var_value = var_values(var_value)
    save_to_var(frame, var_value, result, get_type_val(result))

#Inštrukcia TYPE, zistenie typu symbolu a uloženie výsledného reťazca do definovanej premennej
def r_Type(var, type1, value1):
    type1 = get_type(type1)
    if(type1== 'var'):
        value1 = symb_from_var(value1)
        type1 = get_type_val(value1)
    if (type1 == 'int'):
        result = 'int'
    elif(type1 == 'float'):
        result = 'float'
    elif(type1 == 'string' or type1 == 'read'):
        result = 'string'
    elif(type1 == 'bool'):
        result = 'bool'
    elif(type1 == 'nil'):
        result = 'nil'
    elif(type1 == None):
        result = None
    else:
        exit_operands()
    frame, var_value = var_values(var)
    save_to_var(frame, var_value, result, get_type_val(result))

#Inštrukcia JUMP, prevedenie skoku na existujúce návestie
def r_Jump(label):
    global labels, insts_count
    if label not in labels.keys():
        exit_semantics()
    else:
        insts_count += 1
        return labels[label]

#Relačné inštrukcie skoku JUMPIFEQ a JUMPIFNEQ, zistenie typu, vyhodnotenie podmienky a následné prevedenie/neprevedenie skoku
def r_Relational_jumps(label, type1, value1, type2, value2, position, op):
    global labels, insts_count
    type1 = get_type(type1)
    type2 = get_type(type2)
    if(type1== 'var'):
        type1, value1 = check_var (type1, value1)   
    if (type2 == 'var'):
        type2, value2 = check_var (type2, value2)   
    if(type1 == 'int'):
        value1 = int(value1)
    if (type2 == 'int'):
        value2 = int(value2)
    if(type1 == 'float'):
        try:
            value1 = float.fromhex(value1)
        except:
            pass
    if(type2 == 'float'):
        try:
            value2 = float.fromhex(value2)
        except:
            pass
    if(type1 == 'bool'):
        if (value1 == True or value1 == 'true'):
            value1 = True
        else:
            value1 = False
    if(type2 == 'bool'):
        if (value2 == True or value2 == 'true'):
            value2 = True
        else:
            value2 = False   
    value1, value2 = read_vs_string(type1, value1, type2, value2)
    if(type1 == 'read'):
        type1 = 'string'
    if(type2 == 'read'):
        type2 = 'string'
    if label not in labels.keys():
        exit_semantics()
    if((type1 == 'nil' or type2 == 'nil') or (type1 == type2)):
        if(op == "=="):
            if(value1 == value2):
                insts_count += 1
                return labels[label]
            else:
                return position
        elif(op == "!="):
            if(value1 != value2):
                insts_count += 1
                return labels[label]
            else:
                return position
    else:
        exit_operands()

#Inštrukcia DPRINT, vypísanie zadanej hodnoty na STDERR
def r_Dprint(type1, value1):
    type1 = get_type(type1)
    if(type1== 'var'):
        value1 = symb_from_var(value1)
        if(value1 == None):
            exit_none_var()
        sys.stderr.write(value1)
    else:
        sys.stderr.write(value1)

#Inštrukcia BREAK, vypísanie stavu interpreteru (pozícia v kóde, obsah rámcov) na STDERR
def r_Break(position):
    global LF, TF, GF
    sys.stderr.write("\nPozicia:{0}\nLF:{1}\nTF:{2}\nGF:{3}\n".format(position+1, LF, TF, GF))

#Inštrukcia EXIT, ukončenie vykonávania programu a ukončenie interpretera  so zadaným návratovým kódom (od 0 do 49)
def r_Exit(type1, value1):
    type1 = get_type(type1)
    if(type1== 'var'):
        type1, value1 = check_var (type1, value1)   
    if(type1 != 'int'):
        exit_operands()
    if(0 <= int(value1) <= 49):
        exit(int(value1))
    else:
        exit_bad_operand()
 
 #ROZŠÍRENIE STACK
 #Získanie hodnoty z vrcholu zásobníka
def get_stack_value():
    if(len(data)!= 0):
        return data.pop()
    else:
        exit_none_var()

#Získanie typu hodnoty z vrcholu zásobníka
def get_stack_types():
    value2 = get_stack_value()
    value1 = get_stack_value()
    type1 = get_type_val(value1)
    if(type1 == None):
        type1 = 'nil'
    type2 = get_type_val(value2)
    if(type2 == None):
        type2 ='nil'
    return(value1, value2, type1, type2)

#Inštrukcia CLEARS, vyčistenie zásobníka
def r_Clears():
    global data
    data = []

#Verzia aritmetických inštrukcii pre prácu so zásobníkom
def r_Arithmetics_S(sign):
    global data
    value1, value2, type1, type2 = get_stack_types()
    if(type1== 'var'):
        type1, value1 = check_var(type1, value1)
    if (type2 == 'var'):
        type2, value2 = check_var (type2, value2)
    if (type1 not in ['int', 'float'] or type2 not in ['int', 'float']):
        exit_operands()
    if(type1 == 'int' and type2 == 'int'):
        value1 = int(value1)
        value2 = int(value2)
    elif(type1 == 'float' and type2 == 'float'):
        try:
            value1 = float.fromhex(value1)
        except:
            pass
        try:
            value2 = float.fromhex(value2)
        except:
            pass
    else:
        exit_operands()
    if(sign == '+'):
        result = value1 + value2
    elif(sign == '-'):
        result = value1 - value2
    elif(sign == '*'):
        result = value1 * value2
    elif(sign == '//'):
        if(type1 != 'int' and type2 != 'int'):
            exit_operands()
        if (value2 == "0" or value2 == 0):
            exit_bad_operand()
        result = value1 // value2
    elif(sign == '/'):
        if(type1 != 'float' and type2 != 'float'):
            exit_operands()
        if (value2 == "0" or value2 == 0):
            exit_bad_operand()
        result = value1 / value2
    data.append(result)    

#Verzia relačných inštrukcií pre prácu so zásobníkom
def r_Relational_op_S(op):
    global data
    value1, value2, type1, type2 = get_stack_types()
    if(type1== 'var'):
        type1, value1 = check_var (type1, value1)     
    if (type2 == 'var'):
        type2, value2 = check_var (type2, value2)     
    if(type1 == 'nil' or type2 == 'nil'):
        if(op == "=="):
            if(value1 == value2):
                result = True
            else:
                result = False
        else:
            exit_operands()
    elif(type1== 'int'):
        if(type2 == 'int'):
            if(op == "<"):
                if(int(value1) < int(value2)):
                    result = True
                else:
                    result = False
            elif(op == ">"):
                if(int(value1) > int(value2)):
                    result = True
                else:
                    result = False
            else:
                if(int(value1) == int(value2)):
                    result = True
                else:
                    result = False
        else:
            exit_operands()
    elif(type1 == 'float'):
        if(type2 == 'float'):
            if(op == "<"):
                try:
                    value1 = float.fromhex(value1)
                except:
                    pass
                try:
                    value2 = float.fromhex(value2)
                except:
                    pass
                if(value1 < value2):
                    result = True
                else:
                    result = False
            elif(op == ">"):
                if(value1 > value2):
                    result = True
                else:
                    result = False
            else:
                if(value1 == value2):
                    result = True
                else:
                    result = False
        else:
            exit_operands()    
    elif(type1 == 'bool'):
        if(type2 == 'bool'):
            value1, value2 = convert_to_bool(value1,value2)
            if(op == "<"):
                if(value1 == False):
                    if(value2 == True):
                        result = True
                    else:
                        result = False
                else:
                    result = False
            elif(op == ">"):
                if(value1 == True):
                    if(value2 == False):
                        result = True
                    else:
                        result = False
                else:
                    result = False      
            else:
                if(value1 == value2):
                    result = True
                else:
                    result = False                      
        else:
            exit_operands()
    elif(type1 in ['string', 'read'] and type2 in ['string', 'read'] ):
        value1, value2 = read_vs_string(type1, value1, type2, value2)
        if(op == "<"):
            if(value1 < value2):
                result = True
            else:
                result = False
        elif(op == ">"):
            if(value1 > value2):
                result = True
            else:
                result = False
        else:
            if(value1 == value2):
                result = True
            else:
                result = False        
    else:
        exit_operands()
    data.append(result)

#Verzia logických inštrukcii pre prácu so zásobníkom
def r_Logical_op_S(op):
    global data
    value1, value2, type1, type2 = get_stack_types()
    if(type1== 'var'):
        type1, value1 = check_var (type1, value1)     
    if (type2 == 'var'):
        type2, value2 = check_var (type2, value2)     
    if(type1 not in ['bool', 'var'] or type2 not in ['bool', 'var']): 
        exit_operands()
    value1, value2 = convert_to_bool(value1, value2)  
    if(op == "&&"):
        result = value1 and value2
    elif(op == "||"):
        result = value1 or value2
    data.append(result)

#Inštrukcia NOTS, verzia inštrukcie NOT pre prácu so zásobníkom
def r_Nots():
    global data
    value1 = get_stack_value()
    type1 = get_type_val(value1)
    if(type1== 'var'):
        type1, value1 = check_var (type1, value1)     
    if(type1 not in ['bool', 'var']): 
        exit_operands()
    value1, value2 = convert_to_bool(value1, None)
    if(type(value1) is not bool):
        exit_operands()
    result = not value1
    data.append(result)

#Inštrukcia INT2CHARS, verzia inštrukcie INT2CHAR pre prácu so zásobníkom
def r_Int2chars():
    global data
    value1 = get_stack_value()
    type1 = get_type_val(value1)
    if(type1== 'var'):
        type1, value1 = check_var (type1, value1)     
    if(type1 not in ['int', 'var']):
        exit_operands()
    try:
        result = chr(int(value1))
    except:
        exit_string()
    data.append(result)

#Inštrukcia STRI2INTS, verzia inštrukcie STRI2INT pre prácu so zásobníkom
def r_Stri2ints():
    global data
    value1, value2, type1, type2 = get_stack_types()
    if(type1== 'var'):
        type1, value1 = check_var (type1, value1)     
    if (type2 == 'var'):
        type2, value2 = check_var (type2, value2)     
    if(type1 not in ['string', 'var', 'read'] or type2 != 'int'):
        exit_operands()
    value1, unused =  read_vs_string(type1, value1, None, None)
    if 0<= int(value2) <len(value1):
        char_ord = [ord(i) for i in value1]
        result = char_ord[int(value2)]
    else:
        exit_string()        
    data.append(result)

#Verzia relačných inštrukcií skoku pre prácu so zásobníkom
def r_Relational_jumps_S(label, position, op):
    global labels, insts_count, data
    value1, value2, type1, type2 = get_stack_types()
    if(type1== 'var'):
        type1, value1 = check_var (type1, value1)   
    if (type2 == 'var'):
        type2, value2 = check_var (type2, value2)   
    if(type1 == 'int'):
        value1 = int(value1)
    if (type2 == 'int'):
        value2 = int(value2)
    if(type1 == 'float'):
        try:
            value1 = float.fromhex(value1)
        except:
            pass
    if(type2 == 'float'):
        try:
            value2 = float.fromhex(value2)
        except:
            pass
    if(type1 == 'bool'):
        if (value1 == True or value1 == 'true'):
            value1 = True
        else:
            value1 = False
    if(type2 == 'bool'):
        if (value2 == True or value2 == 'true'):
            value2 = True
        else:
            value2 = False   
    value1, value2 = read_vs_string(type1, value1, type2, value2)
    if(type1 == 'read'):
        type1 = 'string'
    if(type2 == 'read'):
        type2 = 'string'
    if label not in labels.keys():
        exit_semantics()
    if((type1 == 'nil' or type2 == 'nil') or (type1 == type2)):
        if(op == "=="):
            if(value1 == value2):
                insts_count += 1
                return labels[label]
            else:
                return position
        elif(op == "!="):
            if(value1 != value2):
                insts_count += 1
                return labels[label]
            else:
                return position
    else:
        exit_operands()

#Rozšírenie FLOAT
#Inštrukcia INT2FLOAT, prevedenie int hodnoty na hodnotu float, kontrola typov a uloženie výsledku do definovanej premennej
def r_Int2float(var, type1, value1):
    type1 = get_type(type1)
    if(type1== 'var'):
        type1, value1 = check_var (type1, value1)     
    if(type1 not in ['int', 'var']):
        exit_operands()
    try:
        result = float(value1)
    except:
        exit_operands()
    frame, var_value = var_values(var)
    save_to_var(frame, var_value, result, get_type_val(result))

#Inštrukcia FLOAT2INT, prevedenie float hodnoty na hodnotu int, kontrola typov a uloženie výsledku do definovanej premennej
def r_Float2int(var, type1, value1):
    type1 = get_type(type1)
    if(type1== 'var'):
        type1, value1 = check_var (type1, value1)     
    if(type1 not in ['float', 'var']):
        exit_operands()
    try:
        result = int(value1)
    except:
        exit_operands()
    frame, var_value = var_values(var)
    save_to_var(frame, var_value, result, get_type_val(result))

#Rozšírenie STATI
#Spočítanie premenných v rámcoch
def count_vars():
    global TF, LF, GF
    if(TF == None):
        len_TF = 0
    else:
        len_TF = len(TF)
    return len(GF) + len_TF + len(LF)

#Cyklus cez všetky "OPCODE" (Inštrukcie), uloženie argumentov a zavolanie príslušných funkcií
def run_instructions(xml):
    global insts_count, vars_count
    position = 0
    while position < len(xml):
        instruction = xml[position]
        opcode = instruction.attrib['opcode']
        opcode = opcode.upper()
        try:
            arg1 = instruction[0].attrib
            arg1_text = instruction[0].text
        except:
            pass
        try:
            arg2 = instruction[1].attrib
            arg2_text = instruction[1].text
        except:
            pass        
        try:
            arg3 = instruction[2].attrib
            arg3_text = instruction[2].text
        except:
            pass
        if (opcode == "MOVE"):
            r_Move(arg1, arg2, arg1_text, arg2_text)
        elif (opcode == "CREATEFRAME"):
            r_Createframe()
        elif (opcode == "PUSHFRAME"):
            r_Pushframe()
        elif (opcode == "POPFRAME"):
            r_Popframe()
        elif (opcode == "DEFVAR"):
            r_Defvar(arg1_text)
        elif (opcode == "CALL"):
            position = r_Call(arg1_text, position) - 1
        elif (opcode == "RETURN"):
            position = r_Return(position)
        elif (opcode == "EXIT"):
            r_Exit(arg1, arg1_text)
        elif (opcode == "PUSHS"):
            r_Pushs(arg1, arg1_text)
        elif (opcode == "POPS"):
            r_Pops(arg1,arg1_text)
        elif (opcode == "ADD"):
            r_Arithmetics(arg1_text, arg2, arg2_text, arg3,  arg3_text, '+')
        elif (opcode == "SUB"):
            r_Arithmetics(arg1_text, arg2, arg2_text, arg3, arg3_text, '-')
        elif (opcode == "MUL"):
            r_Arithmetics(arg1_text, arg2, arg2_text, arg3, arg3_text, '*')
        elif (opcode == "IDIV"):
            r_Arithmetics(arg1_text, arg2, arg2_text, arg3, arg3_text, '//')
        elif (opcode == "LT"):
            r_Relational_op(arg1_text, arg2, arg2_text, arg3, arg3_text, "<")
        elif (opcode == "GT"):
            r_Relational_op(arg1_text, arg2, arg2_text, arg3, arg3_text, ">")
        elif (opcode == "EQ"):
            r_Relational_op(arg1_text, arg2, arg2_text, arg3, arg3_text, "==")
        elif (opcode == "AND"):
            r_Logical_op(arg1_text, arg2, arg2_text, arg3, arg3_text, "&&")
        elif (opcode == "OR"):
            r_Logical_op(arg1_text, arg2, arg2_text, arg3, arg3_text, "||")
        elif (opcode == "NOT"):
            r_Not(arg1_text, arg2, arg2_text)
        elif (opcode == "INT2CHAR"):
            r_Int2char(arg1_text, arg2, arg2_text)
        elif (opcode == "STRI2INT"):
            r_Stri2int(arg1_text, arg2, arg2_text, arg3, arg3_text)
        elif (opcode == "READ"):
            r_Read(arg1_text, arg2, arg2_text)
        elif (opcode == "WRITE"):
            r_Write(arg1, arg1_text)
        elif (opcode == "CONCAT"):
            r_Concat(arg1_text, arg2, arg2_text, arg3, arg3_text)
        elif (opcode == "STRLEN"):
            r_Strlen(arg1_text, arg2, arg2_text)
        elif (opcode == "GETCHAR"):
            r_Getchar(arg1_text, arg2, arg2_text, arg3, arg3_text)
        elif (opcode == "SETCHAR"):
            r_Setchar(arg1, arg1_text, arg2, arg2_text, arg3, arg3_text)
        elif (opcode == "TYPE"):
            r_Type(arg1_text, arg2, arg2_text)
        elif (opcode == "JUMP"):
            position = r_Jump(arg1_text)
        elif (opcode == "JUMPIFEQ"):
            position = r_Relational_jumps(arg1_text, arg2, arg2_text, arg3, arg3_text, position, "==")
        elif (opcode == "JUMPIFNEQ"):
            position = r_Relational_jumps(arg1_text, arg2, arg2_text, arg3, arg3_text, position, "!=")
        elif (opcode == "DPRINT"):
            r_Dprint(arg1, arg1_text)
        elif (opcode == "BREAK"):
            r_Break(position)
        elif(opcode == "CLEARS"):
            r_Clears()
        elif(opcode == "ADDS"):
            r_Arithmetics_S("+")
        elif(opcode == "SUBS"):
            r_Arithmetics_S("-")
        elif(opcode == "MULS"):
            r_Arithmetics_S("*")
        elif(opcode == "IDIVS"):
            r_Arithmetics_S("//")
        elif(opcode == "LTS"):
            r_Relational_op_S("<")
        elif(opcode == "GTS"):
            r_Relational_op_S(">")
        elif(opcode == "EQS"):
            r_Relational_op_S("==")
        elif(opcode == "ANDS"):
            r_Logical_op_S("&&")
        elif(opcode == "ORS"):
            r_Logical_op_S("||")
        elif(opcode == "NOTS"):
            r_Nots()
        elif(opcode == "INT2CHARS"):
            r_Int2chars()
        elif(opcode == "STRI2INTS"):
            r_Stri2ints()
        elif(opcode == "JUMPIFEQS"):
            position = r_Relational_jumps_S(arg1_text, position, "==")
        elif(opcode == "JUMPIFNEQS"):
            position = r_Relational_jumps_S(arg1_text, position, "!=")
        elif(opcode == "DIV"):
            r_Arithmetics(arg1_text, arg2, arg2_text, arg3, arg3_text, "/")
        elif(opcode == "INT2FLOAT"):
            r_Int2float(arg1_text, arg2, arg2_text)
        elif(opcode == "FLOAT2INT"):
            r_Float2int(arg1_text, arg2, arg2_text)
        #počítanie inštrukcií pre rozšírenie STATI, a ukladanie maximálneho počtu definovaných premenných v rámcoch
        insts_count+=1
        old_vars = vars_count
        vars_count = count_vars()
        if(vars_count > old_vars):
            vars_count = vars_count
        else:
            vars_count = old_vars
        position+=1

#Vypísanie štatistík (rozšírenie STATI) do zadaného súboru, každé na nový riadok (vždy len 1 číslo)
def print_stats():
    global insts_count, vars_count, file_stats, arg_insts, arg_vars
    try:
        with open(file_stats, "w") as stats_f:
            for arg in sys.argv:
                if(arg == "--insts"):
                    stats_f.write(str(insts_count))
                    stats_f.write("\n")
                if(arg == "--vars"):
                    stats_f.write(str(vars_count))
                    stats_f.write("\n")
    except:
            sys.stderr.write('Cannot open desired file\n')
            sys.exit(12)

#Kontrola argumentov skriptu, uloženie príslušných súborov
arg_input = arg_source = arg_stats = arg_insts = arg_vars = False
if ('--help' in sys.argv):
    if ((len(sys.argv))!=2):
        sys.stderr.write("Invalid help combination\n")
        sys.exit(10)
    else:
        print_help()
else:
    sys.argv.pop(0)
    for arg in sys.argv:
        if(arg.startswith('--source=')):
            file_source = arg.split('=')
            file_source = file_source[1]
            arg_source = True
        elif(arg.startswith('--input=')):
            file_input = arg.split('=')
            file_input = file_input[1]
            arg_input = True
        elif(arg.startswith('--stats=')):
            file_stats = arg.split('=')
            file_stats = file_stats[1]
            arg_stats = True
        elif(arg == '--insts'):
            arg_insts = True
        elif(arg == '--vars'):
            arg_vars = True
        else:
            sys.stderr.write('Invalid argument\n')
            sys.exit(10)

#Argument pre počet inštrukcii a pre počet premenných bol zadaný bez argumentu pre výstupný súbor (STATI)
if(arg_insts ==True or arg_vars == True):
    if(arg_stats == False):
        sys.stderr.write('you cant have argument --insts or --vars without --stats=file\n')
        sys.exit(10)

#Argument pre vstup a ani pre výstup nebol zadaný -> chyba, inak otvorenie a kontrola zadaných vstupov, parsovanie XML na vstupe
if (arg_input == False and arg_source == False):
    sys.stderr.write('you need to give at least one valid argument\n')
    sys.exit(10)
else:
    if(arg_input):
        try:
            input_f = open(file_input, "r")
        except:
            sys.stderr.write('Cannot open desired file\n')
            sys.exit(11)
    else:
        input_f = sys.stdin
    if (arg_source):
        try:
            source_f = open(file_source, "r")
        except:
            sys.stderr.write('Cannot open desired file\n')
            sys.exit(11)
        try:
            xml_raw = ET.parse(source_f)
            source_f.close()
        except:
            sys.stderr.write('xml not well-formed\n')
            source_f.close()
            sys.exit(31)
    else:
        source_f = sys.stdin
        try:
            xml_raw = ET.parse(source_f)
        except:
            sys.stderr.write('xml not well-formed\n')
            sys.exit(31)

xml = xml_raw.getroot()
xml = check_xml_root(xml)
run_instructions(xml)
if(arg_stats):
    print_stats()