
#include <iostream>
#include <boost/filesystem/path.hpp>

#include "JzIndexer.h"

using namespace std;
//============== test indexer ============
void test_indexer()
{
	jz::FileTypeStore::getInstance().add(jz::FileTypeInfo(_T(".log"), 20));
	jz::Indexer ind;
	ind.addDirectory(_T("."), 2);
	ind.print(Cout, jz::txt);
	ind.print(Cout, 20);
}

int main(int argc, char* argv[])
{
	test_indexer();
	return 0;
}

