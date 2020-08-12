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
    habits::load_dataset("bolts","subject_1");
    // get the first trajectory
    service::vtkhl::plot2::plot(habits::active_dataset());
    service::vtkhl::plot3::plot(habits::active_dataset());
    service::vtkhl::plot3::show(false);
    service::vtkhl::plot2::show(true);

//    service::vtkhl::plot3::plot(habits::active_dataset());
//    service::vtkhl::plot3::show(false);
//    service::vtkhl::plot2::plot(habits::active_dataset());
//    service::vtkhl::plot2::show(true);
}
