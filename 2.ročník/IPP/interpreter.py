import sys
import argparse
import re
import xml.etree.ElementTree as ET

pattern = re.compile(r'\s+')
var_pattern = re.compile(r'^(LF|TF|GF)@[a-zA-Z_\-$&%*!?][a-zA-Z0-9_\-$&%*?!]*$')
sym_pattern = re.compile(r'^int@[-+]?[0-9]+$|^bool@true$|^bool@false$|^nil@nil$')
label_pattern = re.compile(r'^[a-zA-Z_\-$&%*!?][a-zA-Z0-9_\-$&%*!?]*$')
escape_pattern = re.compile(r'\\[0-9]{3}')
labels = {}
functions = []
data = []
TF = None
GF = {}
LF = []

zero_ins = ["CREATEFRAME", "PUSHFRAME", "POPFRAME", "RETURN", "BREAK"]
one_ins = ["DEFVAR", "CALL", "PUSHS", "POPS", "WRITE", "LABEL", "JUMP", "EXIT", "DPRINT"]
two_ins = ["MOVE", "INT2CHAR", "READ", "STRLEN", "TYPE", "NOT"]
three_ins = ["ADD", "SUB", "MUL", "IDIV", "LT", "GT", "EQ", "AND", "OR", "STRI2INT",
    "CONCAT", "GETCHAR", "SETCHAR", "JUMPIFEQ", "JUMPIFNEQ"]

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

def print_help():
    print("TODO HELP")
    sys.exit(0)

def check_xml_root(xml):
    if not 'language' in xml.attrib:
        exit_xml()
    if (xml.tag != 'program' or xml.attrib['language'] != "IPPcode20"):
        exit_xml()
    for attrib in xml.attrib:
        if(attrib not in ('name', 'language', 'description')):
            exit_xml
    xml.text = re.sub(pattern, '', xml.text)
    if (xml.text):
        exit_xml()
    if (xml.tail !=  None):
        exit_xml()
    xml = check_xml_ins(xml)
    return xml

def check_xml_ins(xml):
    order = 0
    sort_ins = []
    for ins in xml:
        if(ins.tag != 'instruction'):
            exit_xml()
        if ('order' not in ins.attrib or 'opcode' not in ins.attrib):
            exit_xml()
        order = int(ins.attrib['order'])
        sort_ins.append((order,ins))
    try:
        sort_ins.sort()
    except:
        exit_xml()
    if(sort_ins[0][0] != 1):
        exit_xml()
    xml[:] = [item[-1] for item in sort_ins]
    execute_ins(xml)
    return xml

def var_check(text):
    if not (re.match(var_pattern, text)):
        exit_xml()

def sym_check(type, text):
    if (type == 'var'):
        if not (re.match(var_pattern, text)):
            exit_xml()
    elif (type == 'string'):
        backslash = re.findall(r'\\', text)
        if(backslash):
            escape = re.findall(r'\\[0-9]{3}', text)
            if (len(escape)!= len(backslash)):
                exit_xml()
    else:
        text = type + '@' + text
        if not (re.match(sym_pattern, text)):
            exit_xml()

def label_check(text):
    if not (re.match(label_pattern, text)):
        exit_xml()

def type_check(text):
    if not (re.match(r'^int$|^bool$|^string$', text)):
        exit_xml()

def zero_arg_check(ins):
    if(len(ins) != 0):
        exit_xml()

def one_arg_check(ins, i):
    if(len(ins) != 1):
        exit_xml()
    for arg in ins:
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
        if(arg.attrib['type'] in ["int", "bool", "string", "var", "nil"]):
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

def two_arg_check(ins):
    if(ins[0].tag != 'arg1' or ins[1].tag != 'arg2'):
        exit_xml()
    if(len(ins) != 2):
        exit_xml()
    for arg in ins:
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
                if(arg.attrib['type'] in ["int", "bool", "string", "var", "nil"]):
                    sym_check(arg.attrib['type'], arg.text)
                else:
                    exit_xml()


def three_arg_check(ins):
    if(ins[0].tag != 'arg1' or ins[1].tag != 'arg2' or ins[2].tag != 'arg3'):
        exit_xml()
    if(len(ins)!=3):
        exit_xml()
    for arg in ins:
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
            if(arg.attrib['type'] in ["int", "bool", "string", "var", "nil"]):
                sym_check(arg.attrib['type'], arg.text)
            else:
                    exit_xml()



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

