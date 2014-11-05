#pragma once
#include <string>
#include <list>
#include <exception>
#include <boost/unordered_map.hpp>


namespace jz {


	typedef std::runtime_error MyException;

	struct ObjectStore {
		template<typename T> class ClassInfo;
	private:
		ObjectStore() {}
	public:
		static ObjectStore& getInstance() {
			static ObjectStore __store;
			return __store;
		}
		//-- object queries  ------------
		template<typename T>
		int isClassRegistered() {
			std::string classname(GetClassName<T>());
			return _classInfos.find(classname) == _classInfos.end()? false: true;
		}

		struct QueryResult {
			int  stackObjectCount;   // number of objects on stack
			int  heapObjectCount;      // number of objects on heap
		};

		// return false if class is not registered.
		template<typename T>
		bool queryObjects(QueryResult& result) {
			std::string classname(GetClassName<T>());
			ClassInfoMap::iterator it = _classInfos.find(classname);
			if( it == _classInfos.end() )
				return false;
			ClassInfo<T> *info = (ClassInfo<T> *) it->second;
			result.stackObjectCount = info->stackObjects.size();
			result.heapObjectCount = info->heapObjects.size();
			return true;
		}

		template<typename T>
		static const char * GetClassName() {
			static const char *name = typeid((T*)NULL).name();
			return name;
		}
	protected:
		typedef boost::unordered_map<std::string, void *> ClassInfoMap;
		ClassInfoMap _classInfos;

		// throws exception if classname conflicts.
		template<typename T>
		void registerClass(ClassInfo<T> *classinfo) {
			std::string classname(GetClassName<T>());
			ClassInfoMap::iterator it = _classInfos.find(classname);
			if( it != _classInfos.end() ) {
				classname += " conflicts";
				throw MyException(classname.c_str());
			}
			_classInfos[classname] = classinfo;
		}

	public:

		// singleton class ClassInfo monitors object creation/deletion on heap and stack.

		template<typename Class>
		struct ClassInfo {
			typedef ClassInfo<Class> ThisType;
		private:
			ClassInfo() {
				ObjectStore::getInstance().registerClass<Class>(this);
			}
		public:
			const static int ClassSize = sizeof(Class);
			enum {ARRAY_HEADER_SIZE = sizeof(int)};    // when objects are allocated with new[], a header will be allocated.
			static const char * CLASSANME;
			struct MemInfo{
				void *mem;
				size_t index; // for object arrays, indicate the current object
				MemInfo(void *mem=NULL, size_t n=0): mem(mem), index(n){}
				MemInfo(const MemInfo& a): mem(a.mem), index(a.index) {}

				bool operator==(const MemInfo& a) {
					return mem == a.mem;
				}
				bool operator!=(const MemInfo& a) {
					return mem != a.mem;
				}
			};

			typedef std::list<MemInfo> MemInfoList;
			MemInfoList stackObjects;
			MemInfoList heapObjects;

			MemInfo currObj; // current constructing or destructing object. buggy in multi-threaded env.
			int bufsize;

			static ClassInfo& getInstance() {
				static ClassInfo __obj;
				return __obj;
			}
			void allocated(void *mem, size_t n) {
				assert(currObj == MemInfo());
				uintptr_t pMem = (uintptr_t)mem;
				if( n > ClassSize ) {
					pMem += ARRAY_HEADER_SIZE;
					n -= ARRAY_HEADER_SIZE;
				}
				currObj = MemInfo((void*)pMem);
				bufsize = n;
			}
			void constructed(void *mem) {
				//
				if( ((char*)currObj.mem) + ClassSize*currObj.index == ((char*)mem) ) { // on heap
					//std::cout << GetClassName<Class>() << " constructed on heap idx: " << currObj.index << " at:" <<  uintptr_t(mem) << std::endl;
					heapObjects.push_back(currObj);
					currObj.index ++;
					if( ((char*)currObj.mem) + bufsize <= ((char*)currObj.mem) + ClassSize*currObj.index ) { // the last elem of array has been stored.
						currObj = MemInfo();
						bufsize = 0;
					}
				}else if( currObj != MemInfo() ) {
					throw MyException("mem constructing error!");
				}else{  // on stack
					stackObjects.push_back(MemInfo(mem));
					//std::cout << GetClassName<Class>() << " constructed on stack idx: " << currObj.index << " at:" <<  uintptr_t(mem) << std::endl;
				}
			}
			void deleted(void *mem) {
			}
			void destructed(void *mem) {
				assert(currObj == MemInfo());
				currObj = MemInfo(mem);
				typename MemInfoList::iterator it = std::find(stackObjects.begin(), stackObjects.end(), currObj);
				if( it == stackObjects.end() ) {
					it = std::find(heapObjects.begin(), heapObjects.end(), currObj);
					if( it == heapObjects.end() ) {
						throw MyException("mem destructing error!");
					}else {
						//std::cout << GetClassName<Class>() << " destructed on heap idx: " << it->index << " at:" <<  uintptr_t(mem) << std::endl;
						heapObjects.erase(it);
					}
				}else{
					//std::cout << GetClassName<Class>() << " destructed on stack idx: " << it->index << " at:" <<  uintptr_t(mem) << std::endl;
					stackObjects.erase(it);
				}
				currObj = MemInfo();
			}
		};
	}; // ObjectStore



	// BaseClass new operator and default constuctor work together to detect object create on heap and stack;
	// virtual destructor detect the object destroy.
	// if placeholder new/delete is used, they alse should be overrided.
	template<typename T>
	struct BaseClass {
		typedef ObjectStore::ClassInfo<T> MyClassInfo;
		BaseClass() {
			MyClassInfo::getInstance().constructed(this);
			//cout << "BaseClass constructed " << uintptr_t(this) << endl;
		}

		void* operator new(size_t n) {
			void *mem = ::operator new(n); 
			MyClassInfo::getInstance().allocated(mem, n);
			return mem;
		}
		void* operator new[](size_t n) {
			void *mem = ::operator new[](n); 
			MyClassInfo::getInstance().allocated(mem, n);
			return mem;
		}
		virtual ~BaseClass() {
			MyClassInfo::getInstance().destructed(this);
		}
	};

}

