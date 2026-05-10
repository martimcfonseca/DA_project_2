//
// Created by Martim on 10/05/2026.
//

#include <iostream>
#include <string>

#include "Parser.h"
using namespace std;

int main(int argc, char *argv[]) {
    cout << "path of the ranges file: ";
    string ranges_file,config;
    cin >> ranges_file;
    cout << "path of the config file: ";
    cin >> config;
    map<string, vector<LiveRange>> ranges = Parser::parseRanges(ranges_file);
    int num,parameter;
    string algorithm;
    Parser::parseConfig(config, num, algorithm, parameter);
}
