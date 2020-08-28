#include <habits/clustering/cluster_trajectories.h>
habits::clustering::cluster_trajectories::cluster_trajectories(const representations::interfaces::segment_cluster_group &cluster_group) {
    run(cluster_group);
}
void habits::clustering::cluster_trajectories::run(const representations::interfaces::segment_cluster_group &segmentation) {
    for (auto it = segmentation.begin(); it != segmentation.end(); ++it){
        m_mean_trajectories.emplace(it.key(),calculate_mean_trajectories(it.value().as<representations::interfaces::segment_cluster>()));
    }
}
std::vector<boost::shared_ptr<representations::interfaces::ordered_collection>> habits::clustering::cluster_trajectories::calculate_mean_trajectories(const representations::interfaces::segment_cluster &cluster) {
    // magic here I guess
    
}
