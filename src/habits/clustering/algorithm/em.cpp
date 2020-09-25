#include <habits/clustering/algorithm/em.h>
#include <random>
#include <chrono>
#include <simple-logger/simple-logger.h>
habits::clustering::algorithm::em::em() {
    // default parameters
    m_convergence_threshold = 1e-2;
    m_iteration_limit = 100;
}
int habits::clustering::algorithm::em::solve(const representations::interfaces::unordered_collection &collection) {
    // assign kernels if necessary
    m_kernels.clear();
    for (int i = m_kernels.size(); i < m_number_of_kernels; i++)
        m_kernels.emplace_back(kernel(i));
    m_responsibilities = Eigen::MatrixXd::Zero(collection.size(),m_kernels.size());
    initialize(collection);
    double cost = 0; double old_cost = 1e25;
    int count = 0;
    while (fabs(cost-old_cost) > m_convergence_threshold){
        if (count >= m_iteration_limit) break;
        old_cost = cost;
        maximize(collection); expectation(collection);
        cost = negative_log_likelihood(collection); count++;
        SLOG(debug) << "completed iteration " << count << ":: cost = " << cost;
        if (cost > old_cost) break; // converged: todo: we'll need to look into this, it sounds like we're seeing some non-convergent behavior here (this should never happen).
    }
    return count-1;
}
double habits::clustering::algorithm::em::negative_log_likelihood(const representations::interfaces::unordered_collection & collection) const {
    // step through kernels:
    double log_likelihood = 0;
    for (const auto & data : collection) {
        double value = 1;
        for (const auto & kernel : m_kernels) {
            value *= kernel.negative_log_likelhood(data.as<const representations::interfaces::ordered_collection_base>());
        }
        log_likelihood += value;
    }
    return log_likelihood;
}
void habits::clustering::algorithm::em::maximize(const representations::interfaces::unordered_collection & collection) {
    #pragma omp parallel for default(none) shared(collection, m_number_of_kernels, m_responsibilities)
    for (int i = 0; i < collection.size(); i++) {
        double local_score = 0;
        for (int k = 0; k < m_number_of_kernels;k++) {
            m_responsibilities(i,k) = m_kernels[k].likelihood(collection[i].as<const representations::interfaces::ordered_collection_base>());
            local_score += m_responsibilities(i,k);
        }
        if (local_score > 1e-8)
            for (int k = 0; k < m_number_of_kernels;k++) m_responsibilities(i,k) /= local_score;
    }
}
void habits::clustering::algorithm::em::expectation(const representations::interfaces::unordered_collection & collection) {
    #pragma omp parallel for default(none) shared(m_kernels, collection, m_responsibilities)
    for (int i =0; i < m_kernels.size(); i++) m_kernels[i].fit(collection,m_responsibilities);
}
void habits::clustering::algorithm::em::initialize(const representations::interfaces::unordered_collection & collection) {
    Eigen::VectorXi x (collection.size());
    x.setLinSpaced(0,collection.size());
    std::shuffle(x.data(),x.data()+x.size(),std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()));
    for (int i = 0; i < m_kernels.size(); i++) {
        m_kernels[i].mean(collection[x(i)].as<const representations::interfaces::ordered_collection_base>());
    }
}
