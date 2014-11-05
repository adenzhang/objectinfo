#ifndef INDEXER_H
#define INDEXER_H

enum FileType {	
				skip = 0,
				txt = 1, 
				xml = 2,
				exe = 3,
				doc = 4,
				xls = 5,
				ppt = 6, 
			};

struct FileDef {
	char		drive[3];
	char		name[256];     // avoid hard coded magic number. use PATH_MAX and FILENAME_MAX
	char		path[256];     // save as above
	char		ext[4];        // save as above
	int			size;		
	FileType	type;
};
// it'll be better to use wchar_t to store file names, because there might be unicode file names
void processDirectory(const char* name);
int* getFirstFile(FileType type, FileDef* fd);
int getNextFile(int* handle, FileDef* fd);
void listFiles(FileType type);

#endif //INDEXER_H
