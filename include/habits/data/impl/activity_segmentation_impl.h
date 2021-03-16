namespace habits {
    namespace data {
        template<typename T>
        activity_segmentation<T>::activity_segmentation(const habits::segmentation::interfaces::segmentation<T> & segmentation){
            // step through segmentation and assign segments based on their keys
            std::vector<std::string> keys = segmentation.keys();
            for (const std::string & key : keys) {
                const auto & s = segmentation.at(key);
                for (size_t i = 0; i < s.size(); i++) {
                    // construct key pair:
                    representations::key_semantic_pair skey (s.at(i).begin_index().get_semantic_component(),s.at(i).end_index().get_semantic_component());
                    // check if cluster exists, create if it doesn't
                    if (!this->exists(skey)) this->insert(skey,representations::interfaces::cluster<representations::const_segment_semantic_ul<representations::interfaces::time_group<T>>>());
                    // now add this segment to that cluster:
                    this->at(skey).insert(key,s.at(i));
                }
            }
        }
    }
}