#include <habits/clustering/cluster_trajectories.h>
#include <km/mixture_em_agl.h>
#include <km/mixture_agl.h>
#include <eigenhl/dtw.h>
#include <eigenhl/interp.h>
#include <simple-logger/simple-logger.h>
#include <gppe/timing.h>
#include <iomanip>
#include <atomic>

typedef std::reference_wrapper<representations::interfaces::ordered_collection_base> collection_reference;

habits::clustering::cluster_trajectories::cluster_trajectories(const representations::interfaces::segment_cluster_group &cluster_group) {
    run(cluster_group);
}
void habits::clustering::cluster_trajectories::run(const representations::interfaces::segment_cluster_group &segmentation) {
    for (auto it = segmentation.begin(); it != segmentation.end(); ++it){
        m_cluster_info.emplace(it.key(),cluster_info());
        m_mean_trajectories.emplace(it.key(),calculate_mean_trajectories(it.value().as<representations::interfaces::segment_cluster>(),m_cluster_info[it.key()]));
    }
}

// create clustering functions
//template<> Eigen::VectorXd km::kernels::gaussian_kernel<collection_reference>::residual(const collection_reference & a, const collection_reference &b) const {
//    Eigen::VectorXd x(1); x[0] = a.get().dtw_alignment_error(b.get());
//    return std::move(x);
//}

//template<> void km::kernels::kernel<collection_reference>::mean_function(const std::vector<collection_reference> &vec, const std::vector<std::vector<double>> &responsibilities) {
//    // calculate mean:
//    boost::shared_ptr<representations::interfaces::ordered_collection> t_mean = boost::dynamic_pointer_cast<representations::interfaces::ordered_collection>(vec.front().get().empty_clone());
//    bool mean_set = false;
//    for (int i = 0; i < vec.size(); i++) {
//        if (responsibilities[i][m_id] > 1e-8) {
//            if (!mean_set) {
//                mean_set = true;
//                m_kernel_responsibility = responsibilities[i][m_id];
//                t_mean->set_data(vec[i].get().cbegin(),vec[i].get().cend());
//                continue;
//            }
//            auto dtw_output = vec[i].get().dtw_alignment(*t_mean);
//            Eigen::VectorXi mapping_index = eigenhl::dtw_index_vector(dtw_output.first);
//            double total_responsibility = responsibilities[i][m_id] + m_kernel_responsibility;
//
//        }
//    }
//}

//template<> Eigen::MatrixXd km::kernels::gaussian_kernel<

std::vector<boost::shared_ptr<representations::interfaces::ordered_collection>> habits::clustering::cluster_trajectories::calculate_mean_trajectories(const representations::interfaces::segment_cluster &cluster, cluster_info & data) {
    // resize cluster info variables
//    data.weights.resize(cluster.size());
//    data.covariances.resize(cluster.size());
//    data.residual_averages.resize(cluster.size());
//    km::mixture_agl<std::reference_wrapper<representations::interfaces::ordered_collection_base>> mixture(cluster.to_reference_vector<representations::interfaces::ordered_collection_base>());
//    mixture.set_merge_distance(m_merge_distance);
//    mixture.set_number_of_kernels(cluster.size());
//    mixture.set_iteration_limit(50);
//    mixture.solve();
}
