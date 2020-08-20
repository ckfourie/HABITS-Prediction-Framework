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
#include <habits/utils/dataset_name_structure.h>
#include <habits/utils/setup_omp.h>
using namespace representations;
int main (int argc, char ** argv) {
    habits::utils::setup_omp();
    representations::trajectory3d::set_interpolation_strategy(representations::trajectory3d::interpolation_style::CUBIC_SPLINE);
    service::core::dataserver::initialize("localhost",62014);
    habits::load_dataset("cubes",".*");
    service::vtkhl::plot3::plot(habits::active_dataset());
    service::vtkhl::plot3::figure("scatter!");
    service::vtkhl::plot3::scatter(habits::active_dataset());
    service::vtkhl::plot3::show(false);
    service::vtkhl::plot2::plot(habits::active_dataset());
    service::vtkhl::plot2::figure("scatter!");
    service::vtkhl::plot2::scatter(habits::active_dataset());
    service::vtkhl::plot2::show(true);
}
