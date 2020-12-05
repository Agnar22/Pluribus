target_link_libraries(source ${Boost_LIBRARIES} stdc++fs)
add_library(source calculations.cpp calculations.h card_deck.cpp card_deck.h holdem.cpp holdem.h mccfr.cpp game.h kuhn_poker.cpp kuhn_poker.h)
