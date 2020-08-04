include(FindPackageHandleStandardArgs)
if (WIN32)
    message(fatal "Need to update file paths to reflect MS Windows. ")
endif ()
if (${CMAKE_HOST_UNIX})
    find_path( LibCluster_INCLUDE_DIR
            NAMES
            libcluster.h
            PATHS
            /usr/include/libcluster
            /usr/local/include/libcluster
            $ENV{LIBCLUSTER_LOCATION}/include
            NO_DEFAULT_PATH
            DOC "The directory where LibCluster headers reside"
            )
    find_library( LibCluster_LIBRARY
            NAMES
                libcluster.so
                libcluster.a
            PATHS
                /usr/local/lib/
                /usr/lib/
                $ENV{LIBCLUSTER_LOCATION}/lib
                NO_DEFAULT_PATH
            DOC "The installed owlsock library."
            )
endif ()
find_package_handle_standard_args(LibCluster
        FOUND_VAR LibCluster_FOUND
        REQUIRED_VARS LibCluster_INCLUDE_DIR
        )
if(LibCluster_FOUND)
    set(LibCluster_INCLUDE_DIRS ${LibCluster_INCLUDE_DIR})
    set(LibCluster_LIBRARIES ${LibCluster_LIBRARY})
endif()
mark_as_advanced( LibCluster_INCLUDE_DIR LibCluster_LIBRARIES)
