<?php
function scan($str)
{
	$character = $str[0];
	$asciiCode = ord($character);
	$length = strlen($str);
	if(48 <= $asciiCode && $asciiCode <= 57){
		$v = 0;
		$i = 1;
		do{
			$v = $v * 10 + $character;
			if($i >= $length) break;
			$character = $str[$i++];
			$asciiCode = ord($character);
		}while(48 <= $asciiCode && $asciiCode <= 57);
	}

	return $v;
}

$code = file_get_contents('data.txt');
$number = scan($code);
echo $number;
echo "\n";