def get_type_val(value1):
    if type(value1) is bool:
       return 'bool'
    if type(value1) is int:
        return 'int'
    if type(value1) is str:
        return 'string'

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


def interpret_escape(escape):
    def replace(match):
        return chr(int(match.group()[1:]))
    escape = re.sub(escape_pattern, replace, escape)
    return escape

def var_values(var):
    frame, var_value = var.split("@", 1)
    return frame, var_value

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
            elif(symb_type == 'string'):
                TF[var] = symb
            elif(symb_type == 'bool'):
                if(symb == 'true' or symb == True):
                    TF[var] = True
                else:
                    TF[var] = False
    elif (frame == "LF"):
        if not LF:
            exit_non_frame()
        elif var not in LF[-1].keys():
            exit_non_var()
        else:
            if(symb_type == 'int'):
                LF[-1][var]  = int(symb)
            elif(symb_type == 'string'):
                LF[-1][var]  = symb
            elif(symb_type == 'bool'):
                if(symb == 'true' or symb == True):
                    LF[-1][var] = True
                else:
                    LF[-1][var] = False
    else:
        if var not in GF.keys():
            exit_non_var()
        else:
            if(symb_type == 'int'):
                GF[var] = int(symb)
            elif(symb_type == 'string'):
                GF[var] = symb
            elif(symb_type == 'bool'):
                if(symb == 'true' or symb == True):
                    GF[var] = True
                else:
                    GF[var] = False

def init_var(frame, var):
    if (frame == "TF"):
        if TF == None:
            exit_non_frame()
        elif var in TF.keys():
            exit_semantics
        else:
            TF[var] = None
    elif (frame == "LF"):
        if not LF:
            exit_non_frame()
        elif var in LF[-1].keys():
            exit_semantics
        else:
            LF[-1][var] = None
    else:
        if var in GF.keys():
            exit_semantics
        else:
            GF[var] = None

def symb_from_var(symb):
    frame, var_value = var_values(symb)
    if(frame == "TF"):
        if (TF == None):
            exit_non_frame()
        try:
            return TF.get(var_value)
        except:
            exit_non_var()
    elif(frame == "LF"):
        if not LF:
            exit_non_frame()
        try:
            return LF.get(var_value)
        except:
            exit_non_var()
    else:
        try:
            return GF.get(var_value)
        except:
            exit_non_var()


def r_Move(var, symb, var_value, symb_value):
    symb = get_type(symb)
    if (symb == 'var'):
        symb_value = symb_from_var(symb_value)
        if (symb_value == None):
            exit_none_var()
    else:
        if(symb == 'string'):
            symb_value = interpret_escape(symb_value)
    frame, var_value = var_values(var_value)
    save_to_var(frame, var_value, symb_value, symb)

def r_Createframe():
    global TF
    TF = {}

def r_Pushframe():
    global TF, LF
    if not TF:
        exit_non_frame()
    LF.append(TF)
    TF = None

def r_Popframe():
    global TF, LF
    try:
        TF = LF.pop()
    except:
        exit_non_frame()

def r_Defvar(var_value):
    frame, var_value = var_values(var_value)
    init_var(frame, var_value)

def r_Call(label, position):
    global functions
    if label in labels:
        functions.append(position)
        return labels.get(label)
    else:
        exit_semantics()

def r_Return(position):
    global functions
    if not functions:
        exit_none_var()
    return functions.pop()

def r_Pushs(symb, symb_value):
    global data
    symb = get_type(symb)
    if(symb == 'var'):
        symb_value = symb_from_var(symb_value)
        if(symb_value == None):
            exit_none_var()
        symb = get_type_val(symb_value)
    if(symb == 'int'):
        data.append(int(symb_value))
    elif(symb == 'bool'):
        if(symb_value == 'true'):
            data.append(True)
        else:
            data.append(False)
    elif(symb == 'nil'):
        data.append(None)
    else:
        data.append(symb_value)


