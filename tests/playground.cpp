#include <simple-logger/simple-logger.h>
#include <service/core/dataserver.h>
#include <service/datasets.h>
#include <service/parameters.h>
#include <habits/datasets.h>
#include <habits/core/options.h>
//#include <service/vtkhl/plot3.h>
#include <service/vtkhl/plot2.h>
#include <representations/trajectory.h>
#include <representations/interfaces/semantic.h>
#include <representations/interfaces/segmentation.h>
#include <habits/utils/dataset_name_structure.h>
#include <habits/utils/setup_omp.h>
#include <habits/segmentation/state_based_segmentation.h>
#include <habits/data/activity_segmentation.h>
#include <habits/clustering/cluster_trajectories.h>
#include <habits/predictors/learn_predictors.h>
#include <habits/predictors/bestpta_predictor.h>
#include <gppe/python.h>
#include <cctype>
#include <gppe/string_manipulations.h>
using namespace representations;
int main (int argc, char ** argv) {
    gppe::timer t;
    Py_Initialize();
    service::core::dataserver::initialize("localhost",62014);
//    habits::utils::setup_omp();
//    omp_set_num_threads(8);
    representations::trajectory3d::set_interpolation_strategy(representations::trajectory3d::interpolation_style::CUBIC_SPLINE);
    service::parameters::config::set_project("hap");
    habits::load_dataset("bolts","subject_1");
    habits::segmentation::state_based_segmentation segmentation (habits::active_dataset());
    habits::data::activity_segmentation activity_group (segmentation);
    habits::clustering::cluster_trajectories cluster (activity_group);
    auto predictors = habits::predictors::learn_predictors(cluster,activity_group,habits::predictors::bestpta_predictor());
    SLOG(info) << "trained and completed in " << t.elapsed();
}
