#include <habits/core/options.h>
#include <habits/datasets.h>
#include <representations/operations/ordered_collection_operations.h>
int main (int argc, char ** argv) {
    habits::core::options options ("habits_goal_positions","shows the goal positions for a given subject");
    options.parse_input(argc, argv);
    // step through the goal regions
    std::vector<std::string> goals;
    for (auto it = habits::goal_information().begin(); it!= habits::goal_information().end();++it) {
        const std::string & name = it.key();
        goals.emplace_back(name);
    }
    std::sort(goals.begin(),goals.end());
    for (const auto & goal : goals) {
        SLOG(info) << "goal: " << goal << " :: " << habits::goal_information()[goal];
    }
}