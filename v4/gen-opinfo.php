<?php
// global $debug;// = true;
$debug = false;
// 运算符和运算优先级映射表
function parsePrecedenceMap(){
	$str = <<<EOT
5#*  /  %  <<  >>  &  &^
4#+  -  |  ^
3#==  !=  <  <=  >  >=
2#&&
1#||
EOT;
	$pattern = "@([0-9]#.*)\n?@";
	preg_match_all($pattern, $str, $matchs);
	var_dump($matchs[1]);
	$map = [];
	foreach($matchs[1] as $v){
		$arr = explode("#", $v);
		$precedence = $arr[0];
		$operators = $arr[1];
		$operatorArr = explode(" ", $operators);
		foreach($operatorArr as $v){
			$map[trim($v)] = $precedence;
		}
	}
	
	return $map;	
}
// 解析Tokenop
function parseTokenop($arr){
	$res = [];
	foreach($arr as $v){
		$res[] = explode(",", $v);
	}

	return $res;
}

// 解析OPINFO：
function parseOpinfo($arr){
	return parseTokenop($arr);
}


// 解析token
function parseToken($arr){
	return parseTokenop($arr);
}

// 把文件中的数据变成数组。
function parseFile($file){
	global $debug;// = true;
	$content = file_get_contents($file);
	$pattern = "#\((.*)\)#";
	// preg_match_all($content, $pattern, $matchs);
	preg_match_all($pattern, $content, $matchs);
//	var_dump($matchs[1]);
	if($debug){
		var_dump($matchs[1]);
	}
	$arr = parseTokenop($matchs[1]);
//	var_dump($arr);
	
	if($debug){
		var_dump($arr);
	}
	
	return $arr;
}

function findTargetToken($op, $tokenopStrArray){
	$targetToken = -1;
	foreach($tokenopStrArray as $v){
		if(trim($op) == trim($v[1]) || trim($op) == trim($v[2])){
			$targetToken = $v[0];
			break;
		}
	}

	return $targetToken;
}

function findTargetOperator($token, $tokenStrArray){
	$op = -1;
	foreach($tokenStrArray as $v){
		if(trim($token) == trim($v[0])){
			$op = $v[1];
			break;
		}
	}

	return $op;
}

function getPrecedence($op, $map){
	$res = -1;
	foreach($map as $k => $v){
		var_dump(trim($op, " \""));
		if(trim($op, " \"") == $k){
			$res = $v;
			break;
		}
	}

	var_dump($res);
	return $res;
}

function main(){
	global $debug;
	
	$map = parsePrecedenceMap();
	//var_dump($map);

	$i = 0;
	$content = "";
	$opinfoContent = "";
	$opinfo = "/home/cg/compiler-basic/v4/include/opinfo.h";
	$opinfoStrArray = parseFile($opinfo);
	$tokenop = "/home/cg/compiler-basic/v4/include/tokenop.h";
	$tokenopStrArray = parseFile($tokenop);
	$tokens = "/home/cg/compiler-basic/v4/include/tokens.txt";
	$tokensStrArray = parseFile($tokens);
	foreach($opinfoStrArray as $opinfoStr){
		$op = $opinfoStr[0];
		$token = findTargetToken($op, $tokenopStrArray);
		$operator = findTargetOperator($token, $tokensStrArray);
		$opinfoStr[2] = $operator;
		$opinfoStr[1] = getPrecedence($operator, $map);
		// $opinfoContent = "OPINFO(" . $opinfoStr[0] . "2" . $opinfoStr[3]
		$opinfoContent = "OPINFO(" . implode(",", $opinfoStr) . ")\n";
		$content .= $opinfoContent;
		if($debug && $i > 2){
			break;
		}
	}

	echo $content;
}

main();
