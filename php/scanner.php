<?php
function skipComment($str, $ptr)
{
    $length = strlen($str);
    $line = 0;
    for ($i = $ptr; $i < $length; $i++) {
        $asciiCode = ord($str[$i]);
        //if($asciiCode == 32 || $asciiCode == 9){
        if (isWhiteSpace($asciiCode)) {
            continue;
        } else if ($asciiCode == 10 || $asciiCode == 13) {
            $txt = sprintf("line : %d\n", ++$line);
            $i++;
            break;
        } else {
            if ($i != 0) {
                // $i--;
            }
            break;
        }
    }
    //if($i > 0){
    //	$i--;
    //}

    return $i;
}

function getKeyword($str, $ptr)
{
    $length = strlen($str);
    if ($ptr && $ptr >= $length) return $ptr;
    if ($length == 0) return $ptr;
    $keyword = '';
    $ch = $str[$ptr];
    $i = $ptr;
    if (isCharacter($ch)) {
        do {
            if ($i >= $length) return $i;
            $keyword .= $ch;
            $ch = $str[++$i];
        } while (isCharacter($ch) || isNumber($ch));
    }
    if (!empty($keyword)) {
        addToHash($keyword);
    }
    //return ($i ? $i - 1 : $i);
    return $i;
}

function addToHash($data)
{
    var_dump($data);
    echo "\n";
}

function isCharacter($ch)
{
    $asciiCode = ord($ch);
    return ((65 <= $asciiCode && $asciiCode <= 90) || (97 <= $asciiCode && $asciiCode <= 122));
}

function isNumber($ch)
{
    $asciiCode = ord($ch);
    return (48 <= $asciiCode && $asciiCode <= 57);
}

function getInteger($str, $ptr)
{
    $length = strlen($str);
    if ($ptr >= $length) return $ptr;
    $character = $str[$ptr];
    $asciiCode = ord($character);
    $v = 0;
    $i = $ptr;
    if (48 <= $asciiCode && $asciiCode <= 57) {
        do {
            if ($i >= $length) return $i;
            $v = $v * 10 + $character;
            $character = $str[++$i];
            $asciiCode = ord($character);
        } while (48 <= $asciiCode && $asciiCode <= 57);
    }
    if ($v > 0) {
        addToHash($v);
    }

    // return ($i ? $i - 1 : $i);
    return $i;
}

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
        if ($c++ == 1) {
            //	break;
        }
        $i = skipComment($str, $i);
        $i = getKeyword($str, $i);
        $i = skipComment($str, $i);
        $i = getInteger($str, $i);
    }
}

function isWhiteSpace($asciiCode)
{
//    $arr = [40, 41, 36, 59, 32, 9];
    $arr = [
        9,      // 水平制表符 Tab
        32,     // 空格
        33,     // !
        40,     //'(',    // 40
        41,     //')',    // 41
        44,     // ,
        46,     // .
        59,     // ;
        34,     // "
        123,    // {
        125,    // }
    ];
    return in_array($asciiCode, $arr);
}

$filename = 'data3.txt';
$str = file_get_contents($filename);
//var_dump($str);
scan($str);
