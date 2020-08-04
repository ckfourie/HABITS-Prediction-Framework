#include <simple-logger/simple-logger.h>
#include <habits/datasets.h>
#include <habits/core/options.h>
#include <service/vtkhl/plot3.h>
#include <service/vtkhl/plot2.h>
#include <gppe/on_startup.h>

gppe::keyword_function_map<void> command_map;
// prototypes
void plot2 ();
void plot3 ();
void scatter2 ();
void scatter3 ();
gppe::on_startup<void(void)> on_startup([](){
   // add entries to command map
   command_map.emplace("plot2",plot2);
   command_map.emplace("scatter2",scatter2);
   command_map.emplace("plot3",plot3);
   command_map.emplace("scatter3",scatter3);
   command_map.set_default([](const std::string & arg){
      SLOG(fatal) << "unknown command: " << arg;
      SLOG(fatal) << "use -h to see help function";
      SLOG(fatal) << "valid commands = " << command_map.keys();
      exit(-1);
   });
});

int main (int argc, char ** argv) {
    habits::core::options options ("habits_plot","creates a variety of plots");
    options.add_command_argument("plot_type","type of plot [valid options: plot2, plot3]");
    options.parse_input(argc,argv);
    return !command_map(options.get_commands()[0]);
}

void plot2 (){
    service::vtkhl::plot2::plot(habits::active_dataset());
    service::vtkhl::plot2::show(true);
}
void plot3 (){
    service::vtkhl::plot3::plot(habits::active_dataset());
    service::vtkhl::plot3::show(true);
}

void scatter2 (){
    service::vtkhl::plot2::scatter(habits::active_dataset());
    service::vtkhl::plot2::show(true);
}
void scatter3 (){
    service::vtkhl::plot3::scatter(habits::active_dataset());
    service::vtkhl::plot3::show(true);
}