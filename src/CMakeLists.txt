add_library(source calculations.cpp calculations.h card_deck.cpp card_deck.h holdem.cpp holdem.h)
target_link_libraries(source ${Boost_LIBRARIES} stdc++fs)
add_executable(kuhn_poker_game kuhn_poker.cpp)
