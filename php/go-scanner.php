<?php

const SLASH = 47;       // /
const STAR = 42;       // *
const NEW_LINE = 10;   // 换行
const ENTER = 13;       // 回车

function skipComment($str, $ptr)
{
    $length = strlen($str);
    $line = 0;
    for ($i = $ptr; $i < $length; $i++) {
        if(isValid($str, $i) == false){
            break;
        }
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

function isNumber($ch)
{
    $asciiCode = ord($ch);
    return (48 <= $asciiCode && $asciiCode <= 57);
}

function getComment($str, $ptr)
{
    // 暂存注释
    $comment = '';
    // 合法性检查

    if (isValid($str, $ptr) == false) {
        return $ptr;
    }

    $ch = $str[$ptr];
    $i = $ptr;
    if (ord($ch) != SLASH) {
        return $ptr;
    }

    $comment .= $str[$i];

    $i++;

    if (isValid($str, $i) == false) {
        return $ptr;
    }

    $comment .= $str[$i];

    $ch = $str[$i];
    $asciiCode = ord($ch);
    if ($asciiCode != SLASH && $asciiCode != STAR) {
        return $ptr;
    }

    if ($asciiCode == SLASH) {
        while (true) {
            $i++;
            if (isValid($str, $i) == false) {
                return $ptr;
            }
            $ch = $str[$i];
            $asciiCode = ord($ch);
            if ((isNewLine($asciiCode) || isEnter($asciiCode))) {
                break;
            }
            $comment .= $str[$i];
        }
    } else if ($asciiCode == STAR) {
        // /*注释*/这种类型的注释，识别结束符有点棘手
        while (true) {
            $i++;
            if (isValid($str, $i) == false) {
                return $ptr;
            }

            $comment .= $str[$i];
            // 识别结束符 '*/'
            $ch = $str[$i];
            $asciiCode = ord($ch);
            if (isStar($asciiCode)) {
                $i++;
                if (isValid($str, $i) == false) {
                    return $ptr;
                }
                $ch = $str[$i];
                $comment .= $str[$i];
                $asciiCode = ord($ch);
                if (isSlash($asciiCode)) {
                    break;
                }
            }
        }
    }

    addToHash($comment);
    $i = $ptr + strlen($comment);
    return $i;
}

function check($str, $ptr)
{
    if (!isValid($str, $ptr)) {
        echo '越界\n';
        exit(1);
    }
}

function isValid($str, $ptr)
{
    $length = strlen($str);
    if ($ptr >= $length) {
        return false;
    } else {
        return true;
    }
}

function addToHash($data)
{
    var_dump($data);
}

function isCharacter($ch)
{
    $asciiCode = ord($ch);
    return ((65 <= $asciiCode && $asciiCode <= 90) || (97 <= $asciiCode && $asciiCode <= 122));
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

    return $i;
}

function scan2($str)
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
        if ($c++ == 3) {
            	break;
        }
        $i = skipComment($str, $i);
        $i = getComment($str, $i);
        var_dump($i);
        $i = skipComment($str, $i);
        var_dump($i);
        $i = getKeyword($str, $i);
        $i = skipComment($str, $i);


        $i = getPunctuationMark($str, $i);
        $i = skipComment($str, $i);


        $i = getInteger($str, $i);
//        var_dump($i);return;
    }
}

function isWhiteSpace($asciiCode)
{
//    $arr = [40, 41, 36, 59, 32, 9];
    $arr = [
        9,      // 水平制表符 Tab
        32,     // 空格
//        33,     // !
//        40,     //'(',    // 40
//        41,     //')',    // 41
//        44,     // ,
//        46,     // .
//        59,     // ;
//        34,     // "
//        123,    // {
//        125,    // }
    ];
    return in_array($asciiCode, $arr);
}

function isNewLine($asciiCode)
{
    return ($asciiCode == NEW_LINE);
}

function isEnter($asciiCode)
{
    return ($asciiCode == ENTER);
}

function isStar($asciiCode)
{
    return ($asciiCode == STAR);
}

function isSlash($asciiCode)
{
    return ($asciiCode == SLASH);
}

function getPunctuationMark($str, $ptr)
{
    if (isValid($str, $ptr) == false) {
        return $ptr;
    }

    $arr = [
        33,     // !
        40,     //'(',    // 40
        41,     //')',    // 41
        44,     // ,
        46,     // .
        59,     // ;
        34,     // "
        123,    // {
        125,    // }

        61,     // =
    ];

    $ch = $str[$ptr];
    $asciiCode = ord($ch);
    if (in_array($asciiCode, $arr)) {

        addToHash($ch);
        return ++$ptr;
    }

    return $ptr;
}

