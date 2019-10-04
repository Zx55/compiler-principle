/*
 * LexParserTest.h
 * Test of Lexical Parser.
 * Copyright (c) zx5. All rights reserved.
 */

#define CATCH_CONFIG_MAIN

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "../LexParser.h"
#include "../../../include/catch.hpp"

using namespace std;
using namespace Compiler;

const string TEST_FILE_PATH = "compiler-principle/oj/LexParser/test/l";
const int TEST_FILE_TOTAL = 8;

string parse(int no) {
    auto inFile = ifstream(TEST_FILE_PATH + to_string(no) + "/in.txt");

    auto parser = LexParser(inFile);
    ostringstream ss;

    while (true) {
        auto result = parser.parseNext();

        switch (result.symbol) {
            case Symbol::SEOF:
                goto done;
            case Symbol::IDENTIFIER:
            case Symbol::INTEGER:
            case Symbol::INCOMPLETECOMMENT: {
                ss << result.symbol << ' ';
                result.info == "" ? (ss << result.num) :
                    (ss << result.info);
                ss << '\n';
                break;
            }
            case Symbol::COMMENT:
                break;
            case Symbol::UNDEFINED: {
                ss << -1 << ' ' << result.info << '\n';
                goto done;
            }
            default: {
                ss << result.symbol << '\n';
            }
        }
    }

    done:
        return ss.str();
}

string ans(int no) {
    auto ansFile = ifstream(TEST_FILE_PATH + to_string(no) + "/ans.txt");
    ostringstream ss;
    string line = "";

    while (getline(ansFile, line)) {
        ss << line << '\n';
    }

    return ss.str();
}

TEST_CASE("Lexical Parser", "[lex]") {
    for (int i = 1; i <= TEST_FILE_TOTAL; ++i) {
        REQUIRE(parse(i) == ans(i));
    }
}