def r_Pops(var, var_value):
    global data
    if(len(data) == 0):
        exit_non_frame
    value = data.pop()
    frame, var_value = var_values(var_value)
    save_to_var(frame, var_value, value, get_type_val(value))

def r_Add(var, type1, value1, type2, value2):
    type1 = get_type(type1)
    type2 = get_type(type2)
    if(type1== 'var'):
        value1 = symb_from_var(value1)
        if(value1 == None):
            exit_none_var
        type1 = get_type_val(value1)
    if (type2 == 'var'):
        value2 = symb_from_var(value2)
        if(value2 == None):
            exit_none_var()
        type2 = get_type_val(value2)
    if (type1 != 'int' and type2 != 'int'):
        exit_operands()
    result = int(value1) + int(value2)
    frame, var_value = var_values(var)
    save_to_var(frame, var_value, result, 'int')

def r_Sub(var, type1, value1, type2, value2):
    type1 = get_type(type1)
    type2 = get_type(type2)
    if(type1== 'var'):
        value1 = symb_from_var(value1)
        if(value1 == None):
            exit_none_var
        type1 = get_type_val(value1)
    if (type2 == 'var'):
        value2 = symb_from_var(value2)
        if(value2 == None):
            exit_none_var()
        type2 = get_type_val(value2)
    if (type1 != 'int' and type2 != 'int'):
        exit_operands()
    result = int(value1) - int(value2)
    frame, var_value = var_values(var)
    save_to_var(frame, var_value, result, 'int')    

def r_Mul(var, type1, value1, type2, value2):
    type1 = get_type(type1)
    type2 = get_type(type2)
    if(type1== 'var'):
        value1 = symb_from_var(value1)
        if(value1 == None):
            exit_none_var
        type1 = get_type_val(value1)
    if (type2 == 'var'):
        value2 = symb_from_var(value2)
        if(value2 == None):
            exit_none_var()
        type2 = get_type_val(value2)    
    if (type1 != 'int' and type2 != 'int'):
        exit_operands()
    result = int(value1) * int(value2)
    frame, var_value = var_values(var)
    save_to_var(frame, var_value, result, 'int')   

def r_Idiv(var, type1, value1, type2, value2):
    type1 = get_type(type1)
    type2 = get_type(type2)
    if(type1== 'var'):
        value1 = symb_from_var(value1)
        if(value1 == None):
            exit_none_var
        type1 = get_type_val(value1)
    if (type2 == 'var'):
        value2 = symb_from_var(value2)
        if(value2 == None):
            exit_none_var()
        type2 = get_type_val(value2)
    if (type1 != 'int' and type2 != 'int'):
        exit_operands()
    if (value2 == "0"):
        exit_bad_operand()
    result = int(value1) / int(value2)
    frame, var_value = var_values(var)
    save_to_var(frame, var_value, int(result), 'int')   

def r_Lt(var, type1, value1, type2, value2):
    type1 = get_type(type1)
    type2 = get_type(type2)
    if(type1== 'var'):
        value1 = symb_from_var(value1)
        if(value1 == None):
            exit_none_var
        type1 = get_type_val(value1)
    if (type2 == 'var'):
        value2 = symb_from_var(value2)
        if(value2 == None):
            exit_none_var()
        type2 = get_type_val(value2)
    if(type1== 'int'):
        if(type2 == 'int'):
            if(int(value1) < int(value2)):
                result = True
            else:
                result = False
        else:
            exit_operands()
    elif(type1 == 'bool'):
        if(type2 == 'bool'):
            if(value1 == 'false'):
                result = True
            else:
                result = False
        else:
            exit_bad_operand()
    elif(type1 == 'string'):
        if(type2 ==  'string'):
            if(value1 < value2):
                result = True
            else:
                result = False
        else:
            exit_bad_operand()
    else:
        exit_bad_operand()
    frame, var_value = var_values(var)
    save_to_var(frame, var_value, result, get_type_val(result))

