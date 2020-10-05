########################## Human Activity Prediction   ###########################
# See README.MD
# author: ckfourie
##################################################################################
cmake_minimum_required(VERSION 3.5)
project (HABITS_FRAMEWORK)
############################## Compiler Options ##################################
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -O3 -fPIC -fopenmp -funroll-loops")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
# Set to place executables in root build directory (for ease of testing)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
################################### Options ######################################
option (HABITS_FRAMEWORK_DEVELOP         "Use development versions of included libraries (not installed)" ON)
option (HABITS_FRAMEWORK_DOCUMENTATION   "Generate Doxygen Documentation" OFF)
option (HABITS_FRAMEWORK_TESTS           "Build tests" ON)
option (HABITS_FRAMEWORK_BUILD_UTILITIES "Build utility functions for activity recognition" ON)
option (HABITS_FRAMEWORK_BUILD_DISPLAY   "Build vizualizations (required for utilities)" ON)
option (HABITS_FRAMEWORK_BUILD_MATLAB    "Build matlab functions" OFF)
set    (HABITS_FRAMEWORK_DATASERVER_PROJECT_NAME "hap" CACHE STRING "The project name for parameters stored on the server")
##################################################################################
add_definitions(-DPROJECT_NAME="${HABITS_FRAMEWORK_DATASERVER_PROJECT_NAME}")
##################################################################################
# Empty variables for compilation files.
set (HABITS_FRAMEWORK_SOURCES)
set (HABITS_FRAMEWORK_LIBS)
# Prepend local modules to module path.
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules/" ${CMAKE_MODULE_PATH})
## Find Eigen
find_package(Eigen3 REQUIRED)
include_directories( ${EIGEN3_INCLUDE_DIR} )
## Find MATLAB
if (HABITS_FRAMEWORK_BUILD_MATLAB)
    find_package(Matlab QUIET)
    if (NOT ${Matlab_FOUND})
        set(Matlab_ROOT_DIR "/opt/matlab/R2018a/")
        find_package(Matlab QUIET REQUIRED)
    endif()
endif()
## Build Vizualization Options
if (HABITS_FRAMEWORK_BUILD_DISPLAY)
    add_definitions(-DBUILD_DISPLAY)
endif()
# Find Development Libraries
#set(HABITS_FRAMEWORK_MODULE_LIST KernelMethods SimpleLogger ProbabilityPrimitives PhaseSpace DataFiles GPPE EigenHL ActivityRecognition)
set(HABITS_FRAMEWORK_MODULE_LIST SimpleLogger GPPE EigenHL DataFiles DataServer Representations Services ProbabilityPrimitives KernelMethods PhaseSpace)
if (HABITS_FRAMEWORK_BUILD_DISPLAY)
    list(APPEND HABITS_FRAMEWORK_MODULE_LIST VTKHL)
endif()
if (HABITS_FRAMEWORK_DEVELOP)
    include(${PROJECT_SOURCE_DIR}/cmake/find_dev_package.cmake)
    foreach(MODULE ${HABITS_FRAMEWORK_MODULE_LIST})
        find_dev_package(${MODULE} $ENV{HOME}/Work/Libraries/Lib${MODULE})
        include_directories(${${MODULE}_INCLUDE_DIRS})
        list(APPEND HABITS_FRAMEWORK_LIBS ${${MODULE}_LIBRARIES})
    endforeach()
else()
    #message(FATAL_ERROR "todo: implement FindXXX.cmake for development components")
endif()
# Include local includes
include_directories(include/)
list(APPEND HABITS_FRAMEWORK_SOURCES
    include/habits/core/options.h
    src/habits/core/options.cpp
    include/habits/datasets.h
    src/habits/datasets.cpp
    include/habits/segmentation/interfaces/segmentation.h
    src/habits/segmentation/interfaces/segmentation.cpp
    include/habits/segmentation/state_based_segmentation.h
    src/habits/segmentation/state_based_segmentation.cpp
    include/habits/utils/dataset_name_structure.h
    src/habits/utils/dataset_name_structure.cpp
    include/habits/segmentation/detectors/state_based_detector.h
    src/habits/segmentation/detectors/state_based_detector.cpp
    include/habits/segmentation/interfaces/detector.h
    include/habits/utils/setup_omp.h
    include/habits/data/activity_segmentation.h
    src/habits/data/activity_segmentation.cpp
    include/habits/clustering/cluster_trajectories.h
    src/habits/clustering/cluster_trajectories.cpp
    include/habits/predictors/interfaces/predictor.h
    src/habits/predictors/interfaces/predictor.cpp
    include/habits/clustering/algorithm/kernel.h
    src/habits/clustering/algorithm/kernel.cpp
    include/habits/clustering/algorithm/em.h
    src/habits/clustering/algorithm/em.cpp
    include/habits/clustering/algorithm/agl.h
    src/habits/clustering/algorithm/agl.cpp
    include/habits/predictors/bestpta_predictor.h
    src/habits/predictors/bestpta_predictor.cpp
    include/habits/predictors/learn_predictors.h
    src/habits/predictors/learn_predictors.cpp
    include/habits/predictors/group_predictors.h
    src/habits/predictors/group_predictors.cpp
    include/habits/predictors/sequence_predictor.h
    src/habits/predictors/sequence_predictor.cpp
    include/habits/results/predictors.h
    src/habits/results/predictors.cpp
)
# This should always come after including submodules, otherwise the definition will be overwritten.
add_definitions(-DLOG_MODULE="HABITS^2")
# Create the library
add_library(HABITS_FRAMEWORK SHARED ${HABITS_FRAMEWORK_SOURCES})
target_link_libraries(HABITS_FRAMEWORK ${HABITS_FRAMEWORK_LIBS})
# Create the utility function
set(HAP_BINARY_INSTALL_TARGETS)
add_executable(hap_playground tests/playground.cpp)
target_link_libraries(hap_playground  HABITS_FRAMEWORK ${HABITS_FRAMEWORK_LIBS} )
if (HABITS_FRAMEWORK_BUILD_UTILITIES)
    add_executable(habits_plot utils/habits_plot.cpp)
    target_link_libraries(habits_plot HABITS_FRAMEWORK ${HABITS_FRAMEWORK_LIBS} )
    add_executable(habits_live_segmentation utils/habits_live_segmentation.cpp)
    target_link_libraries(habits_live_segmentation HABITS_FRAMEWORK ${HABITS_FRAMEWORK_LIBS})
    add_executable(habits_goal_positions utils/habits_goal_positions.cpp)
    target_link_libraries(habits_goal_positions HABITS_FRAMEWORK ${HABITS_FRAMEWORK_LIBS})
    add_executable(habits_predictor_performance_statistics utils/habits_predictor_performance_statistics.cpp)
    target_link_libraries(habits_predictor_performance_statistics HABITS_FRAMEWORK ${HABITS_FRAMEWORK_LIBS})
endif()
if (HABITS_FRAMEWORK_TESTS)
    add_executable(test_habits_live_segmentation tests/test_live_segmentation.cpp)
    target_link_libraries(test_habits_live_segmentation HABITS_FRAMEWORK ${HABITS_FRAMEWORK_LIBS})
endif()
################################### Install ####################################
install(DIRECTORY include/ DESTINATION include/)
install(TARGETS HABITS_FRAMEWORK DESTINATION lib/)
install(TARGETS ${HAP_BINARY_INSTALL_TARGETS} DESTINATION bin/)
################################ Print Status ##################################
message(STATUS "Created Targets for Library ${PROJECT_NAME}")
################################################################################