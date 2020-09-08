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
typedef interfaces::time_group<point3> tp3;
int main (int argc, char ** argv) {
    service::core::dataserver::initialize("localhost",62014);
    habits::utils::setup_omp();
    representations::trajectory3d::set_interpolation_strategy(representations::trajectory3d::interpolation_style::CUBIC_SPLINE);
    service::core::dataserver::initialize("localhost",62014);
    service::parameters::config::set_project("hap");
    habits::load_dataset("bolts","subject_1");
    auto trajectory = (*habits::active_dataset().begin()).as<trajectory3d>();
    // create a group
    auto group = trajectory_cluster3d (); auto gt = trajectory_cluster3d (); gt.insert("bolt_placement_dataset/subject_1:/trajectories/ground_truth",trajectory);
    group.move_insert("bolt_placement_dataset/subject_1:/trajectories/stream_test",trajectory3d());
    trajectory3d & stream = group.at("bolt_placement_dataset/subject_1:/trajectories/stream_test");
    // create segmentation
    habits::segmentation::state_based_segmentation segmentation (group), ground_truth(gt);
    SLOG(debug) << ground_truth.at("bolt_placement_dataset/subject_1:/trajectories/ground_truth");
    // plot and time
    gppe::timer t;
    auto fig = service::vtkhl::plot3::figure("live plot");
    fig->autozoom(true);
    // okay, lets stream this trajectory so we can test things...
    int i = 0;
    for (const auto & point : trajectory){
        stream << point;
        if (i++ == 2) {
            service::vtkhl::plot3::plot(stream);
            service::vtkhl::plot3::show(false);
//            service::vtkhl::plot2::plot(stream);
//            service::vtkhl::plot2::show(false);
        }
//        usleep(5000);
    }
    SLOG(debug) << "elapsed = " << t.elapsed() << "s";
    SLOG(debug) << stream;
    for (const auto & segment : segmentation) {
        SLOG(debug) << segment;
    }
    service::vtkhl::plot3::figure("segmentation");
    service::vtkhl::plot3::plot(segmentation);
    fig->autozoom(false);
//    service::vtkhl::plot3::figure("final figure");
//    service::vtkhl::plot3::plot(stream);
    service::vtkhl::plot3::show(true);
//    habits::segmentation::state_based_segmentation segmentation (habits::active_dataset());
//    habits::data::activity_segmentation by_activities(segmentation);
////    // plot the derivatives
//    service::vtkhl::plot3::plot(segmentation);
//    service::vtkhl::plot3::show(true);
}
