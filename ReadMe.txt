
there are 3 projects included
All the 3 projects are built with boost library v1.54

Author: Jie Zhang, Aden
Date: 2014-11-01
Email: adenzh@gmail.com

building system: ubuntu 14.04 installed with boost_1.54 and cmake

if add definition _UNICODE in CMakeList.txt, Indexer project will be built with unicode.


=== Project Indexer ====
Functionalities: Index different file types. The file types are configurable in FileTypeDb.txt. 

Source Files: JzIndexer.h JzIndexer.cpp FileTypes.h FileTypes.cpp test_Indexer.cpp

Indexer.h and Indexer.cpp are the original copies including my comments.

Require: boost_filesystem

comment: FileTypeStore keeps all the file types that can be detected. the file types are configurable. Indexer selects some of the file types to index. When user wants to index a directory, she can specify the depth of subdirectory to index.

=== Project ObjectInfo ====
Functionalities: Trace object creation. It detects object creation on heap/stack by override operator new().

Soure Files: ObjectInfo.h test_ObjectInfo.cpp

comment: class ObjectStore keeps all the registed class info and is a public interface for queries. Internal class ClassInfo stores per-class info.


=== Project FiniteStateMachine ====
Functionalities:  Implement transaction state machine using boost::msm module. The transaction should complete within specific time. Otherwise, a timer thread will fire a SystemErrorEvent to force a transition.

Source Files: FiniteStateMachine.h test_FiniteStateMachine.cpp

Require: boost_thread boost_date_time

comment: Quantum Framework is a high performance state machine framework. Multi-threaed Actor pattern can be easily implemented with it. But it needs more work to design actor with boost::msm.

=End
