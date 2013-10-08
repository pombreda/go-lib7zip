#include <lib7zip.h>

#include <locale>
#include <iostream>
#include <string>
#include <sstream>
#include <string.h>
using namespace std ;
extern "C" {
#include <_cgo_export.h>
}

//#include <codecvt>
#include "wrap.h"


wstring widen( const string& str )
{
	std::wostringstream wstm ;
      wstm.imbue(std::locale("en_US.utf8"));
      const std::ctype<wchar_t>& ctfacet =
		  std::use_facet< std::ctype<wchar_t> >( wstm.getloc() ) ;
      for( size_t i=0 ; i<str.size() ; ++i )
      wstm << ctfacet.widen( str[i] ) ;
      return wstm.str() ;
}
      
string narrow( const wstring& str )
{
	std::ostringstream stm ;
      stm.imbue(std::locale("C"));
      const std::ctype<char>& ctfacet =
		  std::use_facet< std::ctype<char> >( stm.getloc() ) ;
      for( size_t i=0 ; i<str.size() ; ++i )
      stm << ctfacet.narrow( str[i], 0 ) ;
      return stm.str() ;
}


class InStream : public C7ZipInStream
{
private:
	GoInterface r;
public:
	InStream(GoInterface r) {
		this->r = r;
	}

	~InStream() {
	}

	wstring GetExt() const {
		//std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> ucs2conv;
		auto gostr = go_ReaderExt(r);
		std::string s((const char*)gostr.p, (size_t)gostr.n);
		return widen(s);
	}

	int Read(void *data, unsigned int size, unsigned int *n) {
		return go_ReaderRead(r, data, size, n);
	}

	int Seek(__int64 offset, unsigned int seekOrigin, unsigned __int64 *newPosition) {
		return go_ReaderSeek(r, (GoInt64)offset, (GoInt)seekOrigin, (GoInt64*)newPosition);
	}

	int GetSize(unsigned __int64 * size) {
		if (size) {
			return go_ReaderSize(r, (GoInt64*)size);
		}
		return 0;
	}
};

class OutStream : public C7ZipOutStream
{
private:
	GoInterface w;
public:
	OutStream(GoInterface w) {
		this->w = w;
	}

	int GetSize() const {
		return go_WriterSize(w);
	}

	int Write(const void *data, unsigned int size, unsigned int *n) {
		return go_WriterWrite(w, (void*)data, size, n);
	}

	int Seek(__int64 offset, unsigned int seekOrigin, unsigned __int64 *newPosition) {
		return go_WriterSeek(w, (GoInt64)offset, (GoInt)seekOrigin, (GoInt64*)newPosition);
	}

	int SetSize(unsigned __int64 size) {
		return go_WriterSetSize(w, (GoInt64)size);
	}
};

bool lib7zip_open_archive(void *rp, c7z_Library *l, c7z_InStream **stream, c7z_Archive **a) {
	GoInterface r = *static_cast<GoInterface*>(rp);
	auto instream = new InStream(r);
	*stream = static_cast<c7z_InStream*>(instream);
	return c7zLib_OpenArchive(l, *stream, a);
}

bool lib7zip_close_archive(c7z_InStream *stream, c7z_Archive *a) {
	c7zArc_Close(a);
	delete static_cast<OutStream*>(stream);
}

bool lib7zip_archive_extract(c7z_Archive *a, unsigned int i, void *wp) {
	GoInterface w = *static_cast<GoInterface*>(wp);
	OutStream outstream(w);
	auto stream = static_cast<c7z_OutStream*>(&outstream);
	return c7zArc_ExtractByIndex(a, i, stream);
}

bool lib7zip_archive_extract_password(c7z_Archive *a, unsigned int i, void *wp, wchar_t *ws) {
	GoInterface w = *static_cast<GoInterface*>(wp);
	OutStream outstream(w);
	auto stream = static_cast<c7z_OutStream*>(&outstream);
	return c7zArc_ExtractByIndexPW(a, i, stream, ws);
}

bool lib7zip_item_extract(c7z_Archive *a, c7z_ArchiveItem *i, void *wp) {
	GoInterface w = *static_cast<GoInterface*>(wp);
	OutStream outstream(w);
	auto stream = static_cast<c7z_OutStream*>(&outstream);
	return c7zArc_ExtractByItem(a, i, stream);
}

wchar_t * lib7zip_wchar_array(wchar_t **wss, int i) {
	return wss[i];
}
