#include <habits/datasets.h>
#include <habits/core/options.h>
#include <gppe/timing.h>
#include <representations/trajectory.h>
#include <service/vtkhl/plot3.h>
#include <habits/segmentation/interfaces/segmentation.h>
using namespace representations;
int main (int argc, char ** argv) {
    gppe::delta_timer t;
    habits::core::options options ("playground","test functionality (playing around with stuff)");
    options.parse_input(argc,argv);
    simple_logger::set_verbosity_level(simple_logger::verbosity_level::trace);
    SLOG(debug) << "passed options, trying to get data (options processing took " << t << ")";
    // print all the goals:
//    SLOG(debug) << "goals are: " << habits::goal_information();
    const representations::point_cluster3d & goals = dynamic_cast<const representations::point_cluster3d&> (habits::goal_information());
    std::vector<std::string> keys = goals.keys();
    for (size_t i = 0; i < keys.size(); i++) {
        SLOG(debug) << "goal::: " << keys[i] << " :: " << goals.at(keys[i]);
        SLOG(debug) << "goal::: " << keys[i] << " :: (" << goals.at(keys[i])[0] << ", " << goals.at(keys[i])[1] << ", " << goals.at(keys[i])[2] << ")";
    }
    // get active dataset:
    auto data = dynamic_cast<const representations::time_series_cluster3d&>(habits::active_dataset());
    // create segmentation
    auto segmentation = habits::segmentation::interfaces::segmentation<representations::point3>(data);
    // print segmentation:
    SLOG(debug) << segmentation;
    SLOG(debug) << "retrieving goal information took " << t;
    // try plot all the data:
    service::vtkhl::plot3::plot(habits::active_dataset());
    SLOG(debug) << "plotted data (processing took " << t << ")";
    service::vtkhl::plot3::show();
//    Py_Initialize();
//    service::core::dataserver::initialize("localhost",62014);
//    representations::trajectory3d::set_interpolation_strategy(representations::trajectory3d::interpolation_style::CUBIC_SPLINE);
//    service::parameters::config::set_project("hap");
//    habits::load_dataset("bolts","subject_1");
//    habits::segmentation::state_based_segmentation segmentation (habits::active_dataset());
//    habits::predictors::sequence_predictor sequence_predictor (segmentation);
//    habits::data::activity_segmentation activity_group (segmentation);
//    habits::clustering::cluster_trajectories cluster (activity_group);
//    auto predictors = habits::predictors::learn_predictors(cluster,activity_group,habits::predictors::bestpta_predictor());
//    SLOG(info) << "trained and completed in " << t.elapsed();
//    habits::predictors::group_predictors predictor_group (predictors);
//    for (auto it = habits::active_dataset().begin(); it != habits::active_dataset().end(); ++it) {
//        const std::string &trajectory_name = it.key();
//        const auto &trajectory = it.value().as<const representations::trajectory3d &>();
//        auto group = representations::trajectory_cluster3d();
//        group.move_insert(trajectory_name, representations::trajectory3d());
//        representations::trajectory3d &stream = group.at(trajectory_name);
//        habits::segmentation::state_based_segmentation test_segmentation(group);
//        predictor_group.set_reference_segmentation(test_segmentation.begin().value().as<const representations::interfaces::segmentation &>());
//        sequence_predictor.set_predictors(predictors);
//        sequence_predictor.set_reference_segmentation(test_segmentation.begin().value().as<const representations::interfaces::segmentation &>());
//        service::vtkhl::plot3::figure("live update with predictions");
//        int i = 0;
//        for (const auto &point : trajectory) {
//            gppe::timer update_time;
//            stream << point;
//            if (i++ == 5) {
//                service::vtkhl::plot3::plot(stream);
//                for (const auto &p : predictor_group.spatial_predictions()) {
//                    auto lines = service::vtkhl::plot3::plot(p.get());
//                    lines[0]->set_color(0, 255, 0);
//                }
//                service::vtkhl::plot3::plot(sequence_predictor.current_prediction());
//                service::vtkhl::plot3::show(false);
//            }
//            if (update_time.mselapsed() < 50) {
//                long dt = 50 - update_time.mselapsed();
//                if (dt > 0) usleep(dt * 1000);
//            }
//        }
//        service::vtkhl::plot2::show(true);
//    }
}
