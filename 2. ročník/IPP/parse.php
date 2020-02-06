<?php
/* IPP parser for IPPcode20
 * author: Tomáš Ďuriš (xduris05)
 * 2020
 */

$zero_arg_ins = array
(
    "CREATEFRAME", "PUSHFRAME", "POPFRAME", "RETURN", "BREAK"
);

$one_arg_ins = array
(
    "DEFVAR", "CALL", "PUSHS", "POPS", "WRITE", "LABEL", "JUMP", "EXIT", "DPRINT"
);

$two_arg_ins = array
(
    "MOVE", "INT2CHAR", "READ", "STRLEN", "TYPE", "NOT"
);

$three_arg_ins = array
(
    "ADD", "SUB", "MUL", "IDIV", "LT", "GT", "EQ", "AND", "OR", "STRI2INT",
    "CONCAT", "GETCHAR", "SETCHAR", "JUMPIFEQ", "JUMPIFNEQ"
);
$count = 0;

function xml_init()
{
    global $xmldata;
    $xmldata->openMemory();
    $xmldata->setIndent(true);
    $xmldata->setIndentString("    ");
    $xmldata->startDocument("1.0", "UTF-8");
    $xmldata->startElement("program");
    $xmldata->startAttribute("language");
    $xmldata->text("IPPcode20");
    $xmldata->endAttribute();
}

function check_first_param($param, $zero_arg, $one_arg, $two_arg, $three_arg)
{
    $param = strtoupper($param);
    if(in_array($param, $zero_arg))
        return 0;
    if(in_array($param, $one_arg))
        return 1;
    if(in_array($param, $two_arg))
        return 2;
    if(in_array($param, $three_arg))
        return 3;
    else
    {
        fwrite(STDERR, "Invalid operation\n");
        exit(22);
    }
}

function check_var($var)
{
    if (!preg_match("~^(LF|TF|GF)@[a-zA-Z_\-$&%*][a-zA-Z0-9_\-$&%*]*$~", $var))
    {
        fwrite(STDERR, "Invalid variable\n");
        exit(22);
    }
}
function write_var($number, $arg)
{
    global $xmldata;
    $xmldata->startElement($number);
    $xmldata->writeAttribute("type", "var");
    $xmldata->text($arg);
    $xmldata->endElement();
}

function check_const($const)
{
    if(preg_match("~^(LF|TF|GF)@[a-zA-Z_\-$&%*][a-zA-Z0-9_\-$&%*]*$~", $const))
    {
        return 0;
    }

    if(!preg_match("/^int@[-+]?[0-9]+$|^bool@true$|^bool@false$|^string@.*|^nil@nil$/", $const))
    {
        fwrite(STDERR, "Invalid constant\n");
        exit(22);
    }
    if(preg_match("/^string@.*/", $const))
    {
        $backslash = preg_match_all("/\\\/", $const);
        if($backslash > 0)
        {
            $escape = preg_match_all("/\\\[0-9]{3}/", $const);
            if($escape != $backslash)
            {
                fwrite(STDERR, "Invalid escape\n");
                exit(23);
            }
        }
    }
    return 1;
}

function write_const($number, $type, $arg)
{
    global $xmldata;
    $xmldata->startElement($number);
    $xmldata->writeAttribute("type", $type);
    $xmldata->text($arg);
    $xmldata->endElement();
}

function check_params($param, $word)
{
    switch ($param) {
        case 0:
            {
                break;
            }
        case 1:
            {
                if($word[0] == "DEFVAR" || $word[0] == "POPS")
                {

                }
                else if ($word[0] == "JUMP" || $word[0] == "CALL")
                {

                }
                else if($word[0] == "LABEL")
                {

                }
                else
                {

                }
                break;
            }
        case 2:
            {
                if ($word[0] == "READ")
                {

                }
                else
                {
                    check_var($word[1]);
                    write_var("arg1", $word[1]);
                    $const = check_const($word[2]);
                    if($const == 0)
                    {
                        write_var("arg2", $word[2]);
                    }
                    else if ($const == 1)
                    {
                        $var = explode("@", $word[2], 2);
                        write_const("arg2", $var[0], $var[1]);
                    }
                }
                break;
            }
        case 3:
            {
                if ($word[0] == "JUMPIFEQ" || $word[0] == "JUMPIFNEQ")
                {

                }
                break;
            }
    }
}

$options = array("stats:", "loc", "comments", "labels", "jumps", "help");
$args = getopt("",$options);
$options[0] = "stats";
if($argc > 5)
{
    fwrite(STDERR, "ERROR, too many arguments\n");
    exit(21);
}
elseif ($argc <= 5 && $argc > 1)
{
    if(!empty($args))
    {
        if(array_key_exists("help", $args))
        {
            if($argc == 2)
            {
                echo "HELP\n";
                exit(0);
            }
            else
            {
                fwrite(STDERR, "ERROR, bad help argument\n");
                exit(10);
            }
        }
    }
}

if(!$file = fopen('php://stdin', 'r'))
{
    fwrite(STDERR, "Wrong file\n");
    exit(21);
}
$file = stream_get_contents($file);
//Rozdelenie na riadky na základe delimiteru \n
$file = explode("\n", $file);
//odstranenie prázdnych riadkov
$file = array_filter($file);

$first_line = true;
//inicializácia XML
$xmldata = new XMLWriter;
xml_init();
$order = 1;

foreach ($file as &$line)
{
    if (preg_match("#^[\s]+$#", $line))
        continue;
    $line = trim(preg_replace("/ +/", " ", $line));

    if ($line[0] == "#")
    {
        continue;
    }

    if($first_line)
    {
        if(strtolower($line) != ".ippcode20")
        {
            fwrite(STDERR, "Invalid header\n");
            exit(21);
        }
        $first_line = false;
        continue;
    }
    $word = explode(" ", $line);
    $inst_params = check_first_param($word[0], $zero_arg_ins, $one_arg_ins, $two_arg_ins, $three_arg_ins);
    $line_params = count($word) - 1;
    if ($inst_params != $line_params)
    {
        fwrite(STDERR, "Invalid number of params\n");
        echo $count;
        exit(23);
    }
    else
    {
        $xmldata->startElement('instruction');
        $xmldata->writeAttribute('order', $order);
        $xmldata->writeAttribute('opcode', $word[0]);
    }
    check_params($inst_params, $word);
    //$count++;
    $xmldata->endElement();
    $order++;
}
echo $xmldata->outputMemory(true);
?>