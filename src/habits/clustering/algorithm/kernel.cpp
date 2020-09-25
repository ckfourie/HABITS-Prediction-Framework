#include <habits/clustering/algorithm/kernel.h>
#include <eigenhl/dtw.h>
#include <eigenhl/interp.h>
#include <simple-logger/simple-logger.h>
#include <representations/interfaces/group.h>
habits::clustering::algorithm::kernel::kernel(const int &id) : m_id(id) {
    m_weight = 1.0; m_kernel_responsibility = 1.0; m_covariance = 1;
    m_constant = 1.0/sqrt(2*M_PI)/sqrt(m_covariance);
}

habits::clustering::algorithm::kernel::kernel(const int &id, const representations::interfaces::ordered_collection_base &mean) : m_id(id) {
    m_mean = boost::dynamic_pointer_cast<representations::interfaces::ordered_collection>(mean.empty_clone()); m_weight = 1.0; m_kernel_responsibility = 1.0; m_covariance = 1;
    m_constant = 1.0/sqrt(2*M_PI)/sqrt(m_covariance);
    boost::dynamic_pointer_cast<representations::interfaces::ordered_collection>(m_mean)->set_data(mean.cbegin(),mean.cend());
}

void habits::clustering::algorithm::kernel::mean(const representations::interfaces::ordered_collection_base &value) {
    m_mean = boost::dynamic_pointer_cast<representations::interfaces::ordered_collection>(value.empty_clone());
    boost::dynamic_pointer_cast<representations::interfaces::ordered_collection>(m_mean)->set_data(value.cbegin(),value.cend());
}

double habits::clustering::algorithm::kernel::likelihood(const representations::interfaces::ordered_collection_base &x) const {
    return m_weight * m_constant *exp(-(m_mean->dtw_alignment_error(x))/(2*m_covariance));
}
double habits::clustering::algorithm::kernel::negative_log_likelhood(const representations::interfaces::ordered_collection_base &x) const {
    return -log(likelihood(x));
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

void habits::clustering::algorithm::kernel::fit(const representations::interfaces::unordered_collection &x, const Eigen::MatrixXd &responsibilities) {
    boost::shared_ptr<representations::interfaces::ordered_collection> t_mean = boost::dynamic_pointer_cast<representations::interfaces::ordered_collection>(x.begin().value().empty_clone());
    bool mean_set = false; int i = 0; m_kernel_responsibility = 0;
    for (auto it = x.begin(); it != x.end(); ++it) {
        const auto & ref = dynamic_cast<const representations::interfaces::ordered_collection_base&>(it.value());
        if (responsibilities(i,m_id) > 1e-8) {
            if (!mean_set) {
                mean_set = true;
                m_kernel_responsibility = responsibilities(i,m_id);
                t_mean->set_data(ref.cbegin(),ref.cend());
                i++;
                continue;
            }
            auto dtw_output = ref.dtw_alignment(*t_mean);
            Eigen::VectorXi mapping_index = eigenhl::dtw_index_vector(dtw_output.first);
            double total_responsibility = responsibilities(i,m_id) + m_kernel_responsibility;
            // project 3d for now, in future this will need to involve operations on the actual representation.
            Eigen::MatrixXd unweighted_ref = ref.project3d().transpose();
            Eigen::MatrixXd unweighted_mean = t_mean->project3d().transpose();
            Eigen::MatrixXd weighted_x = responsibilities(i,m_id) * unweighted_ref/total_responsibility +
                                         subsect(unweighted_mean, mapping_index) * m_kernel_responsibility/total_responsibility;
            Eigen::VectorXd weighted_indices = responsibilities(i,m_id) /total_responsibility * Eigen::VectorXd::LinSpaced(weighted_x.rows(), 0, weighted_x.rows()-1) +
                                               mapping_index.cast<double>()*m_kernel_responsibility/total_responsibility;
            std::function<Eigen::MatrixXd(const Eigen::VectorXd &)> interp_func = eigenhl::interp(weighted_indices, weighted_x);
            m_kernel_responsibility += responsibilities(i,m_id);
            int limit = (int) round(weighted_indices[weighted_indices.rows()-1]);
            Eigen::MatrixXd output = interp_func(Eigen::VectorXd::LinSpaced(limit+1, 0, limit));
            // check if we're dealing with an indexed version of an ordered_collection
            try {
                const auto & value1 = dynamic_cast<const representations::interfaces::index_element_group_base&>(ref.at(0));
                const auto & value2 = dynamic_cast<const representations::interfaces::index_element_group_base&>(ref.at(1));
                double delta = value2.get_abstract_index_value() - value1.get_abstract_index_value();
                // if we're here then we must modify the matrix
                Eigen::MatrixXd augmented = Eigen::MatrixXd(output.cols()+1,output.rows());
                augmented.block(0,0,1,augmented.cols()) = Eigen::RowVectorXd::LinSpaced(augmented.cols(),0,augmented.cols()-1)*delta;
                augmented.block(1,0,augmented.rows()-1,augmented.cols()) = output.transpose();
                t_mean->set_data(augmented);
            } catch (const std::bad_cast & e) {
                t_mean->set_data(output.transpose());
            }
        }
        i++;
    }

    m_mean = t_mean;
    m_weight = m_kernel_responsibility;
    // set the residual average
    m_residual_average = 0.0; i =0; m_covariance = 0;
    std::vector<double> errors; errors.reserve(x.size());
    for (auto it = x.begin(); it != x.end(); ++it) {
        if (responsibilities(i,m_id) <= 1e-8){i++; continue;}
        const auto &ref = dynamic_cast<const representations::interfaces::ordered_collection_base &>(it.value());
        double error = m_mean->dtw_alignment_error(ref)/(std::max(m_mean->size(),ref.size()));
        errors.emplace_back(error);
        m_residual_average += responsibilities(i,m_id) * error;
        i++;
    }
    m_residual_average /= m_kernel_responsibility;
    for (int i = 0; i < errors.size(); i++) m_covariance += responsibilities(i,m_id)*pow(errors[i]-m_residual_average,2);
    m_covariance /= m_kernel_responsibility;
    if (m_covariance < 1e-8) m_covariance = 1e-8; // avoid fp errors for now
    m_constant = 1.0/sqrt(2*M_PI)/sqrt(m_covariance);
}

