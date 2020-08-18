#include <habits/segmentation/region_based_segmentation.h>
#include <habits/utils/dataset_name_structure.h>
using namespace habits::segmentation;

region_based_segmentation::region_based_segmentation(const representations::interfaces::unordered_collection &collection)
        : segmentation(collection) {

}
representations::interfaces::segmentation region_based_segmentation::run_strategy(const std::string &name, const representations::interfaces::ordered_collection &trajectory) {
    // for the name, we will want to get the goal locations:
    auto names = utils::dataset_name_structure(name);
    // find data and save it 
}
