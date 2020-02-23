<?php
/* IPP parser for IPPcode20
 * author: Tomáš Ďuriš (xduris05)
 * 2020
 */

// Rozdelenie príkazov podla počtu argumentov
//žiadny argument
$zero_arg_ins = array
(
    "CREATEFRAME", "PUSHFRAME", "POPFRAME", "RETURN", "BREAK"
);
//1 argument
$one_arg_ins = array
(
    "DEFVAR", "CALL", "PUSHS", "POPS", "WRITE", "LABEL", "JUMP", "EXIT", "DPRINT"
);
//2 argumenty
$two_arg_ins = array
(
    "MOVE", "INT2CHAR", "READ", "STRLEN", "TYPE", "NOT"
);
//3 argumenty
$three_arg_ins = array
(
    "ADD", "SUB", "MUL", "IDIV", "LT", "GT", "EQ", "AND", "OR", "STRI2INT",
    "CONCAT", "GETCHAR", "SETCHAR", "JUMPIFEQ", "JUMPIFNEQ"
);
$count = 0;

//Vytvorenie hlavičky XML súboru
function xml_init()
{
    global $xml_file;
    $xml_file->openMemory();
    $xml_file->setIndent(true);
    $xml_file->setIndentString("    ");
    $xml_file->startDocument("1.0", "UTF-8");
    $xml_file->startElement("program");
    $xml_file->startAttribute("language");
    $xml_file->text("IPPcode20");
    $xml_file->endAttribute();
}
//funkcia na vrátenie očakávaného počtu parametrov
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

//funkcia na overenie lexikálnej správnosti premennej
function check_var($var)
{
    if (!preg_match("~^(LF|TF|GF)@[a-zA-Z_\-$&%*!?][a-zA-Z0-9_\-$&%*?!]*$~", $var))
    {
        fwrite(STDERR, "Invalid variable\n");
        exit(23);
    }
}

//Funkcia na výpis premennej do XML
function write_var($number, $arg)
{
    global $xml_file;
    $xml_file->startElement($number);
    $xml_file->writeAttribute("type", "var");
    $xml_file->text($arg);
    $xml_file->endElement();
}

//Funkcia na overenie lexikálnej správnosti konštánt
function check_const($const)
{
    //Ak je to premenná vrátime 0
    if(preg_match("~^(LF|TF|GF)@[a-zA-Z_\-$&%*!?][a-zA-Z0-9_\-$&%*!?]*$~", $const))
    {
        return 0;
    }
    //inak skontrolujeme či zápis konštanty je správny
    if(!preg_match("/^int@[-+]?[0-9]+$|^bool@true$|^bool@false$|^string@.*|^nil@nil$/", $const))
    {
        fwrite(STDERR, "Invalid constant\n");
        exit(23);
    }
    //ak sa jedná o reťazec tak skontrolujeme správnosť "escape" sekvencií a vrátime 1
    if(preg_match("/^string@.+/", $const))
    {
        $backslash = preg_match_all("/\\\\/", $const);
        if($backslash > 0)
        {
            $escape = preg_match_all("/\\\\[0-9]{3}/", $const);
            if($escape != $backslash)
            {
                fwrite(STDERR, "Invalid escape\n");
                exit(23);
            }
        }
    }
    return 1;
}

//funkcia na zápis konštanty do XML dokumentu
function write_const($number, $type, $arg)
{
    global $xml_file;
    $xml_file->startElement($number);
    $xml_file->writeAttribute("type", $type);
    $xml_file->text($arg);
    $xml_file->endElement();
}

//funkcia na overenie lexikálnej správnosti návestia
function check_label($label)
{
    if (!preg_match("~^[a-zA-Z_\-$&%*!?][a-zA-Z0-9_\-$&%*!?]*$~", $label))
    {
        fwrite(STDERR, "Invalid label\n");
        exit(23);
    }
}

//funkcia na zápis návestia do XML
function write_label($number, $arg)
{
    global $xml_file;
    $xml_file->startElement($number);
    $xml_file->writeAttribute("type", "label");
    $xml_file->text($arg);
    $xml_file->endElement();
}

//funkcia na overenie lexikálnej správnosti typu
function check_type($type)
{
    if(!preg_match("/^int$|^bool$|^string$/", $type))
    {
        fwrite(STDERR, "Invalid type\n");
        exit(23);
    }
}

