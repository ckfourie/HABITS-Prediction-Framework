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
#include <representations/operations/semantic_operations.h>
#include <habits/utils/dataset_name_structure.h>
#include <habits/utils/setup_omp.h>
#include <habits/segmentation/state_based_segmentation.h>
#include <habits/data/activity_segmentation.h>
#include <habits/clustering/cluster_trajectories.h>
#include <representations/operations/unordered_collection_operations.h>
#include <habits/predictors/learn_predictors.h>
#include <habits/predictors/bestpta_predictor.h>
#include <gppe/python.h>
#include <habits/predictors/group_predictors.h>
#include <habits/predictors/sequence_predictor.h>
#include <cctype>
#include <gppe/string_manipulations.h>
using namespace representations;
int main (int argc, char ** argv) {
    gppe::timer t;
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
