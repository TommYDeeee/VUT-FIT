<?php
/* IPP parser for IPPcode20
 * author: Tomáš Ďuriš (xduris05)
 * 2020
 */

 //Pole pre všetky testy
$all_tests = array();

//Funkcia na overenie správnosti kombinácie argumentov
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

//Funkcia na overenie existencie zadaných súborov (ciest k súborom)
function check_files($file_p, $file_i, $file_j, $path)
{
    if (!file_exists($file_p))
    {
        fwrite(STDERR, "ERROR, file parser.php doesnt exist!\n");
        exit(10);
    }
    if (!file_exists($file_i))
    {
        fwrite(STDERR, "ERROR, file interpret.py doesnt exist!\n");
        exit(10);
    }
    if (!file_exists($file_j))
    {
        fwrite(STDERR, "ERROR, file jexamxml doesnt exist!\n");
        exit(10);
    }
    if (!is_dir($path))
    {
        fwrite(STDERR, "ERROR, directory doesnt exist!\n");
        exit(10);
    }
}

//funkcia na nájdenie všetkých testov rekurzívnym prechodom a ich uloženie do pola
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

//Funkcia na dogenerovanie prípadných chýbajúcich súborov .in, .out, .rc, zoradenie kompletných testov podľa abecedy
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
                array_push($all_tests, $dir . "/" . $name . ".in");
            }
            if (!file_exists($dir . "/" . $name . ".out")) {
                exec("touch " . $dir . "/" . $name . ".out");
                array_push($all_tests, $dir . "/" . $name . ".out");
            }
            if (!file_exists($dir . "/" . $name . ".rc")) {
                exec("touch " . $dir . "/" . $name . ".rc");
                $rc_file = fopen($dir . "/" . $name . ".rc", "w");
                fwrite($rc_file, "0");
                fclose($rc_file);
                array_push($all_tests, $dir . "/" . $name . ".rc");
            }
        }
    }
    asort($all_tests);
}

