type (
	nodeList = []*Node
	Polar= polar
)

type (
	Point struct{ x, y float64 }
	polar Point
)

type TreeNode struct {
	left, right *TreeNode
	value *Comparable
}

type Block interface {
	BlockSize() int
	Encrypt(src, dst []byte)
	Decrypt(src, dst []byte)
}

type NewMutex Mutex
type PtrMutex *Mutex
type PrintableMutex struct {
	Mutex
}
type MyBlock Block
