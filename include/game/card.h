#pragma once
#include <stdint.h>

typedef uint8_t Suit;
enum {
    SPADES,
    HEARTS,
    DIAMONDS,
    CLUBS,
};

typedef uint8_t Rank;
enum {
    ACE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    TEN,
    JACK,
    QUEEN,
    KING,
};

typedef uint8_t Card;
enum {
    NONE = 0,

    // SPADES
    ACE_SPADES,
    TWO_SPADES,
    THREE_SPADES,
    FOUR_SPADES,
    FIVE_SPADES,
    SIX_SPADES,
    SEVEN_SPADES,
    EIGHT_SPADES,
    NINE_SPADES,
    TEN_SPADES,
    JACK_SPADES,
    QUEEN_SPADES,
    KING_SPADES,

    // HEARTS
    ACE_HEARTS,
    TWO_HEARTS,
    THREE_HEARTS,
    FOUR_HEARTS,
    FIVE_HEARTS,
    SIX_HEARTS,
    SEVEN_HEARTS,
    EIGHT_HEARTS,
    NINE_HEARTS,
    TEN_HEARTS,
    JACK_HEARTS,
    QUEEN_HEARTS,
    KING_HEARTS,

    // DIAMONDS
    ACE_DIAMONDS,
    TWO_DIAMONDS,
    THREE_DIAMONDS,
    FOUR_DIAMONDS,
    FIVE_DIAMONDS,
    SIX_DIAMONDS,
    SEVEN_DIAMONDS,
    EIGHT_DIAMONDS,
    NINE_DIAMONDS,
    TEN_DIAMONDS,
    JACK_DIAMONDS,
    QUEEN_DIAMONDS,
    KING_DIAMONDS,

    // CLUBS
    ACE_CLUBS,
    TWO_CLUBS,
    THREE_CLUBS,
    FOUR_CLUBS,
    FIVE_CLUBS,
    SIX_CLUBS,
    SEVEN_CLUBS,
    EIGHT_CLUBS,
    NINE_CLUBS,
    TEN_CLUBS,
    JACK_CLUBS,
    QUEEN_CLUBS,
    KING_CLUBS,

    // Back of card
    BACK,
};

inline Suit get_suit(Card card) { return (Suit)((card - 1) / 13); }

inline Rank get_rank(Card card) { return (Rank)((card - 1) % 13); }

inline Card get_card(Rank rank, Suit suit) { return (Card)(1 + suit * 13 + rank); }

