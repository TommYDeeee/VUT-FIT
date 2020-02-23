<?php
/* IPP parser for IPPcode20
 * author: Tomáš Ďuriš (xduris05)
 * 2020
 */
$all_tests = array();

function check_arg_validity()
{
    global $int_only_arg, $parse_only_arg, $int_script_arg, $parse_script_arg;
    if ($int_only_arg)
    {
        if($parse_only_arg || $parse_script_arg)
        {
            fwrite(STDERR, "ERROR, combination of arguments\n");
            exit(10);
        }
    }
    elseif ($parse_only_arg)
    {
        if($int_script_arg || $int_script_arg)
        {
            fwrite(STDERR, "ERROR, combination of arguments\n");
            exit(10);
        }
    }
}

function find_files($path)
{
    global $recursive_arg, $all_tests;
    $dir_files = scandir($path);

    foreach ($dir_files as $file)
    {
        if ($file === "." || $file === "..")
        {
            continue;
        }
        $file = $path . $file;
        if(is_dir($file))
        {
            if ($recursive_arg)
            {
                find_files($file . "/");
            }
        }
        elseif (is_file($file))
        {
            array_push($all_tests, $file);
        }
    }
}

function create_missing_files()
{
    global $all_tests;
    foreach($all_tests as $test)
    {
        $name = pathinfo($test, PATHINFO_FILENAME);
        $dir = pathinfo($test, PATHINFO_DIRNAME);
        if(pathinfo($test, PATHINFO_EXTENSION) == "src")
        {
            if (!file_exists($dir . "/" . $name . ".in")) {
                #echo "$dir/$name.in";
                exec("touch " . $dir . "/" . $name . ".in");
            }
            if (!file_exists($dir . "/" . $name . ".out")) {
                exec("touch " . $dir . "/" . $name . ".out");
            }
            if (!file_exists($dir . "/" . $name . ".rc")) {
                exec("touch " . $dir . "/" . $name . ".rc");
                $rc_file = fopen($dir . "/" . $name . ".rc", "w");
                fwrite($rc_file, "0");
                fclose($rc_file);
            }
        }
    }
}

function run_tests()
{
    global $all_tests, $parse_only_arg, $int_only_arg, $file_p, $file_i, $file_j;
    $count = 0;
    foreach ($all_tests as $test)
    {
        $name = pathinfo($test, PATHINFO_DIRNAME);
        $name = $name . "/" . pathinfo($test, PATHINFO_FILENAME);
        //echo "$name\n";
        exec("touch ./tmp_parse");
        exec("touch ./tmp_diff");
        if (pathinfo($test, PATHINFO_EXTENSION) == "rc")
        {
            $rc = fgets(fopen($test, "r"));
        }
        if($parse_only_arg)
        {
            if (pathinfo($test, PATHINFO_EXTENSION) == "src")
            {
                echo
                "    
                <tr>
                    <td>".$count."</td>
                    <td>".$test."</td>
                    <td ";
                exec("php \"" . $file_p . "\" <\"" . $name . ".src\" > ./tmp_parse 2>/dev/null", $output, $return_parse);
                if ($return_parse == $rc)
                {
                    if($rc == "0")
                    {
                        $command = "java -jar ".$file_j." tmp_parse ".$name.".out tmp_diff /D options";
                        #echo "$command\n";
                        exec($command,$output, $return_parse);
                        if($return_parse == "0")
                        {
                            echo "style=\"color:green;background-color:#30EB99;font-weight:bold\">SUCCEEDED";
                        }
                        else
                        {
                            echo "style=\"color:red;background-color:#F78481;font-weight:bold\">FAILED";
                        }
                    }
                    else
                    {
                        echo "style=\"color:green;background-color:#30EB99;font-weight:bold\">SUCCEEDED";
                    }
                }
                else
                {
                    echo "style=\"color:red\">FAILED";
                }
                echo
                "</td>
                </tr>
                ";
                $count++;
            }
        }
    }
}

function html_header()
{
    echo
"<!DOCTYPE html>
<html>
    <head>
        <meta charset='UTF-8'>
        <title>TEST RESULTS</title>
    </head>
        <style>
            h1
            {
                text-align: center;
                padding-bottom: 5%; 
                font-size: 400%;
                color: blue; 
            }
            tr
            {
                border: 2px solid black;
            }
            table
            {
            border-collapse: collapse;
            }
        </style>
    <body>
    <h1>TEST SUMMARY</h1>
    <table style='width: 100%'>
        <tr>
            <th>Number</th>
            <th>Name</th>
            <th>Result</th>
        </tr>";
}

