#include <habits/predictors/group_predictors.h>
#include <simple-logger/simple-logger.h>
habits::predictors::group_predictors::group_predictors(const std::vector<interfaces::predictor::predictor_ptr> &predictors) : m_predictors(predictors) {
    // construct predictor map
    m_spatial_predictions.reserve(m_predictors.size());
    for (const auto & ptr : m_predictors) {
        if (m_predictor_map.count(ptr->start_event()) == 0) m_predictor_map.emplace(ptr->start_event(),std::vector<interfaces::predictor::predictor_ptr>());
        m_predictor_map[ptr->start_event()].emplace_back(ptr);
        m_spatial_predictions.emplace_back(std::reference_wrapper<const representations::interfaces::ordered_collection_base>(ptr->spatial_prediction()));
    }
}
void habits::predictors::group_predictors::set_reference_segmentation(const representations::interfaces::segmentation &segmentation) {
    m_segmentation_connection = segmentation.on_change_signal().connect(boost::bind(&habits::predictors::group_predictors::segmentation_update,this,boost::placeholders::_1));
    // trigger a segmentation update
    segmentation_update(segmentation);
}
void habits::predictors::group_predictors::segmentation_update(const representations::interfaces::representation &segmentation) {
    // when we get the segmentation, we need to reattach the predictors to the latest segment
    for (auto & predictor : m_predictors) predictor->set_reference_segmentation(dynamic_cast<const representations::interfaces::ordered_collection_base&>(segmentation));
    // attach the segment update
    const auto & segment = (*(dynamic_cast<const representations::interfaces::segmentation &>(segmentation).cend()-1)).as<const representations::interfaces::segment&>();
    if (m_segment_connection.connected()) m_segment_connection.disconnect();
    m_segment_connection = segment.on_change_signal().connect(boost::bind(&habits::predictors::group_predictors::segment_update,this,boost::placeholders::_1));
}
void habits::predictors::group_predictors::segment_update(const representations::interfaces::representation & target) {
    // segment update should happen after the predictors, so here we just want to update our activity recognition, and produce our spatial predictions etc.
    // signal that this object has updated
    const auto & segment = dynamic_cast<const representations::interfaces::segment&>(target);
    const auto & start_event = segment.begin_index().element();
    m_likelihood_map.clear();
    m_likelihood_predictor_map.clear();
    for (const auto & p : m_predictor_map[start_event]){
         if (m_likelihood_map.count(p->end_event()) == 0) m_likelihood_map.emplace(p->end_event(),0);
         if (m_likelihood_predictor_map.count(p->end_event()) == 0) m_likelihood_predictor_map.emplace(p->end_event(),std::pair<double,interfaces::predictor::predictor_ptr>{0,p});
         m_likelihood_map[p->end_event()]+= p->weighted_likelihood();
         auto & pair_ref = m_likelihood_predictor_map[p->end_event()];
         if (p->weighted_likelihood() > pair_ref.first) {
             pair_ref.first = p->weighted_likelihood();
             pair_ref.second = p;
         }
    }
    double total = 0;
    for (const auto & p : m_likelihood_map) total += p.second;
    // find the most likely predictor
    const auto & pair = std::max_element(m_likelihood_map.begin(),m_likelihood_map.end(),[](const auto & a, const auto & b){return a.second < b.second;});
    const auto & semantic_index = pair->first;
    const double & overall_likelihood = pair->second;
    double likelihood = 0; habits::predictors::interfaces::predictor::predictor_ptr best_predictor;
    std::tie(likelihood,best_predictor) = m_likelihood_predictor_map[semantic_index];
    representations::interfaces::semantic prediction = best_predictor->semantic_temporal_prediction().element();
    m_event_prediction.append(segment.at(segment.size()-1).as<const representations::interfaces::index_element_group_base&>().get_abstract_index_value(),best_predictor->semantic_temporal_prediction().element());
    // update the spatial prediction opacity
    for (const auto & p : m_predictor_map) {
        if (p.first == start_event) {
            for (const auto & pred : p.second) pred->spatial_prediction().opacity(pred->likelihood() / total);
        } else {
            for (const auto & pred : p.second) pred->spatial_prediction().opacity(0.0);
        }
    }
    m_update_signal(*this);
}
