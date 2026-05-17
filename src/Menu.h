/**
* @file Menu.cpp
 * @brief Implementation of the menu system.
 */

#ifndef DA_PROJECT_MENU_H
#define DA_PROJECT_MENU_H

#include "Graph.h"
#include "Parser.h"

using namespace std;


void runMenu( std::map<std::string, std::vector<LiveRange>>ranges,int numRegisters,std::string algorithm,int parameters,bool rangesF, bool registersF );

#endif