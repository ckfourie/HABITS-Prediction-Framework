#include <habits/predictors/sequence_predictor.h>
#include <habits/predictors/bestpta_predictor.h>
habits::predictors::sequence_predictor::sequence_predictor(const representations::interfaces::homogeneous_cluster <representations::interfaces::segmentation> &segmentations) {
    for (int i = 0; i < segmentations.size(); i++) {
        bool found = false;
        for (int index = 0; index < m_sequences.size(); index++) {
            if (m_sequences[index].sequence_match(segmentations[i])) {
                // add to sources
                m_sources[index].emplace_back(segmentations[i]);
                found = true;
                break;
            }
        }
        // not found:
        if (!found) {
            m_sources.emplace_back(std::vector<representations::interfaces::segmentation>{segmentations[i]});
            m_sequences.emplace_back(segmentations[i]);
        }
        m_total_size++;
    }
    // now, for each of these sequences, we must learn the activity durations:
    for (const std::vector<representations::interfaces::segmentation> & source : m_sources) {
        m_durations.emplace_back(Eigen::MatrixXd(source.size(), source.front().size()));
        for (int i = 0; i < source.size(); i++) {
            for (int j = 0; j < source[i].size(); j++) {
                m_durations.back()(i,j) = source[i].at(j).duration();
            }
        }
        // now train a model
        m_models.emplace_back(probp::distributions::gmm (m_durations.back()));
    }
    SLOG(debug) << "learned " << m_models.size() << " models";
}
void habits::predictors::sequence_predictor::set_predictors(const std::vector<habits::predictors::interfaces::predictor::predictor_ptr> &predictors) {
    // set the predictors:
    for (const auto & p : predictors) {
        if (m_predictor_map.count(p->start_event()) == 0) m_predictor_map.emplace(p->start_event(),std::map<representations::interfaces::semantic,std::vector<interfaces::predictor::predictor_ptr>>());
        if (m_predictor_map[p->start_event()].count(p->end_event()) ==0) m_predictor_map[p->start_event()].emplace(p->end_event(),std::vector<interfaces::predictor::predictor_ptr>());
        m_predictor_map[p->start_event()][p->end_event()].emplace_back(p);
    }
    // now replace the sequence predictions with default predictions:
    for (auto & segmentation : m_sequences) {
        // construct the mean sequence:
        bool set = false; boost::shared_ptr<representations::interfaces::ordered_collection> mean_sequence;
        boost::shared_ptr<representations::interfaces::segmentation> replacement_segmentation;
        std::vector<int> sizes; sizes.reserve(segmentation.size());
        for (int i = 0; i <segmentation.size();i++) {
            const representations::interfaces::segment & segment = segmentation[i];
            const representations::interfaces::semantic start = segment.begin_index().element(), end = segment.end_index().element();
            // find the most likely predictor by weight
            boost::shared_ptr<habits::predictors::bestpta_predictor> ptr; double weight = 0;
            if (m_predictor_map.count(start) == 0) throw std::runtime_error("no predictors for given mean trajectory");
            if (m_predictor_map[start].count(end) == 0) throw std::runtime_error("no predictors for given mean trajectory");
            for (const auto & predictor : m_predictor_map[start][end]) {
                auto p = boost::dynamic_pointer_cast<habits::predictors::bestpta_predictor>(predictor);
                if (p->weight() > weight ) {weight = p->weight();ptr = p;}
            }
            if (!set) {
                mean_sequence = boost::dynamic_pointer_cast<representations::interfaces::ordered_collection>(ptr->mean().empty_clone());set = true;
                replacement_segmentation.reset(new representations::interfaces::segmentation(*mean_sequence));
                mean_sequence->append(ptr->mean());
                m_representative_means.emplace_back(mean_sequence);
            }
            mean_sequence->append(ptr->mean());
            sizes.emplace_back(ptr->mean().size());
        }
        // step through and add segmentation points:
        int segment_index = 0;
        for (int i = 0; i <segmentation.size();i++) {
            const representations::interfaces::segment &segment = segmentation[i];
            const representations::interfaces::semantic start = segment.begin_index().element(), end = segment.end_index().element();
            if (end.id() != 0) {
                segment_index += sizes[i];
                replacement_segmentation->add_segmentation_index(representations::interfaces::semantic_index(segment_index,end));
            }
        }
        // set the segmentation to the replacement one
        segmentation = *replacement_segmentation;
    }
}
const representations::interfaces::segmentation& habits::predictors::sequence_predictor::prediction(const representations::interfaces::segmentation &live_segmentation) const {
    // find the main sequence
    int predictor_index = 0; unsigned long max_size = 0;
    for (int index = 0; index < m_sequences.size(); index++) {
        if (m_sequences[index].partial_sequence_match(live_segmentation)) {
            if (m_sources[index].size() > max_size) {
                max_size = m_sources[index].size();
                predictor_index = index;
            }
        }
    }
    if (!m_current_prediction) {
        m_current_prediction.reset(new representations::interfaces::segmentation(m_sequences[0]));
        m_current_prediction->color({0.4,0.4,0.4});
    }
    // reassign current prediction
    *m_current_prediction = m_sequences[predictor_index].subsect(live_segmentation.size()+1,m_sequences[predictor_index].size());
    return *m_current_prediction;
}
void habits::predictors::sequence_predictor::set_reference_segmentation(const representations::interfaces::segmentation &target) {
    target.on_change_signal().connect(boost::bind(&habits::predictors::sequence_predictor::update,this,boost::placeholders::_1));
    update(target);
}
void habits::predictors::sequence_predictor::update(const representations::interfaces::representation &src) {
    // cast to segmentation
    const auto & segmentation = src.as<const representations::interfaces::segmentation>();
    // update prediction
    prediction(segmentation);
}
