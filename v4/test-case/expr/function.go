func(a, b int, z float64) bool { return a*b < int(z) }
func(x, y int) int { return x + y }
func(ch chan int) { ch <- ACK }(replyChan)
