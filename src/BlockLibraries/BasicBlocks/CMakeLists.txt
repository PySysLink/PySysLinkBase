# Define a library
add_library(BasicBlocks STATIC
    Constant.cpp
    Display.cpp
    Sumator.cpp 
)

target_include_directories(BasicBlocks PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(BasicBlocks PRIVATE PySysLinkBase)
target_link_libraries(BasicBlocks PRIVATE BasicCpp)

# Include directories for header files
target_include_directories(BasicBlocks PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})