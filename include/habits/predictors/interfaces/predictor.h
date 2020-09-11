#pragma once
#include <representations/interfaces/collection.h>
namespace habits {
    namespace predictors {
        namespace interfaces {
            // we define a standard predictor as one that acts on a start event, and attempts to predict a specific event
            // based on the trajectory segmentation
            class predictor {
            public:
                // we want predictions, obviously...
                // and we need to pass it information somehow...
                // this should operate through a callback though, when the source trajectory gets modified... but actually, we care about the segmentation
                // this should load the data
                virtual void set_target_trajectory(const representations::interfaces::ordered_collection & target);
                virtual void update_state (const representations::interfaces::representation & new_data) = 0;
                // get the spatial prediction. note that this should be a reference to an internal variable
            protected:

            };
        }
    }
}