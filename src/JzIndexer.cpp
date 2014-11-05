#include "JzIndexer.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include <algorithm>

using namespace std;
using namespace boost::filesystem;

namespace jz{
	Indexer::Indexer() 
		: _supportedTypes(FileTypeStore::getInstance().getAll()) 	{ 
			resetMap();
	}
	Indexer::Indexer(const FileTypeInfoList& supportedTypes)  
		: _supportedTypes(supportedTypes) {
			resetMap();
	}

	void Indexer::resetMap() {
		_indexedFiles.clear();
		for(FileTypeInfoList::const_iterator it=_supportedTypes.begin(); it!=_supportedTypes.end(); ++it) {
			_indexedFiles[*it]; // init all entries
		}
	}
	int Indexer::handlefile(boost::filesystem::path& p) {
		int n = 0;
		String ext = p.extension().c_str();
		//ext = transform(ext.begin(), ext.end(), std::tolower());
		IndexedFileMap::iterator it = _indexedFiles.find(ext);
		if( it != _indexedFiles.end()) {
			path abspath = absolute(p);
			it->second.push_back(FileDef(abspath.c_str(), file_size(p), it->first.type));
			n = 1;
		}
		return n;
	}
	int Indexer::addDirectory(const String& dir, int dirlevel) {
		int n = 0;
		if( dirlevel == 0 ) return n;
		path p(dir);
		try{
			if( exists(p) ) {
				if( is_regular_file(p) ) {
					return handlefile(p);
				}else if( is_directory(p) ) {
					for(directory_iterator it(p); it!= directory_iterator(); ++it) {
						path subpath(it->path());
						if( is_regular_file(subpath) )
							n += handlefile(subpath);
						else
							n += addDirectory(it->path().c_str(), dirlevel-1);
					}
				}

			}
		}catch(const filesystem_error& e) {
			cout << e.what();
		}
		return n;
	}

} // namespace jz