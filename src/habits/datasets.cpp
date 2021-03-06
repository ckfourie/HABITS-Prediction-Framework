#include <habits/datasets.h>
#include <gppe/on_startup.h>
#include <simple-logger/simple-logger.h>
#include <map>
#include <representations/trajectory.h>
#include <service/datasets.h>
#include <service/parameters.h>
#include <gppe/timing.h>
#include <representations/collection_operations.h>
#include <service/vtkhl/plot2.h>
//#include <representations/operations/ordered_collection_operations.h>
using namespace representations;
namespace habits {
    // declare dataset load function here
    void load_bolt_placement_dataset(const std::string & subject_regex, const std::string & trajectory_regex);
    void load_cube_placement_dataset(const std::string & subject_regex, const std::string & trajectory_regex);

    std::map<std::string,std::function<void(const std::string&,const std::string&)>> m_load_map;
    gppe::on_startup<void(void)> datasets_on_startup ([](){
        // add load function or aliases here
        m_load_map.emplace("bolt_placement_dataset",&load_bolt_placement_dataset);
        m_load_map.emplace("bolts",&load_bolt_placement_dataset);
        m_load_map.emplace("cube_placement_dataset",&load_cube_placement_dataset);
        m_load_map.emplace("cubes",&load_cube_placement_dataset);
    });

    boost::shared_ptr<interfaces::unordered_collection> m_active_dataset;
    boost::shared_ptr<interfaces::unordered_collection> m_goal_information;
    void load_phasespace_dataset(const std::string & dataset_name, const std::string & subject_regex, const std::string & trajectory_regex);

    const interfaces::unordered_collection &active_dataset() {
        if (!m_active_dataset) throw std::runtime_error("habits::active_dataset():: need to load a dataset first");
        return *m_active_dataset;
    }
    const interfaces::unordered_collection &load_dataset(const std::string &dataset_name, const std::string &subject_regex, const std::string & trajectory_regex) {
        // switch dataset name to load data
        if (m_load_map.count(dataset_name) > 0) m_load_map[dataset_name](subject_regex,trajectory_regex);
        else throw std::runtime_error("dataset: " + dataset_name + " doesn't have an associated load function");
        return active_dataset();
    }
    const representations::interfaces::unordered_collection &goal_information() {
        if (!m_goal_information) {
            if (!m_active_dataset) throw std::runtime_error("habits::active_dataset():: need to load a dataset first");
            else throw std::runtime_error("no goal information available for dataset");
        }
        return *m_goal_information;
    }

    void load_bolt_placement_dataset(const std::string & subject_regex, const std::string & trajectory_regex){
        load_phasespace_dataset("bolt_placement_dataset",subject_regex,trajectory_regex);
        // save the bestpta configuration
        service::parameters::globals::save_locally("/predictors/bpta/config/dataset_name",std::string("bolts"));
        service::parameters::globals::save_locally("/predictors/bpta/config/configuration_name",std::string("default"));
    }

    void load_cube_placement_dataset(const std::string & subject_regex, const std::string & trajectory_regex){
        load_phasespace_dataset("cubes",subject_regex,trajectory_regex);
        // save the bestpta configuration
        service::parameters::globals::save_locally("/predictors/bpta/config/dataset_name",std::string("cubes"));
        service::parameters::globals::save_locally("/predictors/bpta/config/configuration_name",std::string("default"));
    }

