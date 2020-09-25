#pragma once
#include <representations/interfaces/collection.h>
#include <representations/interfaces/semantic.h>
#include <habits/clustering/algorithm/kernel.h>
namespace habits {
    namespace predictors {
        namespace interfaces {
            // we define a standard predictor as one that acts on a start event, and attempts to predict a specific event
            // based on the trajectory segmentation
            class predictor {
            public:
                typedef boost::shared_ptr<predictor> predictor_ptr;
                // generic operations
                virtual void train (const representations::interfaces::ordered_collection_base & reference_trajectory, const representations::interfaces::unordered_collection & source_trajectories) = 0;
                virtual void set_target_trajectory(const representations::interfaces::ordered_collection & target) = 0;
                virtual void update_state (const representations::interfaces::representation & new_data) = 0;
                virtual void reset_state() = 0;
                virtual boost::shared_ptr<predictor> empty_clone() const = 0;
                virtual void assign_parameters_from_kernel (const habits::clustering::algorithm::kernel & kernel) = 0;
                // inference
                virtual double likelihood() const = 0;
                virtual const representations::interfaces::ordered_collection_base & spatial_prediction() const = 0;
                virtual const representations::interfaces::semantic_index& semantic_temporal_prediction () const = 0;
                // note that a segment is really a semantic, temporal, spatial prediction...


            };
        }
    }
}