#include <habits/clustering/kernel.h>
#include <eigenhl/dtw.h>
#include <eigenhl/interp.h>

habits::clustering::kernel::kernel(const int &id) : m_id(id) {
    m_weight = 1.0; m_kernel_responsibility = 1.0; m_total_number_of_points = 1; m_covariance = 1;
}

habits::clustering::kernel::kernel(const int &id, const representations::interfaces::ordered_collection_base &mean) : m_id(id) {
    m_mean = boost::dynamic_pointer_cast<representations::interfaces::ordered_collection>(mean.empty_clone()); m_weight = 1.0; m_kernel_responsibility = 1.0; m_total_number_of_points = 1; m_covariance = 1;
    boost::dynamic_pointer_cast<representations::interfaces::ordered_collection>(m_mean)->set_data(mean.cbegin(),mean.cend());
}
double habits::clustering::kernel::likelihood(const representations::interfaces::ordered_collection_base &x) const {
    return 0; // todo!
}
double habits::clustering::kernel::negative_log_likelhood(const representations::interfaces::ordered_collection_base &x) const {
    return 0; // todo!
}

// helper function
Eigen::MatrixXd subsect (const Eigen::MatrixXd & mat , const Eigen::VectorXi & subs) {
    Eigen::MatrixXd r (subs.rows(),mat.cols());
    for (int i = 0; i < subs.rows(); i++) {
        if (subs[i] >= mat.rows()) throw std::runtime_error("index out of bounds");
        r.row(i) = mat.row(subs[i]);
    }
    return std::move(r);
}

void habits::clustering::kernel::fit(const representations::interfaces::unordered_collection &x, const Eigen::MatrixXd &responsibilities) {
    // calculate the mean here, calculate the covariance
    // here we specifically model the covariance on the EM fit as the variance on the alignment
    // first we calculate the mean trajectory:
    boost::shared_ptr<representations::interfaces::ordered_collection> t_mean = boost::dynamic_pointer_cast<representations::interfaces::ordered_collection>(x.begin().value().empty_clone());
    bool mean_set = false; int i = 0;
    for (auto it = x.begin(); it != x.end(); ++it) {
        const auto & ref = dynamic_cast<const representations::interfaces::ordered_collection_base&>(it.value());
        if (responsibilities(i,m_id) > 1e-8) {
            if (!mean_set) {
                mean_set = true;
                m_kernel_responsibility = responsibilities(i,m_id);
                t_mean->set_data(ref.cbegin(),ref.cend());
                continue;
            }
            auto dtw_output = ref.dtw_alignment(*t_mean);
            Eigen::VectorXi mapping_index = eigenhl::dtw_index_vector(dtw_output.first);
            double total_responsibility = responsibilities(i,m_id) + m_kernel_responsibility;
            // project 3d for now, in future this will need to involve operations on the actual representation.
            Eigen::MatrixXd unweighted_ref = ref.project3d();
            Eigen::MatrixXd unweighted_mean = t_mean->project3d();
            Eigen::MatrixXd weighted_x = responsibilities(i,m_id) * unweighted_ref/total_responsibility +
                                         subsect(unweighted_mean, mapping_index) * m_kernel_responsibility/total_responsibility;
            Eigen::VectorXd weighted_indices = responsibilities(i,m_id) /total_responsibility * Eigen::VectorXd::LinSpaced(weighted_x.rows(), 0, weighted_x.rows()-1) +
                                               mapping_index.cast<double>()*m_kernel_responsibility/total_responsibility;
            std::function<Eigen::MatrixXd(const Eigen::VectorXd &)> interp_func = eigenhl::interp(weighted_indices, weighted_x.transpose());
            m_kernel_responsibility += responsibilities(i,m_id);
            int limit = (int) round(weighted_indices[weighted_indices.rows()-1]);
            Eigen::MatrixXd output = interp_func(Eigen::VectorXd::LinSpaced(limit+1, 0, limit));
            t_mean->set_data(output.transpose());
        }
        i++;
    }
    m_mean = t_mean;
    m_weight = m_kernel_responsibility/m_total_number_of_points;
    // set the residual average
    m_residual_average = 0.0; i =0; m_covariance = 0;
    for (auto it = x.begin(); it != x.end(); ++it) {
        const auto &ref = dynamic_cast<const representations::interfaces::ordered_collection_base &>(it.value());
        double error = m_mean->dtw_alignment_error(ref)/(std::max(m_mean->size(),ref.size()));
        m_residual_average += responsibilities(i,m_id) * error;
        m_covariance += responsibilities(i,m_id) * error*error;
        i++;
    }
    m_residual_average /= m_kernel_responsibility;
    m_covariance /= m_kernel_responsibility;
    if (m_covariance < 1e-8) m_covariance = 1e-8; // avoid fp errors for now
}
