#include "habits/predictors/learn_predictors.h"
#include <simple-logger/simple-logger.h>
std::vector<boost::shared_ptr<habits::predictors::interfaces::predictor>> habits::predictors::learn_predictors(const habits::clustering::cluster_trajectories &clustered_trajectories, const habits::data::activity_segmentation &segmentation, const habits::predictors::interfaces::predictor &predictor_template) {
    std::vector<boost::shared_ptr<interfaces::predictor>> predictors;
    // step through activity groups:
    for (auto it = segmentation.begin(); it != segmentation.end(); ++it) {
        const std::string & key = it.key();
        const auto & segment_cluster = it.value().as<const representations::interfaces::segment_cluster>();
        // get clustered output for this activity group
        const std::vector<habits::clustering::algorithm::kernel>& kernels = clustered_trajectories.get_kernel_data(key);
        // for each cluster learn the predictor:
        for (const habits::clustering::algorithm::kernel & kernel : kernels) {
            // create source trajectories container
            representations::interfaces::segment_cluster source_trajectories;
            for (const unsigned long & id : kernel.kernel_indices()) source_trajectories.insert(segment_cluster.key(id),segment_cluster.at(id));
            // learn the predictor
            boost::shared_ptr<habits::predictors::interfaces::predictor> predictor = predictor_template.empty_clone();
            // train it
            predictor->train(kernel.mean(),source_trajectories);
            predictor->assign_parameters_from_kernel(kernel);
            predictors.emplace_back(predictor);
        }
    }
    return predictors;
}