//funkcia na zápis typu do XML
function write_type($number, $arg)
{
    global $xml_file;
    $xml_file->startElement($number);
    $xml_file->writeAttribute("type", "type");
    $xml_file->text($arg);
    $xml_file->endElement();
}

//funkcia na overenie či súhlasí zadaný počet parametrov a očakávaný počet
function check_params($param, $word)
{
    global $labels, $jumps;
    $word[0] = strtoupper($word[0]);
    switch ($param) {
        case 0:
            {
                if($word[0] == "RETURN")
                {
                    $jumps++;
                }
                break;
            }
        case 1:
            {
                if($word[0] == "DEFVAR" || $word[0] == "POPS")
                {
                    check_var($word[1]);
                    write_var("arg1", $word[1]);
                }
                else if ($word[0] == "JUMP" || $word[0] == "CALL")
                {
                    check_label($word[1]);
                    write_label("arg1", $word[1]);
                    $jumps++;
                }
                else if($word[0] == "LABEL")
                {
                    check_label($word[1]);
                    write_label("arg1", $word[1]);
                    $labels++;
                }
                else
                {
                    $const = check_const($word[1]);
                    if($const == 0)
                    {
                        write_var("arg1", $word[1]);
                    }
                    else if ($const == 1)
                    {
                        $var = explode("@", $word[1], 2);
                        write_const("arg1", $var[0], $var[1]);
                    }
                }
                break;
            }
        case 2:
            {
                if ($word[0] == "READ")
                {
                    check_var($word[1]);
                    write_var("arg1", $word[1]);
                    check_type($word[2]);
                    write_type("arg2", $word[2]);
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
                    check_label($word[1]);
                    write_label("arg1", $word[1]);
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
                    $const = check_const($word[3]);
                    if($const == 0)
                    {
                        write_var("arg3", $word[3]);
                    }
                    else if ($const == 1)
                    {
                        $var = explode("@", $word[3], 2);
                        write_const("arg3", $var[0], $var[1]);
                    }
                    $jumps++;
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
                    $const = check_const($word[3]);
                    if($const == 0)
                    {
                        write_var("arg3", $word[3]);
                    }
                    else if ($const == 1)
                    {
                        $var = explode("@", $word[3], 2);
                        write_const("arg3", $var[0], $var[1]);
                    }
                }
                break;
            }
    }
}

function print_stats()
{
    global $dest, $jumpsArg, $commentsArg, $labelsArg, $locArg, $argv, $loc, $comments, $labels, $jumps;
    $file = fopen($dest, 'w');
    if(!$file)
    {
        fwrite(STDERR, "ERROR, too many arguments\n");
        exit(12);
    }
    if ($jumpsArg == false && $commentsArg == false && $locArg == false && $labelsArg == false)
    {
        fclose($file);
        return;
    }
    foreach ($argv as $argument)
    {
        if ($argument == "--loc")
        {
            fwrite($file, $loc);
            fwrite($file, "\n");
        }
        if($argument == "--comments")
        {
            fwrite($file, $comments);
            fwrite($file, "\n");
        }
        if($argument == "--labels")
        {
            fwrite($file, $labels);
            fwrite($file, "\n");
        }
        if($argument == "--jumps")
        {
            fwrite($file, $jumps);
            fwrite($file, "\n");
        }
    }
    fclose($file);

}

//parsovanie argumentov z príkazoveho riadku
$options = array("stats", "loc", "comments", "labels", "jumps", "help");
$args = getopt("",$options);
$statsArg = false;
$locArg = false;
$commentsArg = false;
$jumpsArg = false;
$labelsArg = false;

