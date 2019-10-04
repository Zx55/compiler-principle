/*
 * AutomataTest.cpp
 * Test of Automata.
 * Copyright (c) zx5. All rights reserved.
 */

#define CATCH_CONFIG_MAIN

#include <iostream>
#include <fstream>
#include <string>
#include "../../../include/catch.hpp"
#include "../Automata.h"

using namespace std;
using namespace Automata;

const string TEST_FILE_PATH = "compiler-principle/oj/Automata/test/";
const int TEST_MINIMIZE_FILE_TOTAL = 6;
const int TEST_DETERMIN_FILE_TOTAL = 2;
const int TEST_EQUAL_FILE_TOTAL = 4;

DFA mini(int no) {
    auto inFile = ifstream(TEST_FILE_PATH + "m" + to_string(no) + "/in.txt");
    auto dfa = DFA();
    inFile >> dfa;
    return dfa.minimize();
}

DFA deter(int no) {
    auto inFile = ifstream(TEST_FILE_PATH + "d" + to_string(no) + "/in.txt");
    auto nfa = NFA();
    inFile >> nfa;
    return nfa.determine();
}

DFA ansDFA(int no, string &&type) {
    auto ansFile = ifstream(TEST_FILE_PATH + type + to_string(no) + "/ans.txt");
    auto dfa = DFA();
    ansFile >> dfa;
    return dfa;
}

bool equal(int no) {
    auto inFile = ifstream(TEST_FILE_PATH + "e" + to_string(no) + "/in.txt");
    auto nfa1 = NFA(), nfa2 = NFA();
    inFile >> nfa1 >> nfa2;
    return nfa1.toMinimizedDFA() == nfa2.toMinimizedDFA();
}

bool ansEqu(int no) {
    auto ansFile = ifstream(TEST_FILE_PATH + "e" + to_string(no) + "/ans.txt");
    string ans = "";
    ansFile >> ans;
    return ans == "Yes";
}

TEST_CASE("Automata algorithm", "[automata]") {
    SECTION("Convert NFA to DFA") {
        for (int i = 1; i <= TEST_DETERMIN_FILE_TOTAL; ++i) {
            REQUIRE((deter(i) == ansDFA(i, string("d"))) == true);
        }
    }

    SECTION("Minimize DFA") {
        for (int i = 1; i <= TEST_MINIMIZE_FILE_TOTAL; ++i) {
            REQUIRE((mini(i) == ansDFA(i, string("m"))) == true);
        }
    }

    SECTION("Equivalence of two DFA") {
        for (int i = 1; i <= TEST_EQUAL_FILE_TOTAL; ++i) {
            REQUIRE(equal(i) == ansEqu(i));
        }
    }
}
