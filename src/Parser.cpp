
#include "Parser.h"
#include <iostream>
#include <cctype>
#include <algorithm>

std::string Parser::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";

    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

std::vector<std::string> Parser::split(const std::string& str, char delim) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delim)) {
        token = trim(token);
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }

    return tokens;
}

std::map<std::string, std::vector<LiveRange>> Parser::parseRanges(const std::string& filename) {

    std::map<std::string, std::vector<LiveRange>> ranges;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Erro: não foi possível abrir " << filename << std::endl;
        return ranges;
    }

    std::string linha;
    int linha_num = 0;

    while (std::getline(file, linha)) {
        linha_num++;
        linha = trim(linha);

        // Ignorar linhas vazias e comentários
        if (linha.empty() || linha[0] == '#') {
            continue;
        }

        // Formato esperado: "variavel: 1+,2,3,4-"
        size_t pos_colon = linha.find(':');
        if (pos_colon == std::string::npos) {
            std::cerr << "Aviso: linha " << linha_num << " mal formatada (falta ':')" << std::endl;
            continue;
        }

        std::string variavel = trim(linha.substr(0, pos_colon));
        std::string ranges_str = trim(linha.substr(pos_colon + 1));


        std::vector<std::string> tokens = split(ranges_str, ',');

        LiveRange lr(variavel);

        for (const std::string& token : tokens) {
            // Extrair número e marcador (+/-)
            int numero = 0;
            char marcador = '\0';

            std::string num_str = token;

            // Verificar se tem + ou -
            if (token.back() == '+' || token.back() == '-') {
                marcador = token.back();
                num_str = token.substr(0, token.length() - 1);
            }

            try {
                numero = std::stoi(num_str);
                lr.addLinha(numero, marcador);
            } catch (const std::exception& e) {
                std::cerr << "Erro ao parsear número: " << token
                         << " na linha " << linha_num << std::endl;
            }
        }

        // Ordenar as linhas
        std::sort(lr.linhas.begin(), lr.linhas.end());

        // Adicionar ao mapa
        ranges[variavel].push_back(lr);
    }

    file.close();

    // Debug: imprimir o que foi parseado
    std::cout << "\n=== RANGES PARSEADOS ===\n";
    for (const auto& pair : ranges) {
        std::cout << "Variável: " << pair.first << std::endl;
        for (const auto& lr : pair.second) {
            std::cout << "  ";
            lr.print();
        }
    }
    std::cout << "========================\n\n";

    return ranges;
}

void Parser::parseConfig(const std::string& filename,int& numRegistos,std::string& algoritmo,int& parametro) {

    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Erro: não foi possível abrir " << filename << std::endl;
        return;
    }

    std::string linha;

    while (std::getline(file, linha)) {
        linha = trim(linha);

        // Ignorar comentários e linhas vazias
        if (linha.empty() || linha[0] == '#') {
            continue;
        }

        // Parse "registers.txt: 3"
        if (linha.find("registers.txt:") == 0) {
            size_t pos = linha.find(':');
            std::string valor = trim(linha.substr(pos + 1));
            numRegistos = std::stoi(valor);
            std::cout << "Registos: " << numRegistos << std::endl;
        }

        // Parse "algorithm: basic" ou "algorithm: spilling, 2"
        else if (linha.find("algorithm:") == 0) {
            size_t pos = linha.find(':');
            std::string valor = trim(linha.substr(pos + 1));

            // Verificar se tem parâmetro (separado por vírgula)
            size_t pos_comma = valor.find(',');
            if (pos_comma != std::string::npos) {
                algoritmo = trim(valor.substr(0, pos_comma));
                std::string param_str = trim(valor.substr(pos_comma + 1));
                parametro = std::stoi(param_str);
                std::cout << "Algoritmo: " << algoritmo<< " (parâmetro: " << parametro << ")" << std::endl;
            } else {
                algoritmo = valor;
                parametro = 0;
                std::cout << "Algoritmo: " << algoritmo << std::endl;
            }
        }
    }

    file.close();
}