/*Hlavná funkcia na vykonanie testovania, prejdenie cez pole testov, podľa argumentu na vstupe sa vykoná test parse.php alebo interpret.py prípade obidvoch
/*Návratový kód jednotlivých skriptov sa porovná s očakávaným návratovým kódom, v prípade úspechu nasleduje porovnanie cez JEXAMXML, prípade DIFF
/*Počas testovania sa generuje HTML výstup rpe každý test
*/
function run_tests()
{
    global $all_tests, $parse_only_arg, $int_only_arg, $file_p, $file_i, $file_j;
    $count = $count_passed = 0;
    $rc = "";
    $last_dir = "";
    $test_info = "";
    foreach ($all_tests as $test)
    {
        $name = pathinfo($test, PATHINFO_DIRNAME);
        $name = $name . "/" . pathinfo($test, PATHINFO_FILENAME);
        //echo "$name\n";
        exec("touch ./tmp");
        exec("touch ./tmp_diff");
        exec("touch ./tmp_both");
        if (pathinfo($test, PATHINFO_EXTENSION) == "rc")
        {
            $rc = fgets(fopen($test, "r"));
        }
        //vykonanie testu parse.php
        if($parse_only_arg)
        {
            //ak sa jedná o iný priečinok, oddelíme testy
            if(pathinfo($test, PATHINFO_DIRNAME) != $last_dir)
            {
                $dir = pathinfo($test, PATHINFO_DIRNAME);
                echo
                "
                    <tr class='blank_row'>
                        <td colspan='3' class='dir'>
                            $dir
                        </td>
                    </tr>
                ";
            }
            $test_info = "<b>PARSE ONLY:&nbsp&nbsp</b>";
            //testujeme iba súbory s príponou .src
            if (pathinfo($test, PATHINFO_EXTENSION) == "src")
            {
                echo
                "    
                <tr>
                    <td class='number'>".$count."</td>
                    <td>".$test_info.pathinfo($test, PATHINFO_DIRNAME)."/".pathinfo($test, PATHINFO_FILENAME)."</td>
                    <td ";
                //spustenie testov a porovnanie návratového kódu
                exec("php ".$file_p." <".$name.".src > ./tmp 2>/dev/null", $output, $return);
                if ($return == $rc)
                {
                    //v prípade úspechu porovnanie cez nástroj jexamxml
                    if($rc == "0")
                    {
                        $command = "java -jar ".$file_j." tmp ".$name.".out tmp_diff /D options";
                        #echo "$command\n";
                        exec($command,$output, $return);
                        if($return == "0")
                        {
                            echo "class='ok'>OK!";
                            $count_passed++;
                        }
                        else
                        {
                            echo "class='failed'>FAILED!<span>&nbsp(jexamxml match failed)</span>";
                        }
                    }
                    else
                    {
                        echo "class='ok'>OK!";
                        $count_passed++;
                    }
                }
                else
                {
                    echo "class='failed'>FAILED!<span>&nbsp(expected rc: $rc, got: $return)</span>";
                }
                echo
                "</td>
                </tr>
                ";
                $count++;
            }
        }
        //testovanie interpret.py
        elseif ($int_only_arg)
        {
            if(pathinfo($test, PATHINFO_DIRNAME) != $last_dir)
            {
                $dir = pathinfo($test, PATHINFO_DIRNAME);
                echo
                "
                    <tr class='blank_row'>
                        <td colspan='3' class='dir'>
                            $dir
                        </td>
                    </tr>
                ";
            }
            $test_info = "<b>INT ONLY:&nbsp&nbsp</b>";
            if (pathinfo($test, PATHINFO_EXTENSION) == "src")
            {
                echo
                    "    
                <tr>
                    <td class='number'>" . $count . "</td>
                    <td>" . $test_info . pathinfo($test, PATHINFO_DIRNAME) . "/" . pathinfo($test, PATHINFO_FILENAME) . "</td>
                    <td ";
                exec("python " . $file_i . " --source=" . $name . ".src" . " --input=" . $name . ".in" . " > ./tmp 2>/dev/null", $output, $return);
                if ($return == $rc)
                {
                    if ($rc == "0")
                    {
                        exec("diff ./tmp " . $name . ".out",$output ,$return);
                        if ($return == "0")
                        {
                            echo "class='ok'>OK!";
                            $count_passed++;
                        }
                        else
                        {
                            echo "class='failed'>FAILED!<span>&nbsp(diff match failed)</span>";
                        }
                    }
                    else
                    {
                        echo "class='ok'>OK!";
                        $count_passed++;
                    }
                }
                else
                {
                    echo "class='failed'>FAILED!<span>&nbsp(expected rc: $rc, got: $return)</span>";
                }
                echo
                "</td>
                </tr>
                ";
                $count++;
            }
        }
        //testovanie parse.php a následne výstup posielame na testovanie interpret.py
        else
        {
            if(pathinfo($test, PATHINFO_DIRNAME) != $last_dir)
            {
                $dir = pathinfo($test, PATHINFO_DIRNAME);
                echo
                "
                    <tr class='blank_row'>
                        <td colspan='3' class='dir'>
                            $dir
                        </td>
                    </tr>
                ";
            }
            $test_info = "<b>BOTH:&nbsp&nbsp</b>";
            if (pathinfo($test, PATHINFO_EXTENSION) == "src")
            {
                echo
                    "    
                <tr>
                    <td class='number'>" . $count . "</td>
                    <td>" . $test_info . pathinfo($test, PATHINFO_DIRNAME) . "/" . pathinfo($test, PATHINFO_FILENAME) . "</td>
                    <td ";
                exec("php " . $file_p . " <" . $name . ".src > ./tmp 2>/dev/null", $output, $return);
                if($return == "0")
                {
                    //testovanie výstupu parse.php
                    exec("python " . $file_i . " --source=./tmp" . " --input=" . $name . ".in" . " > ./tmp_both 2>/dev/null", $output, $return);
                    if ($return == $rc)
                    {
                        if ($rc == "0")
                        {
                            exec("diff ./tmp_both " . $name . ".out", $output, $return);
                            if ($return == "0")
                            {
                                echo "class='ok'>OK!";
                                $count_passed++;
                            }
                            else
                            {
                                echo "class='failed'>FAILED!<span>&nbsp(diff match failed)</span>";
                            }
                        }
                        else
                        {
                            echo "class='ok'>OK!";
                            $count_passed++;
                        }
                    }
                    else
                    {
                        echo "class='failed'>FAILED!<span>&nbsp(expected rc: $rc, got: $return)</span>"; 
                    }
                }
                elseif($return == $rc)
                {
                    echo "class='ok'>OK!";
                    $count_passed++;
                }
                else
                {
                    echo "class='failed'>FAILED!<span>&nbsp(expected rc: $rc, got: $return)</span>";
                }
                echo
                "</td>
                </tr>
                ";
                $count++;
            }
        }
        $last_dir = pathinfo($test, PATHINFO_DIRNAME);
    }
    //vrátime počet úspešných a počet celkových testov
    return array($count_passed, $count);
}

