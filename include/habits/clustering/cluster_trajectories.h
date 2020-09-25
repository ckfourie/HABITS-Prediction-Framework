#pragma once
#include <representations/interfaces/collection.h>
#include <habits/segmentation/interfaces/segmentation.h>
#include <habits/clustering/algorithm/kernel.h>
namespace habits {
    namespace clustering {
        class cluster_trajectories {
        public:
            explicit cluster_trajectories (const representations::interfaces::segment_cluster_group & cluster_group);
            const std::vector<algorithm::kernel> & get_kernel_data(const std::string & label) const {return m_clustering_map.at(label);}
        protected:
            void run (const representations::interfaces::segment_cluster_group & segmentation);
            std::vector<algorithm::kernel> calculate_mean_trajectories(const representations::interfaces::segment_cluster & cluster);
            std::unordered_map<std::string,std::vector<algorithm::kernel>> m_clustering_map;
        };
    }
}