def r_Gt(var, type1, value1, type2, value2):
    type1 = get_type(type1)
    type2 = get_type(type2)
    if(type1== 'var'):
        value1 = symb_from_var(value1)
        if(value1 == None):
            exit_none_var
        type1 = get_type_val(value1)
    if (type2 == 'var'):
        value2 = symb_from_var(value2)
        if(value2 == None):
            exit_none_var()
        type2 = get_type_val(value2)
    if(type1 == 'int'):
        if(type2 == 'int'):
            if(int(value1) > int(value2)):
                result = True
            else:
                result = False
        else:
            exit_operands()
    elif(type1 == 'bool'):
        if(type2 == 'bool'):
            if(value1 == 'true'):
                result = True
            else:
                result = False
        else:
            exit_bad_operand()
    elif(type1 == 'string'):
        if(type2 ==  'string'):
            if(value1 > value2):
                result = True
            else:
                result = False
        else:
            exit_bad_operand()
    else:
        exit_bad_operand()
    frame, var_value = var_values(var)
    save_to_var(frame, var_value, result, get_type_val(result))


def r_Eq(var, type1, value1, type2, value2):
    type1 = get_type(type1)
    type2= get_type(type2)
    if(type1== 'var'):
        value1 = symb_from_var(value1)
        if(value1 == None):
            exit_none_var
        type1 = get_type_val(value1)
    if (type2 == 'var'):
        value2 = symb_from_var(value2)
        if(value2 == None):
            exit_none_var()
        type2 = get_type_val(value2)
    if(type1 == 'int'):
        if(type2 == 'int'):
            if(int(value1) == int(value2)):
                result = True
            else:
                result = False
        else:
            exit_operands()
    elif(type1 == 'bool'):
        if(type2 == 'bool'):
            value1, value2 = convert_to_bool(value1, value2)
            if(value1 == value2):
                result = True
            else:
                result = False
        else:
            exit_bad_operand()
    elif(type1 == 'string'):
        if(type2 ==  'string'):
            if(value1 == value2):
                result = True
            else:
                result = False
        else:
            exit_bad_operand()
    else:
        if(type2 == 'nil'):
            if(value1 == value2):
                result = True
            else:
                result = False
        else:
            exit_bad_operand()
    frame, var_value = var_values(var)
    save_to_var(frame, var_value, result, get_type_val(result))

def r_And(var, type1, value1, type2, value2):
    type1 = get_type(type1)
    type2= get_type(type2)
    if(type1== 'var'):
        value1 = symb_from_var(value1)
        if(value1 == None):
            exit_none_var
        type1 = get_type_val(value1)
    if (type2 == 'var'):
        value2 = symb_from_var(value2)
        if(value2 == None):
            exit_none_var()
        type2 = get_type_val(value2)
    if(type1 not in ['bool', 'var'] or type2 not in ['bool', 'var']): 
        exit_bad_operand()
    value1, value2 = convert_to_bool(value1, value2)  
    result = value1 and value2
    frame, var_value = var_values(var)
    save_to_var(frame, var_value, result, get_type_val(result))

def r_Or(var, type1, value1, type2, value2):
    type1 = get_type(type1)
    type2= get_type(type2)
    if(type1== 'var'):
        value1 = symb_from_var(value1)
        if(value1 == None):
            exit_none_var
        type1 = get_type_val(value1)
    if (type2 == 'var'):
        value2 = symb_from_var(value2)
        if(value2 == None):
            exit_none_var()
        type2 = get_type_val(value2)    
    if(type1 not in ['bool', 'var'] or type2 not in ['bool', 'var']): 
        exit_bad_operand()
    value1, value2 = convert_to_bool(value1, value2)
    result = value1 or value2
    frame, var_value = var_values(var)
    save_to_var(frame, var_value, result, get_type_val(result))    

def r_Not(var, type1, value1):
    type1 = get_type(type1)
    if(type1== 'var'):
        value1 = symb_from_var(value1)
        if(value1 == None):
            exit_none_var
    if(type1 not in ['bool', 'var']): 
        exit_bad_operand()
    if(type(value1) is not bool):
        exit_bad_operand()
    value1, value2 = convert_to_bool(value1, None)
    result = not value1
    frame, var_value = var_values(var)
    save_to_var(frame, var_value, result, get_type_val(result))

def r_Int2char(var, type1, value1):
    type1 = get_type(type1)
    if(type1== 'var'):
        value1 = symb_from_var(value1)
        if(value1 == None):
            exit_none_var
    if(type1 not in ['int', 'var']):
        exit_bad_operand()
    try:
        result = chr(value1)
    except:
        exit_string()
    frame, var_value = var_values(var)
    save_to_var(frame, var_value, result, get_type_val(result))

