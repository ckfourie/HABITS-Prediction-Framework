#include <habits/predictors/bestpta_predictor.h>
#include <representations/interfaces/collection.h>
#include <representations/interfaces/semantic.h>
#include <service/parameters.h>
#include <eigenhl/to_std.h>
bool habits::predictors::bestpta_predictor::m_python_module_loaded = false;

void habits::predictors::bestpta_predictor::set_target_trajectory(const representations::interfaces::ordered_collection &target) {
    // tracking trajectory: set callback here.
    // will probably need to change to track the segmentation
}
void habits::predictors::bestpta_predictor::update_state(const representations::interfaces::representation &new_data) {

}
void habits::predictors::bestpta_predictor::reset_state() {
    m_point_count = 0;
    if (m_python_object) m_python_object->void_method("reset");
    if (m_odtw) m_odtw->clear();
}
double habits::predictors::bestpta_predictor::likelihood() const {
    return m_likelihood;
}
const representations::interfaces::ordered_collection_base &habits::predictors::bestpta_predictor::spatial_prediction() const {
    return m_spatial_prediction;
}
const representations::interfaces::semantic_index &habits::predictors::bestpta_predictor::semantic_temporal_prediction() const {
    return m_semantic_temporal_prediction;
}
habits::predictors::bestpta_predictor::bestpta_predictor() {
    if (!m_python_module_loaded) {
        gppe::import_modules("bestpta.wrapbpta");
        m_python_module_loaded = true;
    }
}
void habits::predictors::bestpta_predictor::train(const representations::interfaces::ordered_collection_base &reference_trajectory, const representations::interfaces::unordered_collection &source_trajectories) {
    std::string dataset_name = service::parameters::globals::read("/predictors/bpta/config/dataset_name").primitive<std::string>();
    std::string config_name = service::parameters::globals::read("/predictors/bpta/config/configuration_name").primitive<std::string>();
    m_mean = dynamic_cast<const representations::trajectory3d &>(reference_trajectory);
    Eigen::MatrixXd mean_projection = m_mean.project3d().transpose();
    std::vector<std::vector<double>> mean_trajectory = eigenhl::to_vector(mean_projection);
    // use source trajectories to set start and end event :
    const auto & segment = source_trajectories.begin().value().as<const representations::interfaces::segment>();
    m_start = segment.begin_index().element();
    m_end = segment.end_index().element();
    if (source_trajectories.size() > 12) {
        SLOG(trace) << "training bestpta predictor for " << m_start << "->" << m_end << ": trajectories.size() = " << source_trajectories.size();
        // train our best pta object
        m_python_object.reset(new gppe::python_class<std::string,std::string>("WrappedBPTA","bestpta.wrapbpta",dataset_name,config_name));
        // convert trajectories:
        std::vector<std::vector<std::vector<double>>> sources(source_trajectories.size());
        for (int i = 0; i < source_trajectories.size(); i++) {
            Eigen::MatrixXd projection = source_trajectories[i].project3d().transpose();
            sources[i] = eigenhl::to_vector(projection);
        }
        m_python_object->method<std::vector<std::vector<double>>,std::vector<std::vector<std::vector<double>>>>("train",mean_trajectory,sources);
//        m_timing_prediction_appropriate = m_bpta_python_object->void_method("motion_segments_found").result<bool>();
//        SLOG(trace) << "\tbpta_predictor:: timing prediction appropriate = " << m_timing_prediction_appropriate;
//        clear();
    } else {
        SLOG(trace) << "training odtw predictor for " << m_start << "->" << m_end << ": trajectories.size() = " << source_trajectories.size();
        m_odtw.reset(new eigenhl::odtw<double>(mean_projection));
    }
}

void habits::predictors::bestpta_predictor::update_callback_segmentation() {
    // update happens here. we assume the segmentation pointer has been set, and retrieve the latest info from that.
    const auto & segment = (m_segmentation_ptr->at(0).end()-1)->as<const representations::interfaces::segment>();
    // check the start event against the begin index of this segment
    if (m_start == segment.begin_index().element()) {
        // find the new data
        if (segment.size() > m_point_count) {
            // load data into matrix
            Eigen::MatrixXd src = segment.project3d();
            Eigen::MatrixXd new_data = src.block(0,m_point_count,3,segment.size()-m_point_count).transpose();
            m_point_count = segment.size();
            if (m_python_object) {
                m_python_object->method<std::vector<std::vector<double>>>("update", eigenhl::to_vector(new_data));
                double mu, covariance; bool success;
                std::tie(m_time_to_completion, mu, covariance,success) = m_python_object->void_method("get_time_to_completion").tuple_result<int, double, double, bool>();
                if (!success) {
                    m_score = 1e5; // should drive likelihood to zero?
                } else {
                    m_alignment_point = m_mean.size() - m_time_to_completion;
                    representations::interfaces::segment mean_segment (m_mean, representations::interfaces::semantic_index(0,0),representations::interfaces::semantic_index(m_alignment_point,0));
                    m_score = mean_segment.dtw_alignment_error(segment)/((double) m_point_count);
                }
            } else {
                m_odtw->update(new_data);
                m_alignment_point = m_odtw->alignment_point();
                m_time_to_completion = m_mean.size() - m_alignment_point;
                m_score = m_odtw->score() / ((double) m_point_count);
            }
            double relative_score = m_score-m_score_offset; if (relative_score < 0) relative_score = 0;
//            m_likelihood = 1-tanh(log(1+exp(relative_score/sqrt(m_covariance))));
            m_likelihood = sqrt(2)/(sqrt(m_covariance*M_PI))*exp(-pow(relative_score,2)/(2*m_covariance));
            m_weighted_likelihood = m_likelihood * m_weight;
        }
    } else {
        reset_state();
    }
}
boost::shared_ptr<habits::predictors::interfaces::predictor> habits::predictors::bestpta_predictor::empty_clone() const {
    return boost::shared_ptr<predictor>(new bestpta_predictor());
}
void habits::predictors::bestpta_predictor::assign_parameters_from_kernel(const habits::clustering::algorithm::kernel &kernel) {
    m_covariance = kernel.covariance();
    m_score_offset = kernel.residual_average();
}
