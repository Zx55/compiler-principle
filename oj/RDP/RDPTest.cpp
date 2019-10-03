/*
 * RDPTest.cpp
 * Test of RDP.
 * Copyright (c) zx5. All rights reserved.
 */

#include <iostream>
#include <fstream>
#include "RDP.h"

using namespace std;
using namespace Compiler;

const string TEST_FILE_PATH = "compiler-principle/oj/Parser/test/p";
const int TEST_FILE = 5;

void test(int no) {
    auto inFile = ifstream(TEST_FILE_PATH + to_string(no) + "/in.txt");
    auto outFile = ofstream(TEST_FILE_PATH + to_string(no) + "/out.txt");
    auto ansFile = ifstream(TEST_FILE_PATH + to_string(no) + "/ans.txt");

    auto oldCin = cin.rdbuf(inFile.rdbuf());

    auto parser = RDP();
    string res = parser.parse() ? "T" : "F";
    outFile << res << endl;

    string ans;
    ansFile >> ans;

    if (res == ans) {
        cout << "[" << no << "] Test Pass." << endl;
    } else {
        cout << "[" << no << "] Test Fail." << endl;
    }

    cin.rdbuf(oldCin);
}

void testAll() {
    for (int i = 1; i <= TEST_FILE; ++i) {
        test(i);
    }
}

int main() {
    testAll();

    return 0;
}