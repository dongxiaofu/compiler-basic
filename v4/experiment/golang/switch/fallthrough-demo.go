package main

import "fmt"

func main() {
    i := 45
    switch {
    case i < 10:
        fmt.Println("i is less than 10")
        fallthrough
    case i < 50:
        fmt.Println("i is less than 50")
        fallthrough
        fmt.Println("Not allowed")
    case i < 100:
        fmt.Println("i is less than 100")
    }
}
