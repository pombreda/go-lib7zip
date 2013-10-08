package lib7zip

// #cgo CXXFLAGS: -std=c++11  -I.
// #cgo LDFLAGS: -l7zip -ldl
// #include "wrap.h"
// #include <stdlib.h>
import "C"

import (
	"errors"
	"reflect"
	"runtime"
	"time"
	"unicode/utf16"
	"unsafe"
)

var (
	ErrUnknown      = errors.New("lib7zip: unknown error")
	ErrInit         = errors.New("lib7zip: initialization error")
	ErrPassword     = errors.New("lib7zip: wrong password")
	ErrNotSupported = errors.New("lib7zip: unsupported archive type")
	ErrInvalid      = errors.New("lib7zip: invalid error")
)

type PropertyIndex int

var (
	PackSize    PropertyIndex = C.kpidPackSize
	Attrib      PropertyIndex = C.kpidAttrib
	CTime       PropertyIndex = C.kpidCTime
	ATime       PropertyIndex = C.kpidATime
	MTime       PropertyIndex = C.kpidMTime
	Solid       PropertyIndex = C.kpidSolid
	Encrypted   PropertyIndex = C.kpidEncrypted
	User        PropertyIndex = C.kpidUser
	Group       PropertyIndex = C.kpidGroup
	Comment     PropertyIndex = C.kpidComment
	PhySize     PropertyIndex = C.kpidPhySize
	HeadersSize PropertyIndex = C.kpidHeadersSize
	Checksum    PropertyIndex = C.kpidChecksum
	Characts    PropertyIndex = C.kpidCharacts
	CreatorApp  PropertyIndex = C.kpidCreatorApp
	TotalSize   PropertyIndex = C.kpidTotalSize
	FreeSpace   PropertyIndex = C.kpidFreeSpace
	ClusterSize PropertyIndex = C.kpidClusterSize
	VolumeName  PropertyIndex = C.kpidVolumeName
	Path        PropertyIndex = C.kpidPath
	IsDir       PropertyIndex = C.kpidIsDir
	Size        PropertyIndex = C.kpidSize
)

func getBool(b C.bool) bool {
	return int(b) != 0
}

// from syscall_windows.go

// UTF16FromString returns the UTF-16 encoding of the UTF-8 string
// s, with a terminating NUL added. If s contains a NUL byte at any
// location, it returns (nil, EINVAL).
func _UTF16FromString(s string) ([]uint16, error) {
	for i := 0; i < len(s); i++ {
		if s[i] == 0 {
			return nil, ErrUnknown
		}
	}
	return utf16.Encode([]rune(s + "\x00")), nil
}

// UTF16ToString returns the UTF-8 encoding of the UTF-16 sequence s,
// with a terminating NUL removed.
func _UTF16ToString(s []uint16) string {
	for i, v := range s {
		if v == 0 {
			s = s[0:i]
			break
		}
	}
	return string(utf16.Decode(s))
}

func getString(ws *C.wchar_t) string {
	length := int(C.wcslen(ws))
	var sl []C.wchar_t
	sliceHeader := (*reflect.SliceHeader)((unsafe.Pointer(&sl)))
	sliceHeader.Cap = length
	sliceHeader.Len = length
	sliceHeader.Data = uintptr(unsafe.Pointer(ws))

	s := make([]uint16, length)
	for i := 0; i < length; i++ {
		s[i] = uint16(sl[i])
	}

	return _UTF16ToString(s)
}

func getError(e C.ErrorCodeEnum) error {
	switch int(e) {
	case C.UNKNOWN_ERROR:
		return ErrUnknown
	case C.NOT_INITIALIZE:
		return ErrInit
	case C.NEED_PASSWORD:
		return ErrPassword
	case C.NOT_SUPPORTED_ARCHIVE:
		return ErrNotSupported
	}

	return ErrInvalid
}

type Library struct {
	l unsafe.Pointer //*C.c7z_Library
}

func (lib *Library) Close() {
	if lib.l == nil {
		return
	}

	if getBool(C.c7zLib_IsInitialized(lib.l)) {
		C.c7zLib_Deinitialize(lib.l)
	}
	C.free_C7ZipLibrary(lib.l)
	lib.l = nil
}

func (lib *Library) finalizer(*Library) {
	lib.Close()
}

func NewLibrary() (*Library, error) {
	l := C.create_C7ZipLibrary()
	if getBool(C.c7zLib_Initialize(l)) == false {
		err := getError(C.c7zLib_GetLastError(l))
		C.free_C7ZipLibrary(l)
		return nil, err
	}
	lib := &Library{l}
	runtime.SetFinalizer(lib, lib.finalizer)
	return lib, nil
}

func (lib *Library) SupportedExtensions() ([]string, error) {
	var n C.size_t
	var exts **C.wchar_t
	if getBool(C.c7zLib_GetSupportedExts(lib.l, &exts, &n)) == false {
		return nil, lib.lastError()
	}
	defer C.free_extarr(exts)

	extensions := make([]string, int(n), int(n))
	for i := C.int(0); i < C.int(n); i++ {
		ws := C.lib7zip_wchar_array(exts, i)
		extensions[i] = getString(ws)
	}

	return extensions, nil
}

func (lib *Library) lastError() error {
	return getError(C.c7zLib_GetLastError(lib.l))
}

func (lib *Library) Open(r Reader) (*Archive, error) {
	ar := &Archive{lib: lib}

	if getBool(C.lib7zip_open_archive(unsafe.Pointer(&r), lib.l, &ar.s, &ar.a)) == false {
		return nil, lib.lastError()
	}
	runtime.SetFinalizer(ar, ar.finalizer)
	return ar, nil
}

