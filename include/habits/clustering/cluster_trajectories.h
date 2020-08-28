#pragma once
#include <representations/interfaces/collection.h>
#include <habits/segmentation/interfaces/segmentation.h>
namespace habits {
    namespace clustering {
        class cluster_trajectories {
        public:
            cluster_trajectories (const representations::interfaces::segment_cluster_group & cluster_group);
        protected:
            void run (const representations::interfaces::segment_cluster_group & segmentation);
            std::vector<boost::shared_ptr<representations::interfaces::ordered_collection>> calculate_mean_trajectories(const representations::interfaces::segment_cluster & cluster);
            std::unordered_map<std::string,std::vector<boost::shared_ptr<representations::interfaces::ordered_collection>>> m_mean_trajectories;
        };
    }
}