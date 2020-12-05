add_executable(do_tests do_tests.cpp calculation_tests.cpp card_deck_tests.cpp holdem_tests.cpp tests.cpp tests.h)
add_executable(do_mccfr_tests mccfr_tests.cpp tests.cpp tests.h do_tests.cpp)
target_link_libraries(do_tests PUBLIC source gtest)
target_link_libraries(do_mccfr_tests PUBLIC source gtest)
