package lib7zip

// #include "wrap.h"
import "C"
import (
	"io"
)

type Reader interface {
	io.ReadSeeker
	Size() (int64, error)
	Ext() string
}

type Writer interface {
	io.WriteSeeker
	Size() (int64, error)
	SetSize(int64) error
}
