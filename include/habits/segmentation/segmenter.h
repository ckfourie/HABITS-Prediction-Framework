#pragma once
#include <representations/interfaces/collection.h>
#include <representations/interfaces/segmentation.h>
#include <unordered_map>
namespace habits {
    namespace segmentation {
        class segmenter {
        public:
            typedef std::unordered_map<std::string,representations::interfaces::segmentation> map_type;
            explicit segmenter(const representations::interfaces::unordered_collection & collection);
            // expose iterators 
            map_type::iterator begin(){return m_segmentations.begin();}
            map_type::const_iterator begin() const {return m_segmentations.begin();}
            map_type::iterator end(){return m_segmentations.end();}
            map_type::const_iterator end() const {return m_segmentations.end();}
        protected:
            std::reference_wrapper<const representations::interfaces::unordered_collection> m_collection_reference;
            map_type m_segmentations;
        };
    }
}