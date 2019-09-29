/*
 * LexParserTest.cpp
 * 词法分析器测试
 * Copyright (c) zx5. All rights reserved.
 */

#include <iostream>
#include <fstream>
#include "LexParser.h"

using namespace std;
using namespace Compiler;

const string TEST_FILE_PATH = "cpp/compiler/oj/";

int main() {
    // 流重定向
    auto in = ifstream(TEST_FILE_PATH + "in.txt");
    cin.rdbuf(in.rdbuf());
    //auto out = ofstream((TEST_FILE_PATH + "out.txt");
    //cout.rdbuf(out.rdbuf());

    auto analyzer = LexParser();

    while (true) {
        auto result = analyzer.parseNext();

        switch (result.symbol) {
            case Symbol::SEOF:
                goto done;
            case Symbol::IDENTIFIER:
            case Symbol::INTEGER:
            case Symbol::INCOMPLETECOMMENT: {
                cout << result.symbol << ' ';
                result.info == "" ? (cout << result.num) :
                    (cout << result.info);
                cout << endl;
                break;
            }
            case Symbol::COMMENT:
                break;
            case Symbol::UNDEFINED: {
                cout << -1 << ' ' << result.info << endl;
                goto done;
            }
            default: {
                cout << result.symbol << endl;
            }
        }
    }

done:
    return 0;
}