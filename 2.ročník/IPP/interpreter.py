import sys
import argparse
import re
import xml.etree.ElementTree as ET

pattern = re.compile(r'\s+')

zero_ins = ["CREATEFRAME", "PUSHFRAME", "POPFRAME", "RETURN", "BREAK"]
one_ins = ["DEFVAR", "CALL", "PUSHS", "POPS", "WRITE", "LABEL", "JUMP", "EXIT", "DPRINT"]
two_ins = ["MOVE", "INT2CHAR", "READ", "STRLEN", "TYPE", "NOT"]
three_ins = ["ADD", "SUB", "MUL", "IDIV", "LT", "GT", "EQ", "AND", "OR", "STRI2INT",
    "CONCAT", "GETCHAR", "SETCHAR", "JUMPIFEQ", "JUMPIFNEQ"]

def exit_xml():
    sys.stderr.write('xml is invalid\n')
    sys.exit(32)     

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
    check_xml_ins(xml)

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

def var_check(type, text):
    #TODO
    True

def sym_check(type, text):
    #TODO
    True

def label_check(type, text):
    #TODO
    True

def zero_arg_check(ins):
    if(len(ins) != 0):
        exit_xml()

def one_arg_check(ins):
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
            var_check(arg.attrib['type'], arg.text)
        else:
            exit_xml()
    elif ins_name in ["PUSHS","WRITE","EXIT","DPRINT"]:
        if(arg.attrib['type'] in ["int", "bool", "string", "var", "nil"]):
            sym_check(arg.attrib['type'], arg.text)
        else:
            exit_xml()
    elif ins_name in ["CALL","LABEL","JUMP"]:
        if(arg.attrib['type'] == 'label'):
            label_check(arg.attrib["type"], arg.text)
        else:
            exit_xml()

def two_arg_check(ins):
    print(len(ins))
    for arg in ins:
        True
       # print(arg)
       # print(len(arg.attrib))
def three_arg_check(ins):
    for arg in ins:
      #  print(arg)
      True


def execute_ins(xml):
    for ins in xml:
        ins_name = ins.attrib['opcode'].upper()
        args = get_args(ins_name)
        if(args == 0):
            zero_arg_check(ins)
        if(args == 1):
            one_arg_check(ins)
        if(args == 2):
            two_arg_check(ins)
        if (args == 3):
            three_arg_check(ins)

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
check_xml_root(xml)

