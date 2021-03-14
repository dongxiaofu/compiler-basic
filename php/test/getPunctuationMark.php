<?php
//declare(strict_types=1);
require_once '../go-scanner.php';

function scan($str)
{
    $length = strlen($str);
    /*
    for($i = 0; $i < $length; $i++){
        $i = skipComment($str, $i);
        $i = getKeyword($str, $i);
        $i = skipComment($str, $i);
        $i = getInteger($str, $i);
    }
     */
    $i = 0;
    $c = 0;
    while (true) {
        if ($i >= $length) {
            break;
        }
        if ($c++ == 400) {
            	break;
        }
        $i = skipComment($str, $i);
        $i = getComment($str, $i);
        $i = getKeyword($str, $i);
        $i = getPunctuationMark($str, $i);
        $i = getInteger($str, $i);
//        $i = skipComment($str, $i);
//        $i = getInteger($str, $i);
    }
}


$filename = 'data2.txt';
$str = file_get_contents($filename);
//var_dump($str);
scan($str);