def r_Stri2int(var, type1, value1, type2, value2):
    type1 = get_type(type1)
    type2= get_type(type2)
    if(type1== 'var'):
        value1 = symb_from_var(value1)
        if(value1 == None):
            exit_none_var
        type1 = get_type_val(value1)
    if (type2 == 'var'):
        value2 = symb_from_var(value2)
        if(value2 == None):
            exit_none_var()
        type2 = get_type_val(value2)
    if(type1 not in ['string', 'var'] or type2 != 'int'):
        exit_bad_operand()
    if 0<= int(value2) <len(value1):
        char_ord = [ord(i) for i in value1]
        result = char_ord[int(value2)]
    else:
        exit_string()        
    frame, var_value = var_values(var)
    save_to_var(frame, var_value, result, get_type_val(result))

def r_Read(var, type1, type_val):
    if not arg_input:
        inputi = input()
    else:
        inputi = input_f.readline().split("\n")[0]
    if(type_val == 'int'):
        try:
            result = int(inputi)
        except:
            result = None
    elif(type_val == 'string'):
        try:
            result = str(inputi)
        except:
            result = None
    elif(type_val == 'bool'):
        if(inputi.upper()!= "TRUE"):
            result = False
        else:
            result = True    
    else:
        exit_operands()
    frame, var_value = var_values(var)
    save_to_var(frame, var_value, result, get_type_val(result))

def r_Write(type1, type_value):
    type1 = get_type(type1)
    if(type1== 'var'):
        type_value = symb_from_var(type_value)
        if(type_value == None):
            exit_none_var
        print(type_value, end="")
    elif(type1 == 'bool'):
        if(type_value == 'true'):
            print("true", end="")
        else:
            print("false", end="")
    elif(type1 == "int"):
        print(int(type_value), end="")
    elif(type1 == "string"):
        type_value = interpret_escape(type_value)
        print(type_value, end="")
    elif(type1 == "nil"):
        print("", end="")
    else:
        exit_bad_operand()


def run_instructions(xml):
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
            sys.exit(0)
        elif (opcode == "PUSHS"):
            r_Pushs(arg1, arg1_text)
        elif (opcode == "POPS"):
            r_Pops(arg1,arg1_text)
        elif (opcode == "ADD"):
            r_Add(arg1_text, arg2, arg2_text, arg3,  arg3_text)
        elif (opcode == "SUB"):
            r_Sub(arg1_text, arg2, arg2_text, arg3, arg3_text)
        elif (opcode == "MUL"):
            r_Mul(arg1_text, arg2, arg2_text, arg3, arg3_text)
        elif (opcode == "IDIV"):
            r_Idiv(arg1_text, arg2, arg2_text, arg3, arg3_text)
        elif (opcode == "LT"):
            r_Lt(arg1_text, arg2, arg2_text, arg3, arg3_text)
        elif (opcode == "GT"):
            r_Gt(arg1_text, arg2, arg2_text, arg3, arg3_text)
        elif (opcode == "EQ"):
            r_Eq(arg1_text, arg2, arg2_text, arg3, arg3_text)
        elif (opcode == "AND"):
            r_And(arg1_text, arg2, arg2_text, arg3, arg3_text)
        elif (opcode == "OR"):
            r_Or(arg1_text, arg2, arg2_text, arg3, arg3_text)
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
        """elif (opcode == "CONCAT"):

        elif (opcode == "STRLEN"):

        elif (opcode == "GETCHAR"):

        elif (opcode == "SETCHAR"):

        elif (opcode == "TYPE"):

        elif (opcode == "JUMP"):

        elif (opcode == "JUMPIFEQ"):

        elif (opcode == "JUMPIFNEQ"):


        elif (opcode == "DPRINT"):

        elif (opcode == "BREAK"):"""

    
        position+=1
        

arg_input = arg_source = False
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
        else:
            sys.stderr.write('Invalid argument\n')
            sys.exit(10)

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
#print(ET.tostring(xml, encoding='utf8').decode('utf8'))
xml = check_xml_root(xml)
run_instructions(xml)