
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

    /**
     * @brief Parses a file containing live ranges.
     * @param filename Name of the file to parse.
     * @return Map containing the parsed live ranges organized by identifier.
     */

    static std::map<std::string, std::vector<LiveRange>> parseRanges(
        const std::string& filename);

    /**
     * @brief Parses the registers file.
     *
     * @param filename Name of the configuration file.
     * @param numRegistos Number of available registers.
     * @param algoritmo Name of the selected algorithm.
     * @param parametro Additional parameter associated with the algorithm.
     */
    static void parseConfig(const std::string& filename,int& numRegistos,std::string& algoritmo,int& parametro);

private:

    /**
     * @brief Removes leading and trailing whitespace from a string.
     * @param str Original string.
     * @return Trimmed string without surrounding whitespace.
     */
    static std::string trim(const std::string& str);

    /**
     * @brief Splits a string using a delimiter character.
     *
     * @param str String to split.
     * @param delim Delimiter character.
     * @return Vector containing the resulting substrings.
     */
    static std::vector<std::string> split(const std::string& str, char delim);
};

#endif
