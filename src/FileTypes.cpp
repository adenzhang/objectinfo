#include "FileTypes.h"

using namespace std;

#define ADD_FILETYPE(type) add(FileTypeInfo( _T(".")_T(#type), type))
namespace jz{

	FileTypeStore::FileTypeStore() : _dbName(_T("FileTypeDb.txt")) {
		init();
	}
	FileTypeStore::~FileTypeStore() {
		save();
	}
	FileTypeStore& FileTypeStore::getInstance() {
		static FileTypeStore __store;
		return __store;
	}
	void FileTypeStore::init() {
		load();
		//- add predefined type
		//add(FileTypeInfo(txt, _T(".txt")));
		ADD_FILETYPE(txt);
		ADD_FILETYPE(xml);
		ADD_FILETYPE(exe);
		ADD_FILETYPE(doc);
		ADD_FILETYPE(xls);
		ADD_FILETYPE(ppt);
	}
	int FileTypeStore::load() {
		Ifstream file(_dbName.c_str());
		if( !file.is_open() ) return false;
		FileTypeInfo filetype;
		int count = 0;
		const int MAX_LINE_LEN = 128;
		Char buf[MAX_LINE_LEN];
		while( file.getline(buf, MAX_LINE_LEN) ) {
			Stringstream ss(buf);
			ss >> filetype.type >> filetype.ext;
			add(filetype);
			count ++;
		}
		return count;
		
	}
	int FileTypeStore::save() {
		Ofstream file(_dbName.c_str());
		if( !file.is_open() ) return false;  
		for(FileTypeInfoList::iterator it=_db.begin(); it!=_db.end(); ++it) {
			file << it->type << _T(" ") << it->ext << endl;
		}
	}
	bool FileTypeStore::add(const FileTypeInfo& filetype){
		if( filetype.type == InvalidFileType || filetype.ext.empty() ) return false;
		//- remove conflicting type or ext
		for(FileTypeInfoList::iterator it=_db.begin(); it!=_db.end(); ) {
			FileTypeInfoList::iterator it0 = it;
			if(it->type == filetype.type) {
				it++;
				_db.erase(it0);
				continue;
			}
			if(0 == it->ext.compare(filetype.ext)) {
				it++;
				_db.erase(it0);
				continue;
			}
			it++;
		}

		//- append data
		_db.push_back(filetype);
		
		return true;
	}
	const FileTypeInfo* FileTypeStore::getInfo(FileType type) const {
		for(FileTypeInfoList::const_iterator it=_db.begin(); it!=_db.end(); ++it) {
			if(it->type == type)
				return &(*it);
		}
		return NULL;
	}
	const FileTypeInfo* FileTypeStore::getInfo(const String& ext) const {
		for(FileTypeInfoList::const_iterator it=_db.begin(); it!=_db.end(); ++it) {
			if(it->ext.compare(ext) == 0)
				return &(*it);
		}
		return NULL;
	}
	bool FileTypeStore::remove(FileType type) {
		for(FileTypeInfoList::iterator it=_db.begin(); it!=_db.end(); ++it) {
			if(it->type == type) {
				_db.erase(it);
				return true;
			}
		}
		return false;
	}
	void FileTypeStore::clear() {
		_db.clear();
	}
} // namespace jz