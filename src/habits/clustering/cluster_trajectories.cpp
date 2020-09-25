#include <habits/clustering/cluster_trajectories.h>
#include <habits/clustering/algorithm/em.h>
#include <habits/clustering/algorithm/agl.h>
#include <eigenhl/interp.h>
#include <simple-logger/simple-logger.h>
#include <gppe/timing.h>
#include <iomanip>
#include <atomic>
#include <service/vtkhl/plot3.h>
#include <boost/thread.hpp>
typedef std::reference_wrapper<representations::interfaces::ordered_collection_base> collection_reference;

habits::clustering::cluster_trajectories::cluster_trajectories(const representations::interfaces::segment_cluster_group &cluster_group) {
    run(cluster_group);
}
void habits::clustering::cluster_trajectories::run(const representations::interfaces::segment_cluster_group &segmentation) {
    boost::mutex local_mutex; int i = 1; gppe::timer t;
    #pragma omp parallel
    {
        #pragma omp single
        {
            for (auto it = segmentation.begin(); it != segmentation.end(); ++it) {
                #pragma omp task firstprivate(it)
                {
                    auto data = calculate_mean_trajectories(it.value().as<representations::interfaces::segment_cluster>());
                    boost::lock_guard<boost::mutex> lock(local_mutex);
                    m_clustering_map.emplace(it.key(), data);
                    SLOG(trace) << "[" << i++ << "/" << segmentation.size() << "] : clustered trajectories: " << it.key() << ": " << data.size() << " mean trajectories";
                }
            }
        }
    }
    SLOG(trace) << "clustering completed in " << t.elapsed() << "s";
}

std::vector<habits::clustering::algorithm::kernel> habits::clustering::cluster_trajectories::calculate_mean_trajectories(const representations::interfaces::segment_cluster &cluster) {
    habits::clustering::algorithm::agl clustering;
    clustering.solve(cluster);
//    for (auto & k : clustering.kernels())
//        SLOG(debug) << "kernel: residual_average = " << k.residual_average() << ", covariance = " << k.covariance() << ", weight = " << k.weight();
    return clustering.kernels();
}
