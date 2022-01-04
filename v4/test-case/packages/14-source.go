package main

import (
    "fmt"
)

type DivideError struct {
    dividee int
    divider int
}

func (de *DivideError) Error() string {
            dData := DivideError{
                    dividee: varDividee,
                    divider: varDivider,
            }
    return fmt.Sprintf(strFormat, de.dividee)
}
