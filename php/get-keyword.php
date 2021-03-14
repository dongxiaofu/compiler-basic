<?php
function scan($str)
{
	$length = strlen($str);
	if($length == 0) return "";
	$keyword = '';
	$ch = $str[0];
	$i = 1;
	if(isCharacter($ch)){
		do{
			$keyword .= $ch;
			$ch = $str[$i];
			$i++;
		}while(isCharacter($ch) || isNumber($ch));
	}

	return $keyword;
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

$str = file_get_contents('data1.txt');
$keyword = scan($str);
echo $keyword;
echo "\n";
