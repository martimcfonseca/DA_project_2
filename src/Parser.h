
#ifndef PARSER_H
#define PARSER_H

#include "LiveRange.h"
#include <string>
#include <map>
#include <fstream>
#include <sstream>
/**
 * @class Parser
 * @brief Responsible for parsing configuration files and live ranges.
 *
 * This class provides utility methods to interpret input files
 * and convert their contents into internal data structures used
 * by the program.
 */
class Parser {
public:


    static std::map<std::string, std::vector<LiveRange>> parseRanges(
        const std::string& filename);



    static void parseConfig(const std::string& filename,int& numRegistos,std::string& algoritmo,int& parametro);

private:
    static std::string trim(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delim);
};

#endif
