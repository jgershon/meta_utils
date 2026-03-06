enable_testing()

include(GoogleTest)
include(FetchContent)

FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.17.0
)

FetchContent_MakeAvailable(googletest)

add_executable(test_all
    test_all.cpp
    test_concepts.cpp
    # test_math.cpp
    test_meta_func.cpp
    test_fixed_string.cpp
    # test_for_constexpr.cpp
    # test_overload.cpp
    # test_sequence.cpp
    # test_tuple.cpp
    # test_type_math.cpp
    # test_variant.cpp
)
target_link_libraries(test_all meta_utils gtest gtest_main)
gtest_discover_tests(test_all)
