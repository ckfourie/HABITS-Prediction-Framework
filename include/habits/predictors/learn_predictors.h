#pragma once
#include <habits/predictors/interfaces/predictor.h>
#include <habits/segmentation/interfaces/segmentation.h>
#include <habits/clustering/cluster_trajectories.h>
#include <habits/data/activity_segmentation.h>
namespace habits {
    namespace predictors {
        std::vector<boost::shared_ptr<interfaces::predictor>> learn_predictors(const habits::clustering::cluster_trajectories & clustered_trajectories, const habits::data::activity_segmentation & segmentation, const interfaces::predictor & predictor_template);
    }
}