    void load_phasespace_dataset(const std::string & dataset_name, const std::string & subject_regex, const std::string & trajectory_regex){
        SLOG(trace) << "dataset::load_phasespace_dataset:: loading " << dataset_name << " with subjects " << subject_regex;
        double equivalence_threshold = representations::interfaces::representation::get_equivalence_threshold();
        // load marker 1 and 2 data:
        gppe::timer t;
        std::mutex lock;
        auto marker1 = service::datasets::read_time_series<interfaces::cluster,time_series3>(dataset_name+"/" + subject_regex,trajectory_regex,"marker1");
        auto marker2 = service::datasets::read_time_series<interfaces::cluster,time_series3>(dataset_name+"/" + subject_regex,trajectory_regex,"marker2");
        SLOG(trace) << "dataset::load_phasespace_dataset:: retrieved data from server in " << t.elapsed() << "s";
        // filter dropouts
        auto dropout_filter = [equivalence_threshold](const interfaces::time_group<point<3>> & point)->bool{
            return abs(point.element().x() - -10000.0) < equivalence_threshold && abs(point.element().y() - -10000.0) < equivalence_threshold && abs(point.element().z() - -10000.0) < equivalence_threshold;
        };
        double resample_frequency = 20.0;
        t.reset();
        // get keys
        std::vector<std::string> keys_marker1 = marker1.keys(), keys_marker2 = marker2.keys();
        #pragma omp parallel for default (none) shared(keys_marker1,marker1,dropout_filter,resample_frequency)
        for (size_t i = 0; i < keys_marker1.size(); i++) {
            marker1[keys_marker1[i]].filter_elements(dropout_filter);
            marker1[keys_marker1[i]].resample(resample_frequency);
        }
        #pragma omp parallel for default (none) shared(keys_marker2,marker2,dropout_filter,resample_frequency)
        for (size_t i = 0; i < keys_marker2.size(); i++) {
            marker2[keys_marker2[i]].filter_elements(dropout_filter);
            marker2[keys_marker2[i]].resample(resample_frequency);
        }
        SLOG(trace) << "dataset::load_phasespace_dataset:: filtered dropouts and resampled in " << t.elapsed() << " seconds, trajectory count = " << marker1.size() + marker2.size();
        m_active_dataset.reset(new time_series_cluster3d());
        // now insert into map as mean'ed
        t.reset();
        std::vector<std::string> keys = marker1.keys();
        #pragma omp parallel for default(none) shared(keys,marker1,marker2,m_active_dataset,lock)
        for (size_t i = 0; i < keys.size(); i++) {
            // all trajectories will have /marker1 or /marker2 as a postfix
            std::string key = keys[i].substr(0,keys[i].length()-8);
            // create mean object
            auto obj = mean_trajectory(marker1[key + "/marker1"],marker2[key+"/marker2"]);
            std::lock_guard<std::mutex> lk (lock);
            m_active_dataset->insert(key,obj);
        }
        SLOG(trace) << "dataset::load_phasespace_dataset:: calculated means in " << t.elapsed();
        // load goal information
        m_goal_information.reset(new point_cluster3d());
        auto goal_dropout_filter = [equivalence_threshold](const point<3> & point)->bool{
                return abs(point.x() - -10000.0) < equivalence_threshold  && abs(point.y() - -10000.0) < equivalence_threshold && abs(point.z() - -10000.0) < equivalence_threshold;
        };
        auto goal_marker1 = service::datasets::read_representation<interfaces::cluster,point_series3>(dataset_name+"/" + subject_regex,"/goal/.*","marker1");
        auto goal_marker2 = service::datasets::read_representation<interfaces::cluster,point_series3>(dataset_name+"/" + subject_regex,"/goal/.*","marker2");
        keys_marker1 = goal_marker1.keys(), keys_marker2 = goal_marker2.keys();
        #pragma omp parallel for default (none) shared(keys_marker1,goal_marker1,goal_dropout_filter)
        for (size_t i = 0; i < keys_marker1.size(); i++) {
            goal_marker1[keys_marker1[i]].filter_elements(goal_dropout_filter);
        }
        #pragma omp parallel for default (none) shared(keys_marker2,goal_marker2,goal_dropout_filter)
        for (size_t i = 0; i < keys_marker2.size(); i++) {
            goal_marker2[keys_marker2[i]].filter_elements(goal_dropout_filter);
        }
        // now insert into map as mean'ed
        t.reset();
        keys = goal_marker1.keys();
//        #pragma omp parallel for default(none) shared(keys,goal_marker1,goal_marker2,m_goal_information,lock)
        for (size_t i = 0; i < keys.size(); i++) {
            // all trajectories will have /marker1 as a postfix
            std::string key = keys[i].substr(0,keys[i].length()-8);
            // create mean object
            auto m = mean_trajectory(goal_marker1[key+"/marker1"],goal_marker2[key+"/marker2"]);
            if (m.size() == 0) {
                SLOG(error) << "invalid goal position for key=" << key;
                continue;
            }
            auto obj = mean_value(m);
            // emplace in map
            std::lock_guard<std::mutex> lk (lock);
            m_goal_information->insert(key,obj);
        }
    }
}


