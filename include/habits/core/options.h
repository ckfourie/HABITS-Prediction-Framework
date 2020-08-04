#pragma once
#include <service/util/options.h>

namespace habits {
    namespace core {
        class options : public service::util::service_options {
        public:
            options (const std::string & program_name, const std::string & description);
            void parse_input (int argc, char ** argv) override;
            void add_command_argument(const std::string & name, const std::string & description);
            std::vector<std::string> get_commands();
        protected:
            void add_positional_args();
            std::vector<std::string> m_commands;
        };
    }
}