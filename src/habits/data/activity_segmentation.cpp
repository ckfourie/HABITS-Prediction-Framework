#include <habits/data/activity_segmentation.h>
#include <gppe/timing.h>
#include <simple-logger/simple-logger.h>
habits::data::activity_segmentation::activity_segmentation(const habits::segmentation::interfaces::segmentation &segmentation) {
    run(segmentation);
}
void habits::data::activity_segmentation::run(const habits::segmentation::interfaces::segmentation & segmentation) {
    // split into appropriate segments
    gppe::timer t;
    int count = 0;
    for (auto it = segmentation.begin(); it != segmentation.end(); ++it){
        std::string trajectory_name = it.key();
        const representations::interfaces::segmentation & s = it.value().as<const representations::interfaces::segmentation>();
        for (auto i = s.begin(); i != s.end(); ++i){
            const representations::interfaces::segment & segment = i->as<const representations::interfaces::segment>();
            insert(segment.semantic_bounds(),trajectory_name,segment);
            count++;
        }
    }
    SLOG(trace) << "activity_segmentation:: sorted segments in " << t.elapsed() << "s";
    SLOG(debug) << "count = " << count;
}
