#pragma once
#include <representations/interfaces/collection.h>
#include <habits/clustering/algorithm/kernel.h>
namespace habits {
    namespace clustering {
        namespace algorithm {
            class em {
            public:
                em();
                virtual int solve(const representations::interfaces::unordered_collection & collection);
                std::vector<habits::clustering::algorithm::kernel>& kernels(){return m_kernels;};
                double convergence_threshold() const {return m_convergence_threshold;}
                void convergence_threshold(const double & value){m_convergence_threshold = value;}
                unsigned long iteration_limit() const {return m_iteration_limit;}
                void iteration_limit(const unsigned long & value) {m_iteration_limit = value;}
                unsigned long number_of_kernels() const {return m_number_of_kernels;}
                void number_of_kernels(const unsigned long & value){m_number_of_kernels = value;}
                double negative_log_likelihood(const representations::interfaces::unordered_collection & collection) const;
            protected:
                virtual void maximize(const representations::interfaces::unordered_collection & collection);
                virtual void expectation(const representations::interfaces::unordered_collection & collection);
                virtual void initialize(const representations::interfaces::unordered_collection & collection);
                std::vector<habits::clustering::algorithm::kernel> m_kernels;
                double m_convergence_threshold; unsigned long m_iteration_limit; unsigned long m_number_of_kernels;
                Eigen::MatrixXd m_responsibilities;
            };
        }
    }
}