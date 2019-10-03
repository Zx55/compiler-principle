/*
 * LexParserTest.cpp
 * Test of LexParser.
 * Copyright (c) zx5. All rights reserved.
 */

#include <iostream>
#include <fstream>
#include "LexParser.h"

using namespace std;
using namespace Compiler;

const string TEST_FILE_PATH = "compiler-principle/oj/LexParser/test/";
const int TEST_FILE = 8;

void testParser(int no) {
    const auto outputPath = TEST_FILE_PATH + "l" + to_string(no) + "/out.txt";
    const auto answerPath = TEST_FILE_PATH + "l" + to_string(no) + "/ans.txt";

    auto inputFile = ifstream(TEST_FILE_PATH + "l" + to_string(no) + "/in.txt");
    auto outputFile = ofstream(outputPath);
    auto answerFile = ifstream(answerPath);

    auto parser = LexParser(inputFile);

    while (true) {
        auto result = parser.parseNext();

        switch (result.symbol) {
            case Symbol::SEOF:
                goto done;
            case Symbol::IDENTIFIER:
            case Symbol::INTEGER:
            case Symbol::INCOMPLETECOMMENT: {
                outputFile << result.symbol << ' ';
                result.info == "" ? (outputFile << result.num) :
                    (outputFile << result.info);
                outputFile << endl;
                break;
            }
            case Symbol::COMMENT:
                break;
            case Symbol::UNDEFINED: {
                outputFile << -1 << ' ' << result.info << endl;
                goto done;
            }
            default: {
                outputFile << result.symbol << endl;
            }
        }
    }

done:
    string cmd = "diff " + outputPath + " " + answerPath;
    int ans = system(cmd.c_str());
    cout << "[" << no << "] ";
    if (ans > 0) {
        cout << "Test Fail" << endl;
    } else {
        cout << "Test Pass" << endl;
    }
}

void testAll() {
    for (int i = 1; i <= TEST_FILE; ++i) {
        testParser(i);
    }
}

int main() {
    testAll();

    return 0;
}