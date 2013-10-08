package main

import (
	"github.com/salviati/go-lib7zip/lib7zip"
	"log"
	"os"
	"path/filepath"
)

type File struct {
	*os.File
}

func NewFile(f *os.File) *File {
	return &File{f}
}

func (r *File) Size() (int64, error) {
	fi, err := r.Stat()
	if err != nil {
		return 0, err
	}
	return fi.Size(), nil
}

func (r *File) SetSize(n int64) error {
	return nil
}

func (r *File) Ext() string {
	ext := filepath.Ext(r.Name())
	if len(ext) <= 1 || ext[0] != '.' {
		return ""
	}

	return ext[1:]
}

func main() {
	f, err := os.Open("example.7z")
	if err != nil {
		log.Fatal(err)
	}
	defer f.Close()

	lib, err := lib7zip.NewLibrary()
	if err != nil {
		log.Fatal(err)
	}

	log.Println(lib.SupportedExtensions())

	ar, err := lib.Open(NewFile(f))
	if err != nil {
		log.Fatal(err)
	}

	n, err := ar.NItems()
	if err != nil {
		log.Fatal(err)
	}
	log.Println(n)

	it, err := ar.Item(2)
	if err != nil {
		log.Fatal("item:", err)
	}

	t, err := it.PropertyFileTime(lib7zip.MTime)
	if err != nil {
		log.Fatal("filetime:", err)
	}
	log.Println(t, lib7zip.ToTime(t))

	name, err := it.PropertyString(lib7zip.Path)
	if err != nil {
		log.Fatal("filename:", err)
	}

	log.Println(name)

	w, err := os.OpenFile("out", os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0666)
	if err != nil {
		log.Fatal(err)
	}

	//ar.Extract(0, NewFile(w))
	it.Extract(NewFile(w))
}
