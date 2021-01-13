#pragma once
#include <representations/interfaces/homogeneous_cluster.h>
#include <representations/interfaces/segmentation.h>
#include <probp/distributions/gmm.h>
#include <habits/predictors/interfaces/predictor.h>
namespace habits {
    namespace predictors {
        class sequence_predictor {
        public:
            explicit sequence_predictor (const representations::interfaces::homogeneous_cluster<representations::interfaces::segmentation> & segmentations);
            void set_predictors (const std::vector<habits::predictors::interfaces::predictor::predictor_ptr> & predictors);
            const representations::interfaces::segmentation& prediction (const representations::interfaces::segmentation & live_segmentation) const;
            void set_reference_segmentation(const representations::interfaces::segmentation & target);
            // not safe, should fix later
            const representations::interfaces::segmentation& current_prediction() const {return *m_current_prediction;}
        protected:
            void update(const representations::interfaces::representation & src);
            std::vector<std::vector<representations::interfaces::segmentation>> m_sources;
            std::vector<representations::interfaces::segmentation> m_sequences;
            mutable boost::shared_ptr<representations::interfaces::segmentation> m_current_prediction;
            std::vector<boost::shared_ptr<representations::interfaces::ordered_collection_base>> m_representative_means;
            unsigned long m_total_size = 0;
            boost::signals2::connection m_reference_connection;
            std::vector<Eigen::MatrixXd> m_durations;
            std::vector<probp::distributions::gmm> m_models;
            std::map<representations::interfaces::semantic, std::map<representations::interfaces::semantic, std::vector<interfaces::predictor::predictor_ptr>>> m_predictor_map;
        };
    }
}