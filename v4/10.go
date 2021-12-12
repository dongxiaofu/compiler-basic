func test(age ,height float64) (int,float32){
  var res int = age
  var hei float32 = height


	switch i := x.(type) {
case nil:
	printString("x is nil")                
case int:
	printInt(i)                            
case float64:
	printFloat64(i)                        
case func(int) float64:
	printFunction(i)                       
case bool, string:
	printString("type is bool or string")  
default:
	printString("don't know the type")     
}
}
