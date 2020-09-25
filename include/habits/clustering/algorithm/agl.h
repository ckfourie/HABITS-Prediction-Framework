#pragma once
#include <habits/clustering/algorithm/em.h>
namespace habits {
    namespace clustering {
        namespace algorithm {
            class agl : public em {
            public:
                agl();
                int solve(const representations::interfaces::unordered_collection & collection) override;
                double merge_distance() const {return m_merge_distance;}
                void merge_distance(const double & merge_distance){m_merge_distance = merge_distance;}

            protected:
                void maximize(const representations::interfaces::unordered_collection & collection) override;
                void expectation(const representations::interfaces::unordered_collection & collection) override;
                int count_clusters() const;
                double m_merge_distance = 25.0;
            };
        }
    }
}