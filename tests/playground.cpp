#include <simple-logger/simple-logger.h>
#include <service/core/dataserver.h>
#include <habits/datasets.h>
#include <habits/core/options.h>
#include <service/vtkhl/plot3.h>
#include <service/vtkhl/plot2.h>
#include <representations/trajectory.h>
#include <representations/interfaces/semantic.h>
#include <representations/interfaces/segmentation.h>
int main (int argc, char ** argv) {

    service::core::dataserver::initialize("localhost",62014);
    habits::load_dataset("bolt_placement_dataset","subject_1");
    // get the first trajectory
    auto trajectory = habits::active_dataset().begin().value().as<representations::trajectory3d>();
    service::vtkhl::plot2::plot(trajectory);
    trajectory.resample(5.0);  // downsample
//    service::vtkhl::plot2::scatter(trajectory);
    auto downsampled_trajectory = trajectory;
    representations::trajectory3d::set_interpolation_strategy(representations::trajectory3d::interpolation_style::CUBIC_SPLINE);
    trajectory.resample(20.0);
    service::vtkhl::plot2::scatter(trajectory);
    service::vtkhl::plot2::scatter(downsampled_trajectory);
    service::vtkhl::plot2::show(true);

//    service::vtkhl::plot3::plot(habits::active_dataset());
//    service::vtkhl::plot3::show(false);
//    service::vtkhl::plot2::plot(habits::active_dataset());
//    service::vtkhl::plot2::show(true);
}
