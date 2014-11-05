#include "stdafx.h"         // don't use features only for windows
#include "Indexer.h"
#include "stdlib.h"
#include "stdio.h"
#include "io.h"             // don't use features only for windows. For IO, use boost::filesystem
#include "string.h"


// if global variables are accessable only in this file, use static
// avoid using global variable, because they can not handle multipile instances and are not thread safe

FileDef	**fileIndex;        
int insertIndex;
int maxIndex;

void initIndexes() 
{
	insertIndex = 0;
	maxIndex = 1024;        // avoid using hard coded magic numbers
	fileIndex = new FileDef*[1024];  // it'll be better to use linked list
	memset(fileIndex, 0, maxIndex);
}

void processDirectory(const char* name)
{
	static bool init = true;
	if (init) {                   // unsafe in multi-threaded environment. protect this critical section or use boost::call_once or std::call_once if c++0x applies.
		init = false;
		initIndexes();
	}

	char search[256];
	strcpy(search, name);
	strcat(search, "\\*.*");

	_finddata_t fd;
	intptr_t h = _findfirst(search, &fd); // _findfisrt, _findnext are windows functions. use boost::filesystem::directory_iterator instead.
	if (h != -1) {
		do {
			FileDef* f = new FileDef;
			f->size = fd.size;

			char fname[256];
			strcpy(fname, name);
			strcat(fname,"\\");
			strcat(fname,fd.name);
			_splitpath( fname, f->drive, f->path, f->name, f->ext );  // use platform-independant boost::path to retrive extensions
			if ( strcmp(f->ext, ".txt") == 0) {
				f->type = txt;
			} else if ( strcmp(f->ext, ".xml") == 0) {
				f->type = xml;
			} else if ( strcmp(f->ext, ".exe") == 0) {
				f->type = exe;
			} else if ( strcmp(f->ext, ".doc") == 0) {
				f->type = doc;
			} else if ( strcmp(f->ext, ".xls") == 0) {
				f->type = xls;
			} else if ( strcmp(f->ext, ".ppt") == 0) {
				f->type = ppt;
			} else {
				f->type = skip;
			}
			if (f->type != skip) {
				fileIndex[insertIndex++] = f;
			}
		} while (_findnext(h, &fd) == 0);
		_findclose(h);

	}
}

// return int type instead of int* which tends to memory leak
int* getFirstFile(FileType type, FileDef* fd)
{
	int tmp = 0;
	while ( fileIndex[ tmp ] != 0 && fileIndex[ tmp ]->type != type) ++tmp;   // Bug! Not work when fileIndex is full.
	int* result = new int;
	if ( (*result = ( fileIndex [ tmp ] == 0) ? -1 : tmp + 1) == -1)
		return result;
	*fd = *fileIndex[ tmp ];

	return result;
}

int getNextFile(int* handle, FileDef* fd) 
{
	int tmp = *handle;
	while ( fileIndex[ tmp ] != 0 && fileIndex[ tmp ]->type != fd->type) ++tmp; // Bug! Not work when fileIndex is full.
	if (fileIndex[ tmp ] != 0) {
		*fd = *fileIndex[ tmp ];
		*handle = tmp + 1;
		return 0;
	}
	return -1;
}

void listFiles(FileType type) 
{
	printf("%30s%7s\r\n", "NAME","SIZE");
	FileDef fd;
	int* handle = getFirstFile(type, &fd);
	if ( *handle != -1) {
		do {
			printf("%30s%7i\r\n", fd.name,fd.size);
		} while ( getNextFile(handle, &fd) == 0);
	}
	// should delete handle
}

bool needsBackup(const FileDef* fd) 
{
	switch (fd->type) {
		case doc:
		case xls:
			return true;
	}
	return false;
}

bool needsScan(const FileDef* fd) 
{
	return (fd->type == exe);
}

// test function
void _testIndexer() {

	processDirectory("C:\test directory1" );
	processDirectory("C:\test directory2" );
	listFiles(exe);
	listFiles(txt);
	listFiles(xml);
}
