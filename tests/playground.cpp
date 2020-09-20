#include <simple-logger/simple-logger.h>
#include <service/core/dataserver.h>
#include <service/datasets.h>
#include <service/parameters.h>
#include <habits/datasets.h>
#include <habits/core/options.h>
#include <service/vtkhl/plot3.h>
#include <service/vtkhl/plot2.h>
#include <representations/trajectory.h>
#include <representations/interfaces/semantic.h>
#include <representations/interfaces/segmentation.h>
#include <habits/utils/dataset_name_structure.h>
#include <habits/utils/setup_omp.h>
#include <habits/segmentation/state_based_segmentation.h>
#include <habits/data/activity_segmentation.h>
#include <cctype>
#include <gppe/string_manipulations.h>
using namespace representations;
int main (int argc, char ** argv) {
    service::core::dataserver::initialize("localhost",62014);
    habits::utils::setup_omp();
    representations::trajectory3d::set_interpolation_strategy(representations::trajectory3d::interpolation_style::CUBIC_SPLINE);
    service::parameters::config::set_project("hap");
    habits::load_dataset("bolts","subject_1");
    auto trajectory_a = habits::active_dataset().at("bolt_placement_dataset/subject_1:/trajectories/run2").as<trajectory3d>();
    auto trajectory_b = habits::active_dataset().at("bolt_placement_dataset/subject_1:/trajectories/run4").as<trajectory3d>();
    SLOG(debug) << "dtw distance of a -> b = " << trajectory_a.distance(trajectory_b);
    SLOG(debug) << "normalized dtw distance of a -> b = " << trajectory_a.distance(trajectory_b)/trajectory_a.size();
    SLOG(debug) << "dtw distance of a -> a = " << trajectory_a.distance(trajectory_a);

}
