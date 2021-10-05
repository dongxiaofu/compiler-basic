type Locker interface {
	Write(p []byte) (n int, err error)
	Close() error
}