if ($argc > 1)
{
    if(!empty($args))
    {
        if(array_key_exists("help", $args))
        {
            if($argc == 2)
            {
                echo "parser.php\n";
                echo "Napoveda:\n";
                echo "Autor: Tomas Duris (xduris05)\n\n";
                echo "Skript parser.php nacita zo standardného vstupu zdrojovy kod v jazyku IPPcode20\n";
                echo "nasledne prebehne jeho lexikalna a syntakticka kontrola.\n";
                echo "Vystupom je XML reprezentacia programu\n";
                echo "--------------------------------------------------------------------------------------------------------\n";
                echo "Pouzitie:\n";
                echo "--help    pre vypisanie napovedy\n";
                echo "--loc     pre vypisane statistik s poctom riadkov s instrukciami\n";
                echo "--comments pre vypisanie poctu riadkov s komentami";
                echo "--jumps   pre vypisanie poctu instrukcii pre podmienene/nepodmienene skoky, volania a navraty z funkcii\n";
                echo "--stats=file pre zadanie suboru kam sa statistiky vypisu (ak je iba parameter --stats vystupom je prazdny subor)\n";
                exit(0);
            }
            else
            {
                fwrite(STDERR, "ERROR, bad help argument\n");
                exit(10);
            }
        }
        if (array_key_exists("stats", $args))
        {
            $dest = preg_grep("/^--stats=.+$/", $argv);
            if(count($dest)!= 1)
            {
                fwrite(STDERR, "ERROR, too many --stats\n");
                exit(10);
            }
            $dest = array_pop($dest);
            $dest = explode("=", $dest);
            $dest = $dest[1];
            $statsArg = true;
        }
        if (array_key_exists("loc", $args))
        {
            if ($statsArg == false)
            {
                fwrite(STDERR, "ERROR, you need --stats=file argument\n");
                exit(10);
            }
            $locArg = true;
        }
        if (array_key_exists("comments", $args))
        {
            if ($statsArg == false)
            {
                fwrite(STDERR, "ERROR, you need --stats=file argument\n");
                exit(10);
            }
            $commentsArg = true;
        }
        if (array_key_exists("labels", $args))
        {
            if ($statsArg == false)
            {
                fwrite(STDERR, "ERROR, you need --stats=file argument\n");
                exit(10);
            }
            $labelsArg = true;
        }
        if(array_key_exists("jumps", $args))
        {
            if ($statsArg == false)
            {
                fwrite(STDERR, "ERROR, you need --stats=file argument\n");
                exit(10);
            }
            $jumpsArg = true;
        }
    }
    else
    {
        fwrite(STDERR, "ERROR, bad argument\n");
        exit(10);
    }
}
//nacitanie vstupu zo stdin
if(!$file = fopen('php://stdin', 'r'))
{
    fwrite(STDERR, "Wrong file\n");
    exit(11);
}
$file = stream_get_contents($file);
if (!($file))
{
    fwrite(STDERR, "Missing header\n");
    exit(21);
}
//Rozdelenie na riadky na zaklade delimiteru \n
$file = explode("\n", $file);
//odstranenie prazdnych riadkov
$file = array_filter($file);

$first_line = true;
//inicializacia XML (použitie XMLWriter kvôli safe charakterom ako <,>
$xml_file = new XMLWriter;
xml_init();
$order = 1;
$loc = 0;
$comments = 0;
$labels = 0;
$jumps = 0;
$have_header = false;
//prechadzanie po riadkoch
foreach ($file as &$line)
{
    //odstranenie komentarov samostatne na riadku alebo ak su pred nimi biele znaky
    if (preg_match("~^\s*#~", $line))
    {
        $comments++;
        continue;
    }
    elseif (preg_match("~^\s*$~", $line)) continue;

    //odstranenie komentarov na riadku s inštrukciami
    if (strpos($line, "#"))
    {
        $comments++;
        $line = substr($line,0, strpos($line, "#"));
    }
    //nahradenie viacerých bielych znakov za medzeru
    $line = trim(preg_replace("/\s+/", " ", $line));
    //skontrolovanie hlavicky
    if($first_line)
    {
        if(strtolower($line) != ".ippcode20")
        {
            fwrite(STDERR, "Invalid header\n");
            exit(21);
        }
        $have_header = true;
        $first_line = false;
        continue;
    }
    //rozdelenie riadkov na slova
    $word = explode(" ", $line);
    $inst_params = check_first_param($word[0], $zero_arg_ins, $one_arg_ins, $two_arg_ins, $three_arg_ins);
    $line_params = count($word) - 1;
    //pre neocakavanom pocte parametrov hlasime chybu
    if ($inst_params != $line_params)
    {
        fwrite(STDERR, "Invalid number of params\n");
        exit(23);
    }
    //generovanie instrukcii
    else
    {
        $xml_file->startElement('instruction');
        $xml_file->writeAttribute('order', $order);
        $xml_file->writeAttribute('opcode', strtoupper($word[0]));
    }
    check_params($inst_params, $word);
    $xml_file->endElement();
    $order++;
    $loc++;
}
if($have_header == false)
{
    fwrite(STDERR, "Invalid header\n");
    exit(21);
}
//ukoncenie a vypis XML na vystup
$xml_file->endDocument();
echo $xml_file->outputMemory(true);
//Rozsirenie pre vypis statistik
if ($statsArg == true)
{
    print_stats();
}
?>