$options = array("help", "directory", "recursive", "parse-script", "int-script", "parse-only", "int-only", "jexamxml");
$args = getopt("", $options);
$directory_arg = $recursive_arg = $parse_script_arg = $int_script_arg = $parse_only_arg = $int_only_arg = $jexamxml_arg = false;
$path = getcwd();
$path = $path . '/';
$file_p = realpath("./parse.php");
$file_i = realpath("./interpretery.py");
$file_j = realpath("/pub/courses/ipp/jexamxml.jar");

if($argc > 1)
{
    if(!empty($args)) {
        if (array_key_exists("help", $args)) {
            if ($argc == 2) {
                echo "parser.php\n";
                echo "Napoveda:\n";
                echo "Autor: Tomas Duris (xduris05)\n\n";
                echo "Skript test.php skontroluje správnosť implementácie skriptu parser.php a interpreter.py na zvolenej testovacej sade\n";
                echo "Vystupom je HTML5 reprezentacia výsledkov\n";
                echo "--------------------------------------------------------------------------------------------------------\n";
                echo "Pouzitie:\n";
                echo "--help    pre vypisanie napovedy\n";
                exit(0);
            }
            else
            {
                fwrite(STDERR, "ERROR, bad help argument\n");
                exit(10);
            }
        }
        if (array_key_exists("directory", $args)) {
            $path = preg_grep("/^--directory=.+$/", $argv);
            if (count($path) != 1) {
                fwrite(STDERR, "ERROR, too many --directory\n");
                exit(10);
            }
            $path = array_pop($path);
            $path = explode("=", $path);
            $path = $path[1];
            if (substr($path, -1) != '/')
            {
                $path = $path . '/';
            }
            if (!is_dir(realpath($path)))
            {
                fwrite(STDERR, "ERROR, not such a directory\n");
                exit(11);
            }
            $directory_arg = true;
        }
        if (array_key_exists("recursive", $args)) {
            $recursive_arg = true;
        }
        if (array_key_exists("parse-script", $args)) {
            $file_p = preg_grep("/^--parse-script=.+$/", $argv);
            if (count($file_p) != 1) {
                fwrite(STDERR, "ERROR, too many --parse-script\n");
                exit(10);
            }
            $file_p = array_pop($file_p);
            $file_p = explode("=", $file_p);
            $file_p = $file_p[1];
            $parse_script_arg = true;
        }
        if (array_key_exists("int-script", $args)) {
            $file_i = preg_grep("/^--int-script=.+$/", $argv);
            if (count($file_i) != 1) {
                fwrite(STDERR, "ERROR, too many --int-script\n");
                exit(10);
            }
            $file_i = array_pop($file_i);
            $file_i = explode("=", $file_i);
            $file_i = $file_i[1];
            $int_script_arg = true;
        }
        if (array_key_exists("parse-only", $args)) {
            $parse_only_arg = true;
        }
        if (array_key_exists("int-only", $args)) {
            $int_only_arg = true;
        }
        if (array_key_exists("jexamxml", $args)) {
            $file_j = preg_grep("/^--jexamxml=.+$/", $argv);
            if (count($file_j) != 1) {
                fwrite(STDERR, "ERROR, too many --jexamxml\n");
                exit(10);
            }
            $file_j = array_pop($file_j);
            $file_j = explode("=", $file_j);
            $file_j = $file_j[1];
            $jexamxml_arg = true;
        }
    }
    else
    {
        fwrite(STDERR, "ERROR, bad argument\n");
        exit(10);
    }
}

check_arg_validity();

if ($parse_script_arg)
{
    if (!file_exists($file_p))
    {
        fwrite(STDERR, "ERROR, file doesnt exist!");
        exit(10);
    }
}
if ($int_script_arg)
{
    if (!file_exists($file_i))
    {
        fwrite(STDERR, "ERROR, file doesnt exist!");
        exit(10);
    }
}
if ($directory_arg)
{
    if (!is_dir($path))
    {
        fwrite(STDERR, "ERROR, directory doesnt exist!");
        exit(10);
    }
}
if($jexamxml_arg)
{
    if (!file_exists($file_j))
    {
        fwrite(STDERR, "ERROR, file doesnt exist!");
        exit(10);
    }
}

html_header();
find_files($path);
create_missing_files();
run_tests();
echo
"
            </table>
        </div>
    </body>
</html>
";
exec("rm -rf tmp_parse");
exec("rm -rf tmp_diff");
?>