//generujeme hlavičku HTML a štýľ jednoltivých elementov, celkový vzhľad
function html_header()
{
    echo
"<!DOCTYPE html>
<html style='background-color: #282828; color: white'>
    <head>
        <meta charset='UTF-8'>
        <title>TEST RESULTS</title>
    </head>
        <style>
            h1
            {
                text-align: center;
                font-size: 400%;
                color: #4381F0; 
                margin-bottom: 0;
            }
            h2
            {
                text-align: center;
                font-size: 200%;
                color: #27e8a7; 
            }
            h3
            {
                text-align: center;
                font-size: 200%;
                color: #cc0000; 
            }
            h5
            {
                font-size: 200%;
                color: #4381F0;
                text-align: center;
                padding-bottom: 5%;
                margin-top: 0; 
            }
            tr
            {
                border: 2px solid black;
            }
            td
            {
                text-align: left;
                border: 2px solid black;
            }
            td.failed
            {
                background-color: #ff9999;
                text-align: center;
                color: #cc0000;
                font-weight: bold;
            }
            td.ok
            {
                text-align: center;
                color:green;
                background-color:#30EB99;
                font-weight: bold; 
            }
            span
            {
                font-size: small;
                font-style: italic;
                color: black;
            }
            table.center
            {
                border-collapse: collapse;  
                width: 75%;
                margin-left: auto;
                margin-right: auto;
            }
            td.number
            {
                width: 10%;
                font-weight: bolder;
            }
            .blank_row
            {
                background-color: #4381F0;
            }
            .dir
            {
                height: 25pt;
                text-align: center;
                font-weight: bold;
            }
        </style>
    <body>
    <h1>TEST SUMMARY</h1>
    <h5>(test.php script for testing IPPcode20 language)</h5>
    <table class='center'>
        <tr>
            <th class='number'>Number</th>
            <th>Name</th>
            <th>Result</th>
        </tr>";
}

//generovanie HTML zápätia s počtom úspešných / počtom celkových testov a percentuálnou úspešnosťou
function html_footer($count_passed)
{
    if($count_passed[1] != 0) {
        $passed_percentage = ($count_passed[0] / $count_passed[1] * 100);
        $passed_percentage = round($passed_percentage, 2);
        if ($passed_percentage >= 50) {
            echo
            "
                </table>
                <h2>PASSED TESTS: $count_passed[0] / $count_passed[1]</h2>
                <h2>$passed_percentage%</h2>
            </div>
        </body>
    </html>
    ";
        } else {
            echo
            "
                </table>
                <h3>PASSED TESTS: $count_passed[0] / $count_passed[1]</h3>
                <h3>$passed_percentage%</h3>
            </div>
        </body>
    </html>
    ";
        }
    }
}

//Globálne premenné pre spracovanie argumentov a predvolené hodnoty
$options = array("help", "directory", "recursive", "parse-script", "int-script", "parse-only", "int-only", "jexamxml");
$args = getopt("", $options);
$directory_arg = $recursive_arg = $parse_script_arg = $int_script_arg = $parse_only_arg = $int_only_arg = $jexamxml_arg = false;
$path = getcwd();
$path = $path . '/';
$file_p = realpath("./parse.php");
$file_i = realpath("./interpret.py");
$file_j = realpath("/pub/courses/ipp/jexamxml/jexamxml.jar");

/*Spracovanie argumentov
/*Výpis nápovedy ak je zadaný argument
/*Uloženie zadaných hodnôt argumentov
*/
if((count(array_unique($argv))-1) != count($args))
{
    fwrite(STDERR, "ERROR, bad argument\n");
    exit(10);
}

if($argc > 1)
{
    if(!empty($args)) {
        if (array_key_exists("help", $args)) {
            if ($argc == 2) {
                echo "test.php\n";
                echo "Napoveda:\n";
                echo "Autor: Tomas Duris (xduris05)\n\n";
                echo "Skript test.php skontroluje správnosť implementácie skriptu parser.php a interpret.py na zvolenej testovacej sade\n";
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
check_files($file_p, $file_i, $file_j, $path);
html_header();
find_files($path);
create_missing_files();
$count_passed = run_tests();
html_footer($count_passed);
//Odstránenie dočasných súborov
exec("rm -rf tmp_diff");
exec("rm -rf tmp");
exec("rm -rf tmp_both");
?>
