enable_testing()

add_executable(test_all
    test_all.cpp
    test_const_math.cpp
#    test_for_constexpr.cpp
#    test_sequence.cpp
)
target_link_libraries(test_all gtest)
add_test(
    NAME unit_test
    COMMAND test_all
)
