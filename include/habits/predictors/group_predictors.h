#pragma once
#include <habits/predictors/interfaces/predictor.h>
#include <representations/interfaces/segmentation.h>
#include <representations/trajectory.h>
namespace habits {
    namespace predictors {
        class group_predictors {
        public:
            group_predictors(const std::vector<interfaces::predictor::predictor_ptr> & predictors);
            void set_reference_segmentation(const representations::interfaces::segmentation & segmentation);
            boost::signals2::signal<void (const group_predictors &)>& update_signal() const {return m_update_signal;}
            const representations::trajectory1s & event_prediction() const {return m_event_prediction;}
            const std::vector<std::reference_wrapper<const representations::interfaces::ordered_collection_base>> & spatial_predictions() const {return m_spatial_predictions;}
        protected:
            std::vector<interfaces::predictor::predictor_ptr> m_predictors;
            void segmentation_update(const representations::interfaces::representation & segmentation);
            void segment_update(const representations::interfaces::representation & target);
            boost::signals2::connection m_segmentation_connection, m_segment_connection;
            mutable boost::signals2::signal<void (const group_predictors &)> m_update_signal;
            std::map<representations::interfaces::semantic,std::vector<interfaces::predictor::predictor_ptr>> m_predictor_map;
            std::map<representations::interfaces::semantic,double> m_likelihood_map;
            std::map<representations::interfaces::semantic,std::pair<double,interfaces::predictor::predictor_ptr>> m_likelihood_predictor_map;
            representations::trajectory1s m_event_prediction;
            std::vector<std::reference_wrapper<const representations::interfaces::ordered_collection_base>> m_spatial_predictions;
        };
    }
}