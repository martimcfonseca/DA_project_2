
#include "Web.h"
#include <iostream>
#include <algorithm>

void Web::addLiveRange(LiveRange* lr) {
    liveRanges.push_back(lr);

    // Adicionar todas as linhas ao conjunto
    for (int linha : lr->linhas) {
        linhas.insert(linha);
    }

    // Usar a variável do primeiro live range
    if (variavel.empty()) {
        variavel = lr->variavel;
    }
}

bool Web::contem(int linha) const {
    return linhas.find(linha) != linhas.end();
}

bool Web::interfereWith(const Web& other) const {


    std::vector<int> interseccao;
    std::set_intersection(linhas.begin(), linhas.end(),other.linhas.begin(), other.linhas.end(),std::back_inserter(interseccao));

    if (interseccao.empty()) {
        return false;
    }

    // Verificar se é o caso especial:
    // um web TERMINA exatamente onde outro COMEÇA
    for (int linha : interseccao) {
        bool este_termina = false;
        bool outro_comeca = false;

        // Verificar se este web TERMINA nesta linha
        for (LiveRange* lr : liveRanges) {
            if (lr->fim == linha && lr->tem_uso) {
                este_termina = true;
                break;
            }
        }

        // Verificar se o outro web COMEÇA nesta linha
        for (LiveRange* lr : other.liveRanges) {
            if (lr->inicio == linha && lr->tem_def) {
                outro_comeca = true;
                break;
            }
        }

        // Caso especial: este morre, outro nasce → NÃO interferem
        if (este_termina && outro_comeca) {
            // continuar para as proximas linhas
            continue;
        }

        return true;
    }

    return false;
}

void Web::print() const {
    std::cout << "web" << id << " (" << variavel << "): ";
    std::cout << formatLinhas() << std::endl;
}

std::string Web::formatLinhas() const {
    if (linhas.empty()) return "";

    std::string result;
    std::vector<int> linhas_vec(linhas.begin(), linhas.end());

    for (size_t i = 0; i < linhas_vec.size(); i++) {
        int linha = linhas_vec[i];
        result += std::to_string(linha);

        // Verificar se tem marcador + ou -
        bool tem_plus = false;
        bool tem_minus = false;

        for (LiveRange* lr : liveRanges) {
            if (lr->inicio == linha && lr->tem_def) {
                tem_plus = true;
            }
            if (lr->fim == linha && lr->tem_uso) {
                tem_minus = true;
            }
        }

        if (tem_plus) result += "+";
        if (tem_minus) result += "-";

        if (i < linhas_vec.size() - 1) {
            result += ",";
        }
    }

    return result;
}
