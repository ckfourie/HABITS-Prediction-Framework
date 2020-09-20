#pragma once
#include <representations/interfaces/collection.h>
#include <representations/interfaces/homogeneous_cluster.h>
namespace habits {
    namespace clustering {
        class kernel {
        public:
            kernel(const int & id);
            kernel(const int & id, const representations::interfaces::ordered_collection_base & mean);
            virtual double likelihood(const representations::interfaces::ordered_collection_base & x) const;
            virtual double negative_log_likelhood(const representations::interfaces::ordered_collection_base & x) const;
            virtual void fit (const representations::interfaces::unordered_collection & x, const Eigen::MatrixXd & responsibilities);
            double weight () const {return m_weight;}
            double covariance() const {return m_covariance;}
        protected:
            int m_id; unsigned long m_total_number_of_points;
            boost::shared_ptr<representations::interfaces::ordered_collection> m_mean;
            // this should be upgraded in the future to construct alignment priors for bestpta style alignment
            double m_covariance;
            double m_kernel_responsibility, m_weight;
            double m_residual_average;
        };
    }
}