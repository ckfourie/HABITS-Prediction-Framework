#include <habits/core/options.h>
#include <habits/datasets.h>
habits::core::options::options(const std::string &program_name, const std::string &description)
        : service_options(program_name, description) {

}
void habits::core::options::parse_input(int argc, char **argv) {
    add_positional_args();
    service_options::parse_input(argc, argv);
    // load relevant data
    habits::load_dataset(get<std::string>("dataset_name"),get<std::string>("subject_regex"));
}
void habits::core::options::add_positional_args() {
    // create basic options: dataset, subject
    add_positional_argument<std::string>("dataset_name","the name of the dataset to operate on",true);
    add_positional_argument<std::string>("subject_regex","the name of the dataset to operate on",true);
}
void habits::core::options::add_command_argument(const std::string &name, const std::string &description) {
    add_positional_argument<std::string>(name,description,true);
    m_commands.emplace_back(name);
}
std::vector<std::string> habits::core::options::get_commands() {
    std::vector<std::string> command_results;
    for (const std::string & command_name : m_commands) {
        command_results.emplace_back(get<std::string>(command_name));
    }
    return std::move(command_results);
}
