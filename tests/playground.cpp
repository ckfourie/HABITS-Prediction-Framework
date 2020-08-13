#include <simple-logger/simple-logger.h>
#include <service/core/dataserver.h>
#include <service/datasets.h>
#include <habits/datasets.h>
#include <habits/core/options.h>
#include <service/vtkhl/plot3.h>
#include <service/vtkhl/plot2.h>
#include <representations/trajectory.h>
#include <representations/interfaces/semantic.h>
#include <representations/interfaces/segmentation.h>
using namespace representations;
int main (int argc, char ** argv) {
    representations::trajectory3d::set_interpolation_strategy(representations::trajectory3d::interpolation_style::CUBIC_SPLINE);
    service::core::dataserver::initialize("localhost",62014);
    habits::load_dataset("bolts",".*");
    service::vtkhl::plot2::plot(habits::active_dataset());
    service::vtkhl::plot2::show(true);
//    service::core::dataserver::initialize("localhost",62014);
//    std::string dataset_name = "bolt_placement_dataset", subject_regex = "subject_1";
//    auto marker1 = service::datasets::read_representation<interfaces::homogeneous_cluster,trajectory3d>(dataset_name+"/" + subject_regex,"/trajectories/run17","marker1");
//    auto marker2 = service::datasets::read_representation<interfaces::homogeneous_cluster,trajectory3d>(dataset_name+"/" + subject_regex,"/trajectories/run17","marker2");
//    DEBUG_VALUE(marker1.size());
//    DEBUG_VALUE(marker2.size());
//    auto dropout_filter = [](const interfaces::time_group<point<3>> & point)->bool{
//        return point.element().x() == -10000 && point.element().y() == -10000 && point.element().z() == -10000;
//    };
//    double resample_frequency = 20.0;
//    for (auto it = marker1.begin(); it != marker1.end(); ++it) {
//        it.value().as<trajectory3d>().filter_elements(dropout_filter);
//    }
//    for (auto it = marker2.begin(); it != marker2.end(); ++it) {
//        it.value().as<trajectory3d>().filter_elements(dropout_filter);
//    }
//    SLOG(debug)<< "HERE!";
//    trajectory3d m1 = marker1.begin().value().as<trajectory3d>(),m2 = marker2.begin().value().as<trajectory3d>();
//    for (auto it = marker1.begin(); it != marker1.end(); ++it) {
//        SLOG(debug) << "PROCESSING MARKER1";
//        it.value().as<trajectory3d>().resample(resample_frequency); // lets try 20hz first
//    }
//    for (auto it = marker2.begin(); it != marker2.end(); ++it) {
//        SLOG(debug) << "PROCESSING MARKER2";
//        it.value().as<trajectory3d>().resample(resample_frequency); // lets try 20hz first
//    }
//    SLOG(debug) << "DONE";
//    trajectory3d s1 = marker1.begin().value().as<trajectory3d>(),s2 = marker2.begin().value().as<trajectory3d>();
//    // now plot original, and the filtered values over
//    service::vtkhl::plot2::plot(m1,m2);
//    service::vtkhl::plot2::scatter(s1,s2);
//    service::vtkhl::plot2::show(true);
}
