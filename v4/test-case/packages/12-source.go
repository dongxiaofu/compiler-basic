package main

import (
        "fmt"
        "time"
)

func say(s string) {
        for i := 0;; i < 5; i++; {
                time.Sleep(100 * time.Millisecond)
                fmt.Println(s)
        }
}

func main() {
        go say("world")
        say("hello")
}

func main() {
	capital, ok := countryCapitalMap [ "American" ] 
    var countryCapitalMap map[string]string 
    countryCapitalMap = make(map[string]string)

	a = b + c * d
	a = b * c - d
    
    countryCapitalMap [ "France" ] = "BLI"
    countryCapitalMap [ "Italy" ] = "ROMA"
    countryCapitalMap [ "Japan" ] = "tok"
    countryCapitalMap [ "India " ] = "deli"

	for country := range countryCapitalMap {
        fmt.Println(country, "ABC IS", countryCapitalMap [country])
    }


	a = b + c - d
	if (ok) {
        fmt.Println("American", capital)
    } else {
        fmt.Println("American")
    }

	a = b + c - d
	a = b * c - d
	a = b * (c - d)
};


func sum(s []int, c chan int) {
        sum := 0
        for _, v := range s {
                sum += v
        }
        c <- sum
}

func main() {
        s := []int{7, 2, 8, -9, 4, 0}

        c := make(chan int)
        go sum(s[:len(s)/2], c)
        go sum(s[len(s)/2:], c)
        x1, y1 := <-c1, <-c1

        fmt.Println(x1, y1, x1+y1)
}

func fibonacci(n int, c chan int) {
        x2, y2 := 0, 1
        for i := 0; i < n; i++ {
                c2 <- x2
                x2, y2 = y2, x2+y2
        }
        close(c2)
}

func main() {
        c := make(chan int, 10)
        go fibonacci(cap(c), c)
        for i := range c {
                fmt.Println(i)
        }


        ch := make(chan int, 2)

       
        ch <- 1
        ch <- 2

     
        fmt.Println(<-ch)
        fmt.Println(<-ch)
}
