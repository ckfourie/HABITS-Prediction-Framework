#include <habits/core/options.h>
#include <simple-logger/simple-logger.h>
#include <habits/utils/setup_omp.h>
#include <habits/datasets.h>
#include <representations/trajectory.h>
#include <habits/segmentation/state_based_segmentation.h>
#include <service/vtkhl/plot3.h>
int main (int argc, char ** argv) {
    habits::utils::setup_omp();
    habits::core::options options ("habits_live_segmentation","view the live segmentation for a particular dataset, subject and trajectory");
    options.enable_subject_regex();
    options.enable_trajectory_regex();
    options.add_option("n,no-wait","wait after plotting for window to close before continuing");
    options.parse_input(argc,argv);
    for (auto it = habits::active_dataset().begin(); it != habits::active_dataset().end(); ++it) {
        const std::string & trajectory_name = it.key();
        const auto & trajectory = it.value().as<const representations::trajectory3d&>();
        auto group = representations::trajectory_cluster3d(); group.move_insert(trajectory_name,representations::trajectory3d());
        representations::trajectory3d & stream = group.at(trajectory_name);
        habits::segmentation::state_based_segmentation test_segmentation (group);
        test_segmentation.incremental_update_interval(5);
        auto fig = service::vtkhl::plot3::figure("habits_live_segmentation::" + trajectory_name); fig->autozoom(true);
        service::vtkhl::plot3::show(false); int i = 0;
        for (const auto & point : trajectory) {
            stream << point; i++;
            if (i == 5) service::vtkhl::plot3::plot(test_segmentation);
            usleep(25000);
        }
        if (!options.check_arg("no-wait")) service::vtkhl::plot3::show(true);
    }
    service::vtkhl::plot3::show(true);
}