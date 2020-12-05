add_library(source calculations.cpp calculations.h card_deck.cpp card_deck.h holdem.cpp holdem.h)
target_link_libraries(source ${Boost_LIBRARIES} stdc++fs)
add_executable(mccfr_poker_game mccfr.cpp kuhn_poker.cpp kuhn_poker.h game.h)
