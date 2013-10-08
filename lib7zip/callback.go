package lib7zip

// #include "wrap.h"
import "C"
import (
	"reflect"
	"unsafe"
)

func cbytes(cbuf unsafe.Pointer, clen int) []byte {
	slice := (*[1 << 30]byte)(cbuf)[:clen]
	hdr := (*reflect.SliceHeader)(unsafe.Pointer(&slice))
	hdr.Cap = clen
	hdr.Len = clen

	return slice
}

//export go_ReaderRead
func go_ReaderRead(r Reader, cbuf unsafe.Pointer, clen C.uint, count *C.uint) C.int {
	data := cbytes(cbuf, int(clen))
	n, err := r.Read(data)
	if err != nil {
		return -1
	}

	if count != nil {
		*count = C.uint(n)
	}
	return 0
}

//export go_ReaderExt
func go_ReaderExt(r Reader) string {
	return r.Ext()
}

//export go_ReaderSize
func go_ReaderSize(r Reader, cret *int64) C.int {
	var err error
	*cret, err = r.Size()

	if err != nil {
		return -1
	}
	return 0
}

//export go_ReaderSeek
func go_ReaderSeek(r Reader, offset int64, whence int, cret *int64) C.int {
	ret, err := r.Seek(offset, whence)
	if cret != nil {
		*cret = ret
	}
	if err != nil {
		return -1
	}
	return 0
}

//export go_WriterSize
func go_WriterSize(w Writer) C.int {
	n, err := w.Size()
	if err != nil {
		return -1
	}
	return C.int(n)
}

//export go_WriterSetSize
func go_WriterSetSize(w Writer, n int64) C.int {
	if err := w.SetSize(n); err != nil {
		return -1
	}
	return 0
}

//export go_WriterSeek
func go_WriterSeek(w Writer, offset int64, whence int, cret *int64) C.int {
	ret, err := w.Seek(offset, whence)
	if cret != nil {
		*cret = ret
	}
	if err != nil {
		return -1
	}
	return 0
}

//export go_WriterWrite
func go_WriterWrite(w Writer, cbuf unsafe.Pointer, clen C.uint, count *C.uint) C.int {
	data := cbytes(cbuf, int(clen))
	n, err := w.Write(data)

	if count != nil {
		*count = C.uint(n)
	}

	if err != nil {
		return -1
	}
	return 0
}
