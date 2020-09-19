/***********************************************************************************************************************
*   @file test_live_segmentation.cpp Test Live Segmentation
*   \author ckfourie
*
*   This tests the live segmentation framework to ensure that it matches the output of the standard segmentation.
*   The code will run on bolts by default, but a different dataset can be passed if desired
***********************************************************************************************************************/
#include <habits/core/options.h>
#include <habits/datasets.h>
#include <habits/utils/dataset_name_structure.h>
#include <habits/segmentation/state_based_segmentation.h>
#include <representations/trajectory.h>
#include <service/core/dataserver.h>
#include <service/parameters.h>
int main (int argc, char ** argv) {
    std::string dataset_name = "bolts", subject_regex = "subject_1";
    habits::core::options options("test_live_segmentation","tests the live segmentation framework");
    options.enable_subject_regex();
    options.parse_input(argc,argv);
    dataset_name = options.get("dataset_name");
    subject_regex = options.get("subject_regex");
    habits::load_dataset(dataset_name,subject_regex);
    // calculate the ground truth segmentations
    habits::segmentation::state_based_segmentation groundtruth_segmentation (habits::active_dataset());
    // now step through the dataset and process incrementally:
    bool passed = true;
    for (auto it = habits::active_dataset().begin(); it != habits::active_dataset().end(); ++it) {
        const std::string & trajectory_name = it.key();
        const auto & trajectory = it.value().as<const representations::trajectory3d&>();
        // create a dummy group for autosegmenting
        auto group = representations::trajectory_cluster3d(); group.move_insert(trajectory_name,representations::trajectory3d());
        representations::trajectory3d & stream = group.at(trajectory_name);
        // create the segmentation object
        habits::segmentation::state_based_segmentation test_segmentation (group);
        test_segmentation.incremental_update_interval(5);
        // this will now autoupdate as points are streamed.
        for (const auto & point : trajectory) {
            stream << point;
        }
        // verify that the segmentations match
        if (test_segmentation.at(trajectory_name) == groundtruth_segmentation.at(trajectory_name)){
            SLOG(info) << "[PASSED]: " << trajectory_name << ":" << trajectory;
        } else {
            SLOG(fatal) << "[FAILED]: " << trajectory_name << ":" << trajectory;
            passed = false;
        }
    }
    if (passed) return 0; else return -1;
}