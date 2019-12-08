/*
 * RDPTest.cpp
 * Test of RDP.
 * Copyright (c) zx5. All rights reserved.
 */

#define CATCH_CONFIG_MAIN

#include <iostream>
#include <fstream>
#include <string>
#include "../../../include/catch.hpp"
#include "../RDP.h"

using namespace std;
using namespace Compiler;

const string TEST_FILE_PATH = "compiler-principle/oj/Parser/test/p";
const int TEST_FILE_TOTAL = 5;

bool parse(int no) {
    auto inFile = ifstream(TEST_FILE_PATH + to_string(no) + "/in.txt");
    auto parser = RDP(inFile);
    return parser.parse();
}

bool ans(int no) {
    auto ansFile = ifstream(TEST_FILE_PATH + to_string(no) + "/ans.txt");
    string res = "";
    ansFile >> res;
    return res == "T";
}

TEST_CASE("Recursive Descent Parser", "[rdp]") {
    for (int i = 1; i <= TEST_FILE_TOTAL; ++i) {
        REQUIRE(parse(i) == ans(i));
    }
}
