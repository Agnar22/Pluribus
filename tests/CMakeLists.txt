add_executable(do_holdem_tests do_tests.cpp hand_test_helper.h hand_test_helper.cpp calculation_tests.cpp card_deck_tests.cpp holdem_tests.cpp)
add_executable(do_history_tests do_tests.cpp history_tests.cpp)
target_link_libraries(do_holdem_tests PUBLIC calculations_lib card_lib holdem_lib gtest)
add_test(NAME HOLDEM_TESTS COMMAND do_holdem_tests WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_executable(do_cfr_tests do_tests.cpp cfr_tests.cpp optimal_strategies_tests.h optimal_strategies_tests.cpp)
target_link_libraries(do_cfr_tests PUBLIC cfr_lib kuhn_poker_lib gtest)
add_test(NAME CFR_TESTS COMMAND do_cfr_tests WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_executable(do_mccfr_tests do_tests.cpp mccfr_tests.cpp optimal_strategies_tests.h optimal_strategies_tests.cpp)
target_link_libraries(do_mccfr_tests PUBLIC mccfr_lib kuhn_poker_lib gtest)
add_test(NAME MCCFR_TESTS COMMAND do_mccfr_tests WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(do_history_tests PUBLIC mccfr_lib gtest)
add_test(NAME HISTORY_TESTS COMMAND do_history_tests WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_executable(do_tests do_tests.cpp hand_test_helper.h hand_test_helper.cpp calculation_tests.cpp card_deck_tests.cpp history_tests.cpp holdem_tests.cpp mccfr_tests.cpp optimal_strategies_tests.h optimal_strategies_tests.cpp)
target_link_libraries(do_tests PUBLIC calculations_lib holdem_lib card_lib kuhn_poker_lib mccfr_lib gtest)
