#include <unordered_map>
// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "../Poker/headers/poker.h"
#include "../headers/vars.h"

namespace pd
{
    std::unordered_map<int,int> FromYoloToVal = {
        { (int)pd::Indices::two, pkr::CardValue::two },
        { (int)pd::Indices::three, pkr::CardValue::three },
        { (int)pd::Indices::four, pkr::CardValue::four },
        { (int)pd::Indices::five, pkr::CardValue::five },
        { (int)pd::Indices::six, pkr::CardValue::six },
        { (int)pd::Indices::seven, pkr::CardValue::seven },
        { (int)pd::Indices::eight, pkr::CardValue::eight },
        { (int)pd::Indices::nine, pkr::CardValue::nine },
        { (int)pd::Indices::ten, pkr::CardValue::ten },
        { (int)pd::Indices::jack, pkr::CardValue::jack },
        { (int)pd::Indices::queen, pkr::CardValue::queen },
        { (int)pd::Indices::king, pkr::CardValue::king },
        { (int)pd::Indices::ace, pkr::CardValue::ace }
    };

    std::unordered_map<int,int> FromYoloToSuit =  {
        { (int)pd::Indices::spade, pkr::CardSuit::spade },
        { (int)pd::Indices::club, pkr::CardSuit::club },
        { (int)pd::Indices::diamond, pkr::CardSuit::diamond },
        { (int)pd::Indices::heart, pkr::CardSuit::heart }
    };
}