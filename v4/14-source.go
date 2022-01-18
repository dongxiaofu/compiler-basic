package main

import (
    "fmt"
)

func (de *DivideError) Error() string {
   switch marks {
      case 90: grade = "A"
      case 80: grade = "B"
      case 50,60,70 : grade = "C"
      default: grade = "D"  
   }

	select {
      case i1 = <-c1:
         fmt.Printf("received ", i1, " from c1\n")
      case c2 <- i2:
         fmt.Printf("sent ", i2, " to c2\n")
      case i3, ok := (<-c3):
         if ok > 5 {
            fmt.Printf("received ", i3, " from c3\n")
         } else {
            fmt.Printf("c3 is closed\n")
         }
      default:
         fmt.Printf("no communication\n")
   }    
            fmt.Printf("received ", i3, " from c3\n")
            fmt.Printf("received ", i3, " from c3\n")
            fmt.Printf("received ", i3, " from c3\n")
            fmt.Printf("received ", i3, " from c3\n")
            fmt.Printf("received ", i3, " from c3\n")
            fmt.Printf("received ", i3, " from c3\n")
            fmt.Printf("received ", i3, " from c3\n")
            fmt.Printf("received ", i3, " from c3\n")
            fmt.Printf("received ", i3, " from c3\n")
            fmt.Printf("received ", i3, " from c3\n")
            fmt.Printf("received ", i3, " from c3\n")
            fmt.Printf("received ", i3, " from c3\n")
	if ok > 5 {
            fmt.Printf("received ", i3, " from c3\n")
         } else {
            fmt.Printf("c3 is closed\n")
         }
	if ok > 5 {
            fmt.Printf("received ", i3, " from c3\n")
         } else {
            fmt.Printf("c3 is closed\n")
         }
	if ok > 5 {
            fmt.Printf("received ", i3, " from c3\n")
         } else {
            fmt.Printf("c3 is closed\n")
         }

	if ok > 5 {
            fmt.Printf("received ", i3, " from c3\n")
         } else {
            fmt.Printf("c3 is closed\n")
         }

	select {
      case i1 = <-c1:
         fmt.Printf("received ", i1, " from c1\n")
      case c2 <- i2:
         fmt.Printf("sent ", i2, " to c2\n")
      case i3, ok := (<-c3):
         if ok > 5 {
            fmt.Printf("received ", i3, " from c3\n")
         } else {
            fmt.Printf("c3 is closed\n")
         }
      default:
         fmt.Printf("no communication\n")
   }    

	var grade string = "B"
   var marks int = 90

   switch marks {
      case 90: grade = "A"
      case 80: grade = "B"
      case 50,60,70 : grade = "C"
      default: grade = "D"  
   }

	switch {
      case grade == "A" :
         fmt.Printf("A!\n" )    
      case grade == "B":
         fmt.Printf("B\n" )      
      case grade == "D" :
         fmt.Printf("C\n" )      
      case grade == "F":
         fmt.Printf("D\n" )
      default:
         fmt.Printf("F\n" );
   }
}

	var a int
    return fmt.Sprintf(strFormat, de.dividee)
}
