include_directories(${BigTopicModel_SOURCE_DIR}/src/util)
include_directories(${BigTopicModel_SOURCE_DIR}/src/model)
include_directories(${BigTopicModel_SOURCE_DIR}/src/engine)
add_executable(sort_benchmark sort_benchmark.cpp)
target_link_libraries(sort_benchmark util ${IPP_LIBRARIES} ${TBB_LIBRARIES})
