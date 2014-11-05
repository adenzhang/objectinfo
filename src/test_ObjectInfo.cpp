#include <assert.h>
#include <iostream>
#include "ObjectInfo.h"


namespace jz{
	struct A : BaseClass<A> {
		A() {
			std::cout << "A constructed " << uintptr_t(this) << std::endl;
		}
	};
	struct B {
		int id;
	};
	template<typename A>
	void printInfo() {
		ObjectStore& store = ObjectStore::getInstance();
		ObjectStore::QueryResult objInfo;
		if( false == store.queryObjects<A>(objInfo) ) {
			std::cout << ObjectStore::GetClassName<A>() << " is not registered!" << std::endl;
		}else {
			std::cout << ObjectStore::GetClassName<A>() << " heapObjects:" << objInfo.heapObjectCount << " stackObjects:"  << objInfo.stackObjectCount << std::endl;
		}
	}
}
void test_ObjectInfo() {
	using namespace jz;
	using namespace std;
	A a[2];
	cout << "----on heap" << endl;
	A *pA = new A[3];
	B b;
	printInfo<A>();
	printInfo<B>();
}
int main(int argc, char* argv[])
{
	test_ObjectInfo();
	return 0;
}

