package main

import (
    "fmt"
)

func (de *DivideError) Error() string {
            dData := DivideError{
                    dividee: varDividee,
                    divider: varDivider,
            }
    return fmt.Sprintf(strFormat, de.dividee)
}
