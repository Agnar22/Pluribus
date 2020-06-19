add_executable(do_tests do_tests.cpp calculation_tests.cpp card_deck_tests.cpp holdem_tests.cpp tests.cpp tests.h)
target_link_libraries(do_tests PUBLIC source gtest)
