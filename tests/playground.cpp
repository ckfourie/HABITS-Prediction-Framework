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
#include <habits/segmentation/state_based_segmentation.h>
#include <cctype>
#include <gppe/string_manipulations.h>
using namespace representations;
int main (int argc, char ** argv) {
    habits::utils::setup_omp();
    representations::trajectory3d::set_interpolation_strategy(representations::trajectory3d::interpolation_style::CUBIC_SPLINE);
    service::core::dataserver::initialize("localhost",62014);
    habits::load_dataset("bolts",".*");
    habits::segmentation::state_based_segmentation segmentation (habits::active_dataset());
//    service::vtkhl::plot2::plot(segmentation);
//    service::vtkhl::plot2::show(true);
    // plot the derivatives
    service::vtkhl::plot3::plot(segmentation);
    service::vtkhl::plot3::show(true);
}