type Archive struct {
	a   unsafe.Pointer //*C.c7z_Archive
	s   unsafe.Pointer //*C.c7z_InStream
	lib *Library
}

func (ar *Archive) NItems() (int, error) {
	var cn C.uint
	if getBool(C.c7zArc_GetItemCount(ar.a, &cn)) == false {
		return 0, ar.lib.lastError()
	}
	return int(cn), nil
}

func (ar *Archive) Close() {
	if ar.a == nil {
		return
	}

	C.c7zArc_Close(ar.a)
	C.free_C7ZipArchive(ar.a)
	ar.a = nil
}

func (ar *Archive) finalizer(*Archive) {
	ar.Close()
}

func (ar *Archive) Item(index int) (*Item, error) {
	it := &Item{ar: ar}
	if getBool(C.c7zArc_GetItemInfo(ar.a, C.uint(index), &it.i)) == false {
		return nil, ar.lib.lastError()
	}
	runtime.SetFinalizer(it, it.finalizer)
	return it, nil
}

func (ar *Archive) Extract(index int, w Writer) error {
	if getBool(C.lib7zip_archive_extract(ar.a, C.uint(index), unsafe.Pointer(&w))) == false {
		return ar.lib.lastError()
	}
	return nil
}

func (ar *Archive) ExtractWithPassword(index int, w Writer, s string) error {
	u16s, err := _UTF16FromString(s)
	if err != nil {
		return err
	}
	ws := make([]C.wchar_t, len(u16s)+1)
	for i := range u16s {
		ws[i] = C.wchar_t(u16s[i])
	}

	if getBool(C.lib7zip_archive_extract_password(ar.a, C.uint(index), unsafe.Pointer(&w), &ws[0])) == false {
		return ar.lib.lastError()
	}
	return nil
}

func (ar *Archive) PropertyUint64(index PropertyIndex) (uint64, error) {
	var cval C.ulonglong
	if getBool(C.c7zArc_GetUInt64Property(ar.a, C.int(index), &cval)) == false {
		return 0, ar.lib.lastError()
	}
	return uint64(cval), nil
}

func (ar *Archive) PropertyBool(index PropertyIndex) (bool, error) {
	var cval C.bool
	if getBool(C.c7zArc_GetBoolProperty(ar.a, C.int(index), &cval)) == false {
		return false, ar.lib.lastError()
	}
	return getBool(cval), nil
}

func (ar *Archive) PropertyString(index PropertyIndex) (string, error) {
	var cval *C.wchar_t
	if getBool(C.c7zArc_GetStringProperty(ar.a, C.int(index), &cval)) == false {
		return "", ar.lib.lastError()
	}

	prop := getString(cval)
	C.free(unsafe.Pointer(cval))
	return prop, nil
}

func (ar *Archive) PropertyFileTime(index PropertyIndex) (uint64, error) {
	var cval C.ulonglong
	if getBool(C.c7zArc_GetFileTimeProperty(ar.a, C.int(index), &cval)) == false {
		return 0, ar.lib.lastError()
	}
	return uint64(cval), nil
}

type Item struct {
	i  unsafe.Pointer //*C.c7z_ArchiveItem
	ar *Archive
}

func (it *Item) finalizer(*Item) {
	if it.i == nil {
		return
	}

	//C.free(it.i)
	it.i = nil
}

func (it *Item) Extract(w Writer) error {
	if getBool(C.lib7zip_item_extract(it.ar.a, it.i, unsafe.Pointer(&w))) == false {
		return it.ar.lib.lastError()
	}
	return nil
}

func (it *Item) PropertyUint64(index PropertyIndex) (uint64, error) {
	var cval C.ulonglong
	if getBool(C.c7zItm_GetUInt64Property(it.i, C.int(index), &cval)) == false {
		return 0, it.ar.lib.lastError()
	}
	return uint64(cval), nil
}

func (it *Item) PropertyBool(index PropertyIndex) (bool, error) {
	var cval C.bool
	if getBool(C.c7zItm_GetBoolProperty(it.i, C.int(index), &cval)) == false {
		return false, it.ar.lib.lastError()
	}
	return getBool(cval), nil
}

func (it *Item) PropertyString(index PropertyIndex) (string, error) {
	var cval *C.wchar_t
	if getBool(C.c7zItm_GetStringProperty(it.i, C.int(index), &cval)) == false {
		return "", it.ar.lib.lastError()
	}
	prop := getString(cval)
	C.free(unsafe.Pointer(cval))
	return prop, nil
}

func (it *Item) PropertyFileTime(index PropertyIndex) (uint64, error) {
	var cval C.ulonglong
	if getBool(C.c7zItm_GetFileTimeProperty(it.i, C.int(index), &cval)) == false {
		return 0, it.ar.lib.lastError()
	}
	return uint64(cval), nil
}

func (it *Item) FullPath() string {
	cval := C.c7zItm_GetFullPath(it.i)
	path := getString(cval)
	C.free(unsafe.Pointer(cval))
	return path
}

func (it *Item) Size() uint64 {
	return uint64(C.c7zItm_GetSize(it.i))
}

func (it *Item) IsDir() bool {
	return getBool(C.c7zItm_IsDir(it.i))
}

func (it *Item) IsEncrypted() bool {
	return getBool(C.c7zItm_IsEncrypted(it.i))
}

func (it *Item) Index() int {
	return int(C.c7zItm_GetArchiveIndex(it.i))
}

func ToTime(t uint64) time.Time {
	secpy := uint64(31536000)
	gain := uint64(10000000)
	bias := secpy*gain*369 + secpy*2438356 + 5184000
	return time.Unix(int64((t-bias)/gain), int64(((t-bias)%gain)*100))
}
