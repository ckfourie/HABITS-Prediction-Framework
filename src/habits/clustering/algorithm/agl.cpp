#include <habits/clustering/algorithm/agl.h>
#include <simple-logger/simple-logger.h>
habits::clustering::algorithm::agl::agl() {

}
int habits::clustering::algorithm::agl::solve(const representations::interfaces::unordered_collection &collection) {
    m_responsibilities = Eigen::VectorXd::Ones(collection.size(),1).asDiagonal();
    m_responsibilities/= collection.size(); // equally weighted
    m_kernels.clear();
    for (int i = 0; i < collection.size(); i++) m_kernels.emplace_back(kernel(i));
    initialize(collection); // will fill all randomly without replacement
    int old_size = m_responsibilities.rows()+1;
    int size = m_responsibilities.rows(); int count = 0;
    while (old_size > size) {
        old_size = size;
        maximize(collection);
        expectation(collection);
        size = count_clusters();
    }
    // replace kernels with relevant kernels
    std::vector<kernel> new_kernels;
    for (int i = 0; i < m_kernels.size(); i++) {
        if (m_responsibilities(i,i) > 1e-8) {
            new_kernels.emplace_back(std::move(m_kernels[i]));
            // create the kernel indices vector
            std::vector<unsigned long> indices; indices.reserve(m_responsibilities.rows());
            for (int k = 0; k < m_responsibilities.rows(); k++) {
                if (m_responsibilities(k,i) > 1e-8) indices.emplace_back(k);
            }
            new_kernels.back().kernel_indices(indices);
        }
    }
    m_kernels = new_kernels;
}
void habits::clustering::algorithm::agl::maximize(const representations::interfaces::unordered_collection &collection) {
    for (int i = 0; i < collection.size(); i++) {
        if (m_responsibilities(i,i) > 1e-8) {
            // cluster owns value, check other clusters for rewiring
            for (int j = 0; j < collection.size(); j++) {
                if (j != i)
                if (m_responsibilities(j, j) > 1e-8) {
                    // check if within distance of kernel:
                    double dtw_distance = m_kernels[i].mean().dtw_alignment_error(m_kernels[j].mean());
                    dtw_distance/=std::max(m_kernels[i].mean().size(),m_kernels[j].mean().size());
                    if (dtw_distance < m_merge_distance) {
                        // find all values assigned to this cluster and reassign
                        for (int k = 0; k < collection.size(); k++) {
                            if (m_responsibilities(k,j) > 1e-8) {
                                m_responsibilities(k,i) = m_responsibilities(k,j);
                                m_responsibilities(k,j) = 0;
                            }
                        }
                    }
                }
            }
        }
    }
}
int habits::clustering::algorithm::agl::count_clusters() const {
    int size = 0;
    for (int i = 0; i < m_responsibilities.rows(); i++) if (m_responsibilities(i,i) > 1e-8) ++size;
    return size;
}
void habits::clustering::algorithm::agl::expectation(const representations::interfaces::unordered_collection &collection) {
    #pragma omp parallel for default(none) shared(m_kernels, collection, m_responsibilities)
    for (int i =0; i < m_kernels.size(); i++) if (m_responsibilities(i,i) > 1e-8) m_kernels[i].fit(collection,m_responsibilities);
}
