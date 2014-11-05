project(petest)

cmake_minimum_required( VERSION 2.6 )

# check 64 bit
if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set( ARCH x64 )
else()
    set( ARCH x86 )
endif()

if( NOT CMAKE_BUILD_TYPE )
	set(CMAKE_BUILD_TYPE Release)
endif()

if (CMAKE_BUILD_TYPE STREQUAL Debug)
	set( BUILDTYPE debug)
else()
	set( BUILDTYPE release)
endif()
message("*** ${ARCH}/${BUILDTYPE} ***")

# boost libraries
set (BOOST_LIBS system filesystem date_time thread)

if( NOT Boost_DIR )
    if( EXISTS $ENV{BOOST_DIR} )
        set(Boost_DIR $ENV{BOOST_DIR})
        #message("will use boost path in env $ENV{BOOST_DIR}")
    else()
        #message("not set boost path")
    endif()
endif()
if( Boost_DIR )
	set(INCLUDES ${INCLUDES} ${Boost_DIR}/include)
	#set(LIBPATHS ${LIBPATHS} ${Boost_DIR}/lib)
	set(LIBPATHS ${LIBPATHS} ${Boost_DIR}/lib/${ARCH}/${BUILDTYPE})
endif()

if( NOT WIN32 )
	foreach(F ${BOOST_LIBS})
		set(LIBS ${LIBS} boost_${F})
	endforeach()
endif()
#find_library(Boost_LIBRARIES NAMES boost_system ${Boost_DIR}/lib/${ARCH}/${BUILDTYPE})
#set(LIBS ${LIBS} ${Boost_DIR}/lib/${ARCH}/${BUILDTYPE}/boost_system)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

add_definitions( -DBOOST_ALL_DYN_LINK -std=c++11)

LINK_DIRECTORIES(${LIBPATHS})

message("- link libs: ${LIBS}")
message("- link dirs: ${LIBPATHS}")

#----------------------------------

if( NOT WIN32 )
	set(LIBS ${LIBS} -lrt)
endif()

set(INCLUDES ${INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR}/../src)

message("- INCLUDES dirs: ${INCLUDES}")

include_directories(${INCLUDES})

set( SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/FileTypes.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/JzIndexer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/test_Indexer.cpp"
)

add_executable( test_Indexer ${SRC} )
target_link_libraries(test_Indexer ${LIBS})

set( SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/test_ObjectInfo.cpp"
)

add_executable( test_ObjectInfo ${SRC} )
target_link_libraries(test_ObjectInfo ${LIBS})


set( SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/test_FiniteStateMachine.cpp"
)

add_executable( test_FiniteStateMachine ${SRC} )
target_link_libraries(test_FiniteStateMachine ${LIBS})

