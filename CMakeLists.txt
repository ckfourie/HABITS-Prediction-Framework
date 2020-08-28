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
add_definitions(-DHAP_PROJECT_NAME="${HABITS_FRAMEWORK_DATASERVER_PROJECT_NAME}")
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

#    include/hap/data/maps/event_pair_mapping.h
#    src/hap/data/maps/event_pair_mapping.cpp
#    include/hap/data/maps/goal_region_mapping.h
#    src/hap/data/maps/goal_region_mapping.cpp
#    include/hap/data/maps/dropout_region_mapping.h
#    src/hap/data/maps/dropout_region_mapping.cpp
#    include/hap/data/interfaces/datasource.h
#    src/hap/data/interfaces/datasource.cpp
#    include/hap/data/interfaces/source.h
#    src/hap/data/interfaces/source.cpp
#    include/hap/data/interfaces/datatype.h
#    src/hap/data/interfaces/datatype.cpp
#    include/hap/data/sources/filedata.h
#    src/hap/data/sources/filedata.cpp
#    include/hap/data/types/trajectory.h
#    src/hap/data/types/trajectory.cpp
#    include/hap/data/types/mean_trajectory.h
#    src/hap/data/types/mean_trajectory.cpp
#    include/hap/data/types/partial_trajectory.h
#    src/hap/data/types/partial_trajectory.cpp
#    include/hap/data/types/event.h
#    src/hap/data/types/event.cpp
#    include/hap/data/types/entry_event.h
#    src/hap/data/types/entry_event.cpp
#    include/hap/data/types/exit_event.h
#    src/hap/data/types/exit_event.cpp
#    include/hap/data/types/dropout_start_event.h
#    include/hap/data/types/dropout_end_event.h
#    src/hap/data/types/dropout_start_event.cpp
#    src/hap/data/types/dropout_end_event.cpp
#    include/hap/analysis/data/dropout_count.h
#    src/hap/analysis/data/dropout_count.cpp
#    include/hap/filters/interfaces/filter.h
#    src/hap/filters/interfaces/filter.cpp
#    include/hap/filters/event_filter.h
#    include/hap/filters/interfaces/vector_filter.h
#    src/hap/filters/interfaces/vector_filter.cpp
#    include/hap/filters/interfaces/multisource_filter.h
#    src/hap/filters/interfaces/multisource_filter.cpp
#    include/hap/filters/dropout_filter.h
#    src/hap/filters/dropout_filter.cpp
#    include/hap/filters/mean_filter.h
#    src/hap/filters/mean_filter.cpp
#    include/hap/filters/offset_filter.h
#    src/hap/filters/offset_filter.cpp
#    include/hap/filters/region_event_filter.h
#    src/hap/filters/region_event_filter.cpp
#    include/hap/convertors/split_by_interval.h
#    src/hap/convertors/split_by_interval.cpp
#    include/hap/convertors/event_to_interval.h
#    src/hap/convertors/event_to_interval.cpp
#    include/hap/convertors/merge.h
#    src/hap/convertors/merge.cpp
#    include/hap/convertors/to_activity_group.h
#    src/hap/convertors/to_activity_group.cpp
#    include/hap/core/settings.h
#    src/hap/core/settings.cpp
#    include/hap/core/sort.h
#    src/hap/core/sort.cpp
#    include/hap/core/print.h
#    src/hap/core/print.cpp
#    include/hap/events/region_detector.h
#    src/hap/events/region_detector.cpp
#    include/hap/algorithm/cluster_trajectories.h
#    src/hap/algorithm/cluster_trajectories.cpp
#    include/hap/predictors/interfaces/predictor.h
#    src/hap/predictors/interfaces/predictor.cpp
#    include/hap/predictors/pdtw_predictor.h
#    src/hap/predictors/pdtw_predictor.cpp
#    include/hap/predictors/odtw_predictor.h
#    src/hap/predictors/odtw_predictor.cpp
#    include/hap/algorithm/generate_predictors.h
#    include/hap/hl/datafile_to_predictors.h
#    include/hap/hl/split_trajectories.h
#    src/hap/hl/split_trajectories.cpp
#    include/hap/analysis/data/markers_exist.h
#    src/hap/analysis/data/markers_exist.cpp
#    include/hap/analysis/prediction/likelihood_matrix.h
#    src/hap/analysis/prediction/likelihood_matrix.cpp
#    include/hap/analysis/prediction/get_predictor_index.h
#    src/hap/analysis/prediction/get_predictor_index.cpp
#    include/hap/analysis/prediction/score_alignment_performance.h
#    src/hap/analysis/prediction/score_alignment_performance.cpp
#    include/hap/analysis/prediction/score_matrix.h
#    src/hap/analysis/prediction/score_matrix.cpp
#    include/hap/analysis/prediction/ground_truth_event_vector.h
#    src/hap/analysis/prediction/ground_truth_event_vector.cpp
#    include/hap/analysis/prediction/prediction_event_vector.h
#    src/hap/analysis/prediction/prediction_event_vector.cpp
#    include/hap/data/sources/type_source.h
#    src/hap/data/sources/type_source.cpp
#    include/hap/data/interfaces/vector_source.h
#    include/hap/data/sources/vector_type_source.h
#    src/hap/data/sources/vector_type_source.cpp
#    include/hap/analysis/prediction/likelihood_alignment_performance.h
#    src/hap/analysis/prediction/likelihood_alignment_performance.cpp
#    include/hap/analysis/prediction/weighted_likelihood_matrix.h
#    include/hap/analysis/prediction/weighted_likelihood_alignment_performance.h
#    src/hap/analysis/prediction/weighted_likelihood_matrix.cpp
#    src/hap/analysis/prediction/weighted_likelihood_alignment_performance.cpp
#    include/hap/core/options_events.h
#    src/hap/core/options_events.cpp
#    include/hap/core/options_activities.h
#    src/hap/core/options_activities.cpp
#    include/hap/predictors/predictor_map.h
#    src/hap/predictors/predictor_map.cpp
#    include/hap/algorithm/activity_durations.h
#    src/hap/algorithm/activity_durations.cpp
#    include/hap/data/types/event_sequence.h
#    src/hap/data/types/event_sequence.cpp
#    include/hap/hl/datafile_to_trajectories.h
#    src/hap/hl/datafile_to_trajectories.cpp
#    include/hap/predictors/longterm/gmm_temporo_semantic_predictor.h
#    src/hap/predictors/longterm/gmm_temporo_semantic_predictor.cpp
#    include/hap/analysis/data/durations_vector.h
#    src/hap/analysis/data/durations_vector.cpp
#    include/hap/analysis/prediction/temporal_error_statistics.h
#    src/hap/analysis/prediction/temporal_error_statistics.cpp
#    include/hap/predictors/joint_action_predictor.h
#    src/hap/predictors/joint_action_predictor.cpp
#    include/hap/predictors/interfaces/longterm_predictor.h
#    src/hap/predictors/interfaces/longterm_predictor.cpp
#    include/hap/predictors/longterm/composite_longterm_predictor.h
#    include/hap/predictors/interfaces/spatial_predictor.h
#    src/hap/predictors/interfaces/spatial_predictor.cpp
#    include/hap/predictors/bpta_predictor.h
#    src/hap/predictors/bpta_predictor.cpp
#    include/hap/data/maps/marginal_probability_map.h
#    src/hap/data/maps/marginal_probability_map.cpp
#    include/hap/data/types/start_event.h
#    src/hap/data/types/start_event.cpp
#    include/hap/data/types/end_event.h
#    src/hap/data/types/end_event.cpp
#    include/hap/core/combine_equivalent.h
#    src/hap/core/combine_equivalent.cpp
#    include/hap/analysis/prediction/joint_action_prediction_performance.h
#    src/hap/analysis/prediction/joint_action_prediction_performance.cpp
#    include/hap/data/sources/remote_filedata.h
#    src/hap/data/sources/remote_filedata.cpp
#    include/hap/data/interfaces/filesource.h
#    src/hap/data/interfaces/filesource.cpp
#    include/hap/data/sources/label_source.h
#    src/hap/data/sources/label_source.cpp
#    include/hap/analysis/prediction/markov_alignment_performance.h
#    src/hap/analysis/prediction/markov_alignment_performance.cpp
#    include/hap/analysis/prediction/markov_alignment_matrix.h
#    src/hap/analysis/prediction/markov_alignment_matrix.cpp
#    include/hap/analysis/prediction/predictors_temporal_performance.h
#    src/hap/analysis/prediction/predictors_temporal_performance.cpp
#    include/hap/analysis/prediction/get_predictor_posterior.h
#    src/hap/analysis/prediction/get_predictor_posterior.cpp
#    include/hap/analysis/prediction/short_term_activity_recognition.h
#    src/hap/analysis/prediction/short_term_activity_recognition.cpp
#    include/hap/analysis/prediction/short_term/evaluation_matrices.h
#    src/hap/analysis/prediction/short_term/evaluation_matrices.cpp
#    include/hap/analysis/prediction/short_term/results.h
#    src/hap/analysis/prediction/short_term/results.cpp
)
#if (HABITS_FRAMEWORK_BUILD_DISPLAY)
#    list(APPEND HABITS_FRAMEWORK_SOURCES
#        include/hap/visual/view_trajectories.h
#        src/hap/visual/view_trajectories.cpp
#        include/hap/visual/view_alignment.h
#        src/hap/visual/view_alignment.cpp
#        include/hap/visual/view_mean_trajectories.h
#        src/hap/visual/view_mean_trajectories.cpp
#    )
#endif()
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
endif()
################################### Install ####################################
install(DIRECTORY include/ DESTINATION include/)
install(TARGETS HABITS_FRAMEWORK DESTINATION lib/)
install(TARGETS ${HAP_BINARY_INSTALL_TARGETS} DESTINATION bin/)
################################ Print Status ##################################
message(STATUS "Created Targets for Library ${PROJECT_NAME}")
################################################################################
