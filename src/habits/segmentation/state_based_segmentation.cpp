#include <habits/segmentation/state_based_segmentation.h>
#include <habits/utils/dataset_name_structure.h>
#include <habits/datasets.h>
#include <simple-logger/simple-logger.h>
#include <representations/point.h>
#include <representations/operations/generic_operations.h>
#include <service/vtkhl/plot2.h>
#include <service/vtkhl/plot3.h>
#include <service/parameters.h>
#include <gppe/file_operations.h>
using namespace habits::segmentation;

state_based_segmentation::state_based_segmentation(const representations::interfaces::unordered_collection &collection)
        : segmentation(collection) {
    // step through collection and run strategy
    if (m_detector_map.empty()) load_detectors_from_goal_information();
    gppe::timer t;
    std::vector<std::string> keys = collection.keys();
    #pragma omp parallel for
    for (int i = 0; i < keys.size(); i++) {
        run_strategy(keys[i], collection.at(keys[i]).as<const representations::interfaces::ordered_collection>());
    }
    SLOG(trace) << "state_based_segmentation:: computed segmentations in " << t.elapsed() << " seconds (trajectory count = " << collection.size() << ")";
}
bool state_based_segmentation::run_strategy(const std::string &name, const representations::interfaces::ordered_collection &trajectory) {
    return try_subject_detector_map(name,trajectory);
}
bool state_based_segmentation::try_subject_detector_map(const std::string &name, const representations::interfaces::ordered_collection & trajectory) {
    auto names = utils::dataset_name_structure(name);
    // check for detectors
    if (m_detector_map.count(names.subject_name) == 0) return false;
    if (m_detector_map[names.subject_name].empty()) return false;
    // for each detector, run the segmentation
    representations::interfaces::segmentation segmentation (trajectory);
    for (auto it = m_detector_map[names.subject_name].cbegin(); it != m_detector_map[names.subject_name].cend(); ++it) {
        auto local_segmentation = it->second->run(trajectory,0);
        segmentation += local_segmentation;
    }
    // filter for short segments
    segmentation.filter_short_segments(12);
    insert(name,segmentation);
    // add callback to the collection here to continue segmentation if more data is added.
    trajectory.on_change_signal().connect(boost::bind(&state_based_segmentation::update_segmentation,this,boost::placeholders::_1,name,m_detector_map[names.subject_name]));
    return true;
}

void state_based_segmentation::update_segmentation(const representations::interfaces::representation &data, const std::string & name, const std::map<std::string,boost::shared_ptr<detectors::state_based_detector>> & local_detectors) {
    auto & trajectory = dynamic_cast<const representations::interfaces::ordered_collection &>(data);
    if (trajectory.size() < 5) return;
    auto & seg_ref = at(name);
    if (trajectory.size() % m_update_interval != 0) return;
//    seg_ref.at(seg_ref.size()-1).as<representations::interfaces::segment>().end_index().index() = trajectory.size();
    /// update segmentation from each detector
    // run the detectors
    for (const auto & detector : local_detectors){
        // get incremental indices vector from detector
        auto indices_vector = detector.second->incremental_run(trajectory);
        for (auto & id : indices_vector) seg_ref.add_segmentation_index(id);
    }
    // filter
    if (seg_ref.at(seg_ref.size()-1).as<const representations::interfaces::segment>().size() > 20) seg_ref.filter_short_segments(12);
}

void state_based_segmentation::load_detectors_from_goal_information() {
    // create the detector map
    // should be subject id -> vector<detector_ptr> ---> run strategy should then run the segmentation on each
    gppe::timer t;
    for (auto it = habits::goal_information().begin(); it!= habits::goal_information().end(); ++it){
        auto names = utils::dataset_name_structure(it.key());
        if (m_detector_map.count(names.subject_name) == 0) m_detector_map.emplace(names.subject_name,std::map<std::string,boost::shared_ptr<detectors::state_based_detector>>());
        if (m_detector_map[names.subject_name].count(names.data_element_name) == 0) {
            // create detector
            boost::shared_ptr<detectors::state_based_detector> detector (new detectors::state_based_detector(names.data_element_name));
            // add a position check
            try {
                double radius = service::parameters::read({gppe::path_join(names.dataset_name,names.subject_name)},gppe::path_join(names.data_element_name,"event_region_radius")).primitive<double>();
                detector->add_zero_order_detection([radius,it](const representations::interfaces::representation & d)->bool {
                    double distance = d.distance(*it);
                    return distance < radius;
                });
            } catch (const std::exception & e) {
                SLOG(error) << e.what();
                throw std::runtime_error("couldn't create position check");
            }
            try {
                double velocity_limit = service::parameters::read({gppe::path_join(names.dataset_name,names.subject_name)},gppe::path_join(names.data_element_name,"velocity_limit")).primitive<double>();
                detector->add_first_order_detection([velocity_limit](const representations::interfaces::representation & d)->bool {
                    double distance = representations::norm(d);
                    return distance < velocity_limit*1400;
                });
            } catch (const std::exception & e) {
                SLOG(error) << e.what();
                throw std::runtime_error("couldn't create velocity check");
            }
            m_detector_map[names.subject_name].emplace(names.data_element_name,detector);
        } else {
            SLOG(warning) << "state_based_segmentation:: multiple entries for " << it.key();
        }
    }
    SLOG(trace) << "state_based_segmentation:: loaded detectors from goal information in " << t.elapsed() << " seconds";
}

