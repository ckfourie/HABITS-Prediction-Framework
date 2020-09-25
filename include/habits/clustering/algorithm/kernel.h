#pragma once
#include <representations/interfaces/collection.h>
#include <representations/interfaces/homogeneous_cluster.h>
namespace habits {
    namespace clustering {
        namespace algorithm {
            class kernel {
            public:
                kernel(const int &id);
                kernel(const int &id, const representations::interfaces::ordered_collection_base &mean);
                virtual double likelihood(const representations::interfaces::ordered_collection_base &x) const;
                virtual double negative_log_likelhood(const representations::interfaces::ordered_collection_base &x) const;
                virtual void fit(const representations::interfaces::unordered_collection &x, const Eigen::MatrixXd &responsibilities);
                double weight() const { return m_weight; }
                double covariance() const { return m_covariance; }
                double residual_average() const { return m_residual_average; }
                const representations::interfaces::ordered_collection_base& mean () const {return *m_mean;}
                void mean (const representations::interfaces::ordered_collection_base & value);
                boost::shared_ptr<representations::interfaces::ordered_collection> mean_ptr() {return m_mean;}
                void kernel_indices(const std::vector<unsigned long> & indices){m_kernel_indices = indices;}
                const std::vector<unsigned long> & kernel_indices() const {return m_kernel_indices;}
            protected:
                int m_id;
                boost::shared_ptr<representations::interfaces::ordered_collection> m_mean;
                // this should be upgraded in the future to construct alignment priors for bestpta style alignment
                double m_covariance, m_constant;
                double m_kernel_responsibility, m_weight;
                double m_residual_average;
                std::vector<unsigned long> m_kernel_indices;
            };
        }
    }
}