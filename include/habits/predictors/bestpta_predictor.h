#pragma once
#include <habits/predictors/interfaces/predictor.h>
#include <habits/segmentation/interfaces/segmentation.h>
#include <representations/trajectory.h>
#include <gppe/python.h>
#include <eigenhl/odtw.h>
namespace habits {
    namespace predictors {
        class bestpta_predictor: public interfaces::predictor {
        public:
            bestpta_predictor();
            void train (const representations::interfaces::ordered_collection_base & reference_trajectory, const representations::interfaces::unordered_collection & source_trajectories) override;
            void set_target_trajectory(const representations::interfaces::ordered_collection &target) override;
            void update_state(const representations::interfaces::representation &new_data) override;
            void reset_state() override;
            double likelihood() const override;
            const representations::interfaces::ordered_collection_base &spatial_prediction() const override;
            const representations::interfaces::semantic_index &semantic_temporal_prediction() const override;
            void start_event(const representations::interfaces::semantic & start){m_start =start;}
            const representations::interfaces::semantic& start_event() const {return m_start;}
            void end_event(const representations::interfaces::semantic & end){m_end =end;}
            const representations::interfaces::semantic& end_event() const {return m_end;}
            void segmentation_ptr(const habits::segmentation::interfaces::segmentation * ptr){m_segmentation_ptr = ptr;}
            boost::shared_ptr<predictor> empty_clone() const override;
            void assign_parameters_from_kernel (const habits::clustering::algorithm::kernel & kernel) override;
        protected:
            double m_covariance, m_score, m_time_to_completion, m_likelihood, m_weighted_likelihood;
            double m_score_offset, m_weight;
            unsigned long m_alignment_point;
            representations::interfaces::semantic m_start, m_end;
            representations::trajectory3d m_mean;
            representations::interfaces::semantic_index m_semantic_temporal_prediction;
            representations::trajectory3d m_spatial_prediction;
            boost::shared_ptr<gppe::python_class<std::string,std::string>> m_python_object;
            boost::shared_ptr<eigenhl::odtw<double>> m_odtw;
            static bool m_python_module_loaded;
            unsigned long m_point_count;
            // bad, we shouldn't be throwing around raw pointers but will do for now.
            const habits::segmentation::interfaces::segmentation * m_segmentation_ptr;
            void update_callback_segmentation();
        };

    }
}