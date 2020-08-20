#include <habits/segmentation/state_based_segmentation.h>
#include <habits/utils/dataset_name_structure.h>
using namespace habits::segmentation;

state_based_segmentation::state_based_segmentation(const representations::interfaces::unordered_collection &collection)
        : segmentation(collection) {

}
representations::interfaces::segmentation state_based_segmentation::run_strategy(const std::string &name, const representations::interfaces::ordered_collection &trajectory) {
    // for the name, we will want to get the goal locations:
}
bool state_based_segmentation::try_subject_detector_map(const std::string &name) {
    auto names = utils::dataset_name_structure(name);
    // see if we have goal locations for the name

}
