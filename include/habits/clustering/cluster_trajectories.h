#pragma once
#include <representations/interfaces/collection.h>
#include <habits/segmentation/interfaces/segmentation.h>
namespace habits {
    namespace clustering {
        class cluster_trajectories {
        public:
            explicit cluster_trajectories (const representations::interfaces::segment_cluster_group & cluster_group);
        protected:
            class cluster_info {
            public:
                cluster_info() = default;
                std::vector<double> weights;
                std::vector<double> residual_averages;
                std::vector<Eigen::MatrixXd> covariances;
            };
            void run (const representations::interfaces::segment_cluster_group & segmentation);
            std::vector<boost::shared_ptr<representations::interfaces::ordered_collection>> calculate_mean_trajectories(const representations::interfaces::segment_cluster & cluster, cluster_info & data);
            std::unordered_map<std::string,std::vector<boost::shared_ptr<representations::interfaces::ordered_collection>>> m_mean_trajectories;
            double m_merge_distance = 25.0;
            std::unordered_map<std::string, cluster_info> m_cluster_info;
        };
    }
}