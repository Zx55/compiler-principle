/*
 * LexParser.h
 * Test of OPG Parser.
 * Copyright (c) zx5. All rights reserved.
 */

#define CATCH_CONFIG_MAIN

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "../OPG.h"
#include "../../../include/catch.hpp"

using namespace std;
using namespace Compiler;

const string TEST_FILE_PATH = "compiler-principle/oj/OPG/test/o";
const int TEST_FILE_TOTAL = 2;

string parse(int no) {
    auto inFile = ifstream(TEST_FILE_PATH + to_string(no) + "/in.txt");

    int t;
    OPG grammar;
    string line;
    stringstream ss;

    inFile >> grammar;
    auto parser = Parser(grammar);

    for (inFile >> t, getline(inFile, line); t != 0; --t) {
        getline(inFile, line);
        if (parser.isValiad(line)) {
            ss << "T\n";
        } else {
            ss << "F\n";
        }
    }

    return ss.str();
}

string ans(int no) {
    auto ansFile = ifstream(TEST_FILE_PATH + to_string(no) + "/ans.txt");

    stringstream ss;
    string line;

    while (ansFile >> line) {
        ss << line << "\n";
    }

    return ss.str();
}

TEST_CASE("OPG Parser", "[opg]") {
    for (int i = 1; i <= TEST_FILE_TOTAL; ++i) {
        REQUIRE(parse(i) == ans(i));
    }
}
