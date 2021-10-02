const (
        age  uint = a[1:3:5];
	age  uint = a[1:4] + a[:] + a[2:] + a[:3] + a[1:3:5];
	age  uint = arr[2] + a[1:4] + a[:] + a[2:] + a[:3] + a[1:3:5];
	age  uint = arr[2] + arr[index + 1] + a[1:4] + a[:] + a[2:] + a[:3] + a[1:3:5];
	age  uint = arr[2] + arr[index + 1] + a[1:4] + a[:] + a[2:] + a[:3] + a[1:3:5] + a[index:index - 1:index % 2];
)
