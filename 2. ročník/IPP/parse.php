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
        exit(21);
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

    //$count++;
    $xmldata->endElement();
    $order++;
}
echo $xmldata->outputMemory(true);
?>