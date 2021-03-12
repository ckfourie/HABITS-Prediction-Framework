namespace habits {
    namespace segmentation {
        namespace interfaces {
            template<typename T> segmentation<T>::segmentation(const representations::interfaces::cluster<representations::interfaces::time_series<T>> &collection) {
                std::vector<std::string> keys = collection.keys();
                for (const std::string & key : keys) {
                    // create segmentation object
                    this->insert(key,representations::const_segmentation_semantic_ul<representations::interfaces::time_group<T>>(collection.at(key)));
                }
            }
        }
    }
}