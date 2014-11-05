#pragma once
#include <string>
#include <cstdint>
#include <list>
#include <fstream>
#include <sstream>
#include <iostream>


#ifdef _UNICODE
#define _T(text) L##text
#else
#define _T(text) text

#endif

namespace jz{
#ifdef _UNICODE
	typedef wchar_t Char;

	typedef ::std::wstring String;
	typedef ::std::wifstream Ifstream;
	typedef ::std::wofstream Ofstream;
	typedef ::std::wstringstream Stringstream;
	typedef ::std::wostream Ostream;
	#define Cout ::std::wcout

#else
	typedef char Char;

	typedef ::std::string String;
	typedef ::std::ifstream Ifstream;
	typedef ::std::ofstream Ofstream;
	typedef ::std::stringstream Stringstream;
	typedef ::std::ostream Ostream;
	#define Cout std::cout 

#endif

	// pre-defined types.
	// file type is scalable and stored in database/file.
	enum FileType_ {	
				skip = 0,
				txt = 1, 
				xml = 2,
				exe = 3,
				doc = 4,
				xls = 5,
				ppt = 6, 
				EndFileType,
				InvalidFileType = -1
			};
	typedef int FileType;                              // file type should be integer and extendable

	struct FileTypeInfo{
		FileType   type;
		String     ext;
		FileTypeInfo(const String& ext=_T(""), FileType type=InvalidFileType): type(type), ext(ext){}

		unsigned int flag;
		enum ATTR{NEED_BACKUP=0x0001, NEED_SCAN=0x0002};
		void setFlag(ATTR bits) {
			flag |= bits;
		}
		void resetFlag(ATTR bits) {
			flag &= ~bits;
		}
		bool allFlag(ATTR bits) {
			return (flag & bits) == bits;
		}
		bool anyFlag(ATTR bits) {
			return (flag &bits) != 0;
		}
	};
	typedef ::std::list<FileTypeInfo> FileTypeInfoList;

	// store all supported file types, which can be dynamically added and removed.
	class FileTypeStore{
	private:
		// constructor auto init&save internal data
		FileTypeStore();
		// destructor auto save data
		virtual ~FileTypeStore();
		// @return number of record loaded
		int load();
		// @return number of record saved
		int save();
		std::string _dbName;
	public:

		static FileTypeStore& getInstance();

		const FileTypeInfo* getInfo(FileType) const;
		const FileTypeInfo* getInfo(const String& ext) const;

		FileTypeInfoList& getAll() {
			return _db;
		}

		void init();
		bool add(const FileTypeInfo& filetype);
		bool remove(FileType type);
		void clear();
	protected:
		FileTypeInfoList  _db;
	};

	struct FileDef {
		String         fullpath;   // absolute full path
		std::uintmax_t size;  // or boost::uintmax_t
		FileType       type;

		FileDef(const String& p, std::uintmax_t size, FileType type) : fullpath(p), size(size), type(type) {}
	};

	typedef std::list<FileDef> FileDefList;


}
