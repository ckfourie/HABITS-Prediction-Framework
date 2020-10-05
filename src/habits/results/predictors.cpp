#include <habits/results/predictors.h>
#include <representations/operations/unordered_collection_operations.h>
#include <habits/segmentation/state_based_segmentation.h>
#include <habits/data/activity_segmentation.h>
#include <habits/clustering/cluster_trajectories.h>
#include <habits/predictors/group_predictors.h>
#include <habits/predictors/sequence_predictor.h>
#include <habits/predictors/learn_predictors.h>
#include <representations/operations/semantic_operations.h>
std::vector<habits::results::predictor_results> habits::results::calculate_predictor_results(const representations::interfaces::unordered_collection &data, const habits::predictors::interfaces::predictor &predictor_template) {
    std::vector<predictor_results> output; output.reserve(data.size());
    // step through the data
    double incremental_average = 0.0; int count = 0;
    for (auto it = data.begin(); it != data.end(); ++it) {
        // get the trajectory
        const auto & trajectory = it.value().as<const representations::interfaces::ordered_collection>();
        boost::shared_ptr<representations::interfaces::unordered_collection> loocv_collection = representations::loocv(data,it.key());
        // calculate predictors etc.
        gppe::timer t;
        habits::segmentation::state_based_segmentation segmentation (*loocv_collection);
        habits::predictors::sequence_predictor sequence_predictor (segmentation);
        habits::data::activity_segmentation activity_group (segmentation);
        habits::clustering::cluster_trajectories cluster (activity_group);
        auto predictors = habits::predictors::learn_predictors(cluster,activity_group,predictor_template);
        habits::predictors::group_predictors predictor_group (predictors);
        SLOG(info) << it.key() << ":: trained and completed in " << t.elapsed();
        // create the group predictors object
        auto group = representations::trajectory_cluster3d(); group.move_insert(it.key(),representations::trajectory3d());
        representations::trajectory3d & stream = group.at(it.key());
        habits::segmentation::state_based_segmentation test_segmentation (group);
        predictor_group.set_reference_segmentation(test_segmentation.begin().value().as<const representations::interfaces::segmentation&>());
        sequence_predictor.set_predictors(predictors);
        sequence_predictor.set_reference_segmentation(test_segmentation.begin().value().as<const representations::interfaces::segmentation&>());
        std::vector<long> update_times; update_times.reserve(trajectory.size());
        for (const auto & point : trajectory) {
            gppe::timer update_time;
            stream << point;
            update_times.emplace_back(update_time.mselapsed());
        }
        // get results and store them.
        // for now lets just calculate the average error
        representations::trajectory1s semantic_prediction = predictor_group.event_prediction();
        representations::trajectory1s semantic_prediction_gt = representations::future_semantic_sequence(test_segmentation.begin().value().as<const representations::interfaces::segmentation>());
        count++;
        double performance = representations::percent_identical(semantic_prediction,semantic_prediction_gt);
        incremental_average += (performance-incremental_average)/count;
        SLOG(fatal) << "percent overlap = " << performance;
        SLOG(fatal) << "running average = " << incremental_average;
    }
    return output;
}
