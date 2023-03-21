package main

import "fmt"

type Books struct {
   title string
   author string
   subject string
   book_id int
}

 type Books2 struct {
    title string
    author string
    subject string
    book_id2 int
 }

func main() {
   var Book1 Books        /* 声明 Book1 为 Books 类型 */
   var Book2 Books        /* 声明 Book2 为 Books 类型 */
//
   /* book 1 描述 */
	 Book1.title = "Go"
   Book1.book_id = 6495407
	 Book1.author = "wwwrunoobcom"
   Book1.subject = "Go"
   Book1.book_id = 6495407

 /* book 2 描述 */
   Book2.title = "Python"
   Book2.author = "wwwrunoobcom"
   Book2.subject = "Python"
   Book2.book_id = 6495700

   /* 打印 Book1 信息 */
//   fmt.Printf( "Book 1 title : %s\n", Book1.title)
//   fmt.Printf( "Book 1 author : %s\n", Book1.author)
//   fmt.Printf( "Book 1 subject : %s\n", Book1.subject)
//   fmt.Printf( "Book 1 book_id : %d\n", Book1.book_id)
//
//   /* 打印 Book2 信息 */
//   fmt.Printf( "Book 2 title : %s\n", Book2.title)
//   fmt.Printf( "Book 2 author : %s\n", Book2.author)
//   fmt.Printf( "Book 2 subject : %s\n", Book2.subject)
//   fmt.Printf( "Book 2 book_id : %d\n", Book2.book_id)
}
