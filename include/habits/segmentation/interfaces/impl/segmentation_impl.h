namespace habits {
    namespace segmentation {
        namespace interfaces {
            template<typename T> segmentation<T>::segmentation(const representations::interfaces::cluster<representations::interfaces::time_series<T>> &collection) {
                std::vector<std::string> keys = collection.keys();
                m_start.label("start of trajectory");
                m_end.label("end of trajectory");
                for (const std::string & key : keys) {
                    // create segmentation object
                    auto seg = representations::const_segmentation_semantic_ul<representations::interfaces::time_group<T>>(collection.at(key));
                    seg.at(0).begin_index().set_semantic_component(m_start);
                    seg.at(0).end_index().set_semantic_component(m_end);
                    this->insert(key,seg);
                }
            }
        }
    }
}