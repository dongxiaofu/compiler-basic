<?php
main();
function main(){

	$a = 20;

	switch($a){
		case $a < 10:
			printf("a\n");
		case ($a < 30):
			printf("b\n");
		case $a < 50:
			printf("c\n");
		default:
			printf("d\n");	

	}

	return 0;
}
