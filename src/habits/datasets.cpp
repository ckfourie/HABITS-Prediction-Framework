#include <habits/datasets.h>
#include <gppe/on_startup.h>
#include <simple-logger/simple-logger.h>
#include <map>
#include <representations/trajectory.h>
#include <service/datasets.h>
#include <gppe/timing.h>
using namespace representations;
namespace habits {
    // declare dataset load function here
    void load_bolt_placement_dataset(const std::string & subject_regex);
    void load_cube_placement_dataset(const std::string & subject_regex);

    std::map<std::string,std::function<void(const std::string&)>> m_load_map;
    gppe::on_startup<void(void)> datasets_on_startup ([](){
        // add load function or aliases here
        m_load_map.emplace("bolt_placement_dataset""bolt_placement_dataset",&load_bolt_placement_dataset);
        m_load_map.emplace("bolts",&load_bolt_placement_dataset);
        m_load_map.emplace("cube_placement_dataset",&load_cube_placement_dataset);
        m_load_map.emplace("cubes",&load_cube_placement_dataset);
    });

    boost::shared_ptr<interfaces::unordered_collection> m_active_dataset;
    void load_phasespace_dataset(const std::string & dataset_name, const std::string & subject_regex);

    const interfaces::unordered_collection &active_dataset() {
        if (!m_active_dataset) throw std::runtime_error("habits::active_dataset():: need to load a dataset first");
        return *m_active_dataset;
    }
    const interfaces::unordered_collection &load_dataset(const std::string &dataset_name, const std::string &subject_regex) {
        // switch dataset name to load data
        if (m_load_map.count(dataset_name) > 0) m_load_map[dataset_name](subject_regex);
        else throw std::runtime_error("dataset: " + dataset_name + " doesn't have an associated load function");
        return active_dataset();
    }

    void load_bolt_placement_dataset(const std::string & subject_regex){
        load_phasespace_dataset("bolt_placement_dataset",subject_regex);
    }

    void load_cube_placement_dataset(const std::string & subject_regex){
        load_phasespace_dataset("cubes",subject_regex);
    }

    void load_phasespace_dataset(const std::string & dataset_name, const std::string & subject_regex){
        SLOG(trace) << "dataset::load_phasespace_dataset:: loading " << dataset_name << " with subjects " << subject_regex;
        // load marker 1 and 2 data:
        gppe::timer t;
        auto marker1 = service::datasets::read_representation<interfaces::homogeneous_cluster,trajectory3d>(dataset_name+"/" + subject_regex,"/trajectories/.*","marker1");
        auto marker2 = service::datasets::read_representation<interfaces::homogeneous_cluster,trajectory3d>(dataset_name+"/" + subject_regex,"/trajectories/.*","marker2");
        SLOG(trace) << "dataset::load_phasespace_dataset:: retrieved data from server in " << t.elapsed() << "s";
        // filter dropouts
        auto dropout_filter = [](const interfaces::time_group<point<3>> & point)->bool{
            return point.element().x() == -10000 && point.element().y() == -10000 && point.element().z() == -10000;
        };
        double resample_frequency = 20.0;
        t.reset();
        #pragma omp parallel default (none) shared(marker1,marker2,dropout_filter,resample_frequency)
        {
            #pragma omp single
            {
                for (auto it = marker1.begin(); it != marker1.end(); ++it) {
                    #pragma omp task
                    {
                        it.value().as<trajectory3d>().filter_elements(dropout_filter);
                        it.value().as<trajectory3d>().resample(resample_frequency); // lets try 20hz first
                    }
                }
            }
            #pragma omp single
            {
                for (auto it = marker2.begin(); it != marker2.end(); ++it) {
                    #pragma omp task
                    {
                        it.value().as<trajectory3d>().filter_elements(dropout_filter);
                        it.value().as<trajectory3d>().resample(resample_frequency); // lets try 20hz first
                    }
                }
            }
        }
        SLOG(trace) << "dataset::load_phasespace_dataset:: filtered dropouts and resampled in " << t.elapsed() << " seconds, trajectory count = " << marker1.size() + marker2.size();
        m_active_dataset.reset(new trajectory_cluster3d());
        // now insert into map as mean'ed
        t.reset();
        std::vector<std::string> keys = marker1.keys();
        #pragma omp parallel for default(none) shared(keys,marker1,marker2,m_active_dataset)
        for (int i = 0; i < keys.size(); i++) {
            // all trajectories will have /marker1 as a postfix
            std::string key = keys[i].substr(0,keys[i].length()-8);
            // create mean object
            SLOG(debug) << key;
            auto obj = mean(marker1[key + "/marker1"].as<trajectory3d>(),marker2[key+"/marker2"].as<trajectory3d>());
            // emplace in map
            m_active_dataset->move_insert(key,std::move(obj));
        }
        SLOG(trace) << "dataset::load_phasespace_dataset:: calculated means in " << t.elapsed();
    }
}


