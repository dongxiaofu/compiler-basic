package main

import (
    "fmt"
)

func (de *DivideError) Error() string {
	var a int
            dData := DivideError{
                    dividee: varDividee,
                    divider: varDivider,
            }
    return fmt.Sprintf(strFormat, de.dividee)
}
