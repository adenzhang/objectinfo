#pragma once
#include <string>
#include <cstdint>
#include <boost/unordered_map.hpp>
#include <boost/functional/hash.hpp>
#include <boost/filesystem.hpp>

#include "FileTypes.h"

namespace jz{

	//-- only ext is used to hash and compare
	inline bool operator== (const FileTypeInfo& one, const FileTypeInfo& another) {
		return one.ext.compare(another.ext) == 0;
	}

	inline size_t hash_value(const FileTypeInfo& a) {
		boost::hash<String> hasher1;
		return hasher1(a.ext);
	}
	struct IndexedFileIterator{
	protected:
		typedef FileDefList::iterator FileDefIterator;
		FileDefIterator _curr, _end;
		bool _found;
	public:
		IndexedFileIterator(FileDefIterator curr, FileDefIterator end) : _curr(curr), _end(end), _found(true) {}
		IndexedFileIterator(): _found(false) {}

		bool has_elem() {
			return _found && _curr != _end;
		}
		void operator++ () {
			if( has_elem() )
				_curr++;
		}
		FileDef* operator->() {
			return &(*_curr);
		}
		FileDef& operator*() {
			return (*_curr);
		}
	};
	class Indexer {
	protected:
		Indexer(const Indexer&){}
		Indexer& operator=(const Indexer&){}
	public:
		typedef boost::unordered_map<FileTypeInfo, FileDefList> IndexedFileMap; 

		// default constructor supports all types
		Indexer();
		Indexer(const FileTypeInfoList& supportedTypes);

		virtual ~Indexer(){}

		// @param dirlevel: 1, only add current directory; -1, add all subdir recursively.
		// @return number of files been indexed.
		int addDirectory(const String& dir, int dirlevel=1);

		IndexedFileIterator find(FileType type) {
			const FileTypeInfo* info = FileTypeStore::getInstance().getInfo(type);
			if( info == NULL )
				return IndexedFileIterator();
			IndexedFileMap::iterator it = _indexedFiles.find(*info);
			if( it == _indexedFiles.end() )
				return IndexedFileIterator();
			if( it->second.size() == 0 )
				return IndexedFileIterator();
			return IndexedFileIterator( it->second.begin(), it->second.end() );

		}

		void print(Ostream& os, FileType type) {
			for( IndexedFileIterator it = find(type); it.has_elem(); ++it ) {
				os << String(it->fullpath) << ", "  << it->size << std::endl;
			}
		}
	protected:
		const FileTypeInfoList _supportedTypes;
		IndexedFileMap _indexedFiles;

		void resetMap();

		int handlefile(boost::filesystem::path& p);
	};

}  // namespace jz
