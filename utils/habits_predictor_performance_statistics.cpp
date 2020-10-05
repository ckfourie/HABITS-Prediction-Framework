#include <gppe/python.h>
#include <habits/core/options.h>
#include <habits/predictors/bestpta_predictor.h>
#include <habits/results/predictors.h>
int main (int argc, char ** argv) {
    habits::core::options options ("habits_predictor_performance_statistics","calculate the performance statistics");
    options.enable_subject_regex();
    options.enable_trajectory_regex();
    options.parse_input(argc,argv);
    Py_Initialize();
    habits::results::calculate_predictor_results();
}