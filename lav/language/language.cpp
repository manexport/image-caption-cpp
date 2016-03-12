#include "language.h"
#include "../../settings.h"
#include <iostream>
#include <cstdlib>  //for exit
#include <algorithm>  //for find
#include <cmath>  //for std::abs, for std::pow

vector<string> Language::loadExcludeWord(string excludeFile, string includeFile, bool needExclude, bool needInclude) {
    vector<string> excludeVec;
    //exclude stop words
    if (needExclude) {
        ifstream excludeWords(excludeFile.c_str(), std::ios::in);
        if (!excludeWords) {
            std::cout << excludeFile << " open filed!" << endl;
            exit(1);
        }
       string excludeWord;
       while (getline(excludeWords, excludeWord, ',')) {
           excludeVec.push_back(excludeWord);
        }
    }
    //include operator words, that means to remove then from excludeVec if exists.
    if (needInclude) {
        ifstream includeWords(includeFile.c_str(), std::ios::in);
        if (!includeWords) {
            std::cout << includeFile << " open error!" << endl;
            exit(1);
        }
        string includeWord;
        while (getline(includeWords, includeWord, ',')) {
            vector<string>::iterator it = std::find(excludeVec.begin(), excludeVec.end(), includeWord);
            if (it != excludeVec.end()) {
                excludeVec.erase(it);
            }
        } 
    }
    return excludeVec;
}

vector<string> Language::sentenceTokenizer(string sentence) {
    vector<string> words;
    std::istringstream tokens(sentence);
    string word;
    while (getline(tokens, word, ' ')) {
        words.push_back(word);
    }
    return words;
}

vector<double> Language::getSentenceVec(Word2Vec<std::string> model, const vector<string>& sentence, const vector<string>& excludeVec,
                               bool needExclude) {
    vector<double> sentenceVec;
    for (auto &word : sentence) {
        if (needExclude && (find(excludeVec.begin(), excludeVec.end(), word) != excludeVec.end())) {
            const vector<float> wordVec = model.word_vector(word);
            if (sentenceVec.size() != wordVec.size()) {
                std::cout << "wrong word vector maybe!" << endl;
                exit(1);
            }
            else {
                for (int i = 0; i < sentence.size(); i++) {
                    sentenceVec[i] += wordVec[i];
                }
            }
        }
    }
    return sentenceVec;
}

double Language::getSentencePenalty(const int tokenCnt, const int avgCnt) {
    double result;
    auto maxCnt = [](const int a, const int b) {if (a >= b) return a; return b;};
    result = 1 - (std::abs(tokenCnt - avgCnt)) / maxCnt(tokenCnt, avgCnt);
    return result;
}

double Language::getCosSimilarity(const vector<double>& baseVec, const vector<double>& targetVec) {
    double cosDist = 0;
    if (baseVec.size() != targetVec.size()) {
        std::cout << "the vectors are not in the same size, cannot calculate cos distance" << endl;
        exit(1);
    }
    else {
        double dotProduct = 0, baseNorm = 0, targetNorm = 0;
        for (int i = 0; i < baseVec.size(); i++) {
            dotProduct += baseVec[i] * targetVec[i];
            baseNorm += std::pow(baseVec[i], 2);
            targetNorm += std::pow(targetVec[i], 2);
        }
        baseNorm = std::sqrt(baseNorm);
        targetNorm = std::sqrt(targetNorm);
        cosDist = dotProduct / (baseNorm * targetNorm);
    }
    return cosDist;
}
