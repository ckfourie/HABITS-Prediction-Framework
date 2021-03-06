#pragma once
#include <service/util/options.h>

namespace habits {
    namespace core {
        class options : public service::util::service_options {
        public:
            options (const std::string & program_name, const std::string & description);
            void parse_input (int argc, char ** argv) override;
            bool enable_subject_regex(){m_subject_regex = true; return m_subject_regex;}
            bool enable_trajectory_regex(){m_trajectory_regex = true; return m_trajectory_regex;}
            void add_command_argument(const std::string & name, const std::string & description);
            std::vector<std::string> get_commands();
        protected:
            void add_positional_args();
            std::vector<std::string> m_commands;
            bool m_subject_regex = false, m_trajectory_regex = false;
        };
    }
}