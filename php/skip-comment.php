<?php
function skipComment($str)
{
	$length = strlen($str);
	$line = 0;
	for($i = 0; $i < $length; $i++){
		$asciiCode = ord($str[$i]);
		if($asciiCode == 32 || $asciiCode == 9)	continue;
		else if($asciiCode == 10 || $asciiCode == 13){
			$txt = sprintf("line : %d\n", ++$line);
			echo $txt;
		}else{
			echo $str[$i];
		}
	}
}

$filename = __FILE__;
$str = file_get_contents($filename);
skipComment($str);
