# Common utilities used by multiple targets
# This is a header-only "library" providing shared includes like logging and colors.
add_library(common_headers INTERFACE)

target_include_directories(common_headers INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
