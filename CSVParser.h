//
// Created by nikol on 26.11.2021.
//

#ifndef LAB_4_CSVPARSER_H
#define LAB_4_CSVPARSER_H

#include <string>
#include <fstream>
#include <memory>
#include <utility>
#include <vector>
#include "CSVLineParser.h"
#include "CSVParserParametrs/CSVCellMaker.h"
#include "CSVParserParametrs/CSVStringCellMakerImpl.h"


template<class... Args>
class CSVParser {
public:
    explicit CSVParser(const std::string& fileName, int skipFirstLinesCount = 0);
    explicit CSVParser(const std::string& fileName, std::shared_ptr<CSVCellMaker> maker, int skipFirstLinesCount = 0):maker(std::move(maker)){
        customizeInputStream(fileName);
        parseFile();
    }

    virtual ~CSVParser();

    struct InputIterator{
        typedef typename std::tuple<Args...> value_type;
        typedef value_type& reference;
        typedef value_type* pointer;
        typedef std::input_iterator_tag iterator_category;

        explicit InputIterator(typename std::vector<std::tuple<Args...> >::iterator it);
        InputIterator(const InputIterator& other);

        InputIterator& operator++();

        //InputIterator operator++(int) const;

        value_type operator*() const;

        bool operator==(const InputIterator&);
        bool operator!=(const InputIterator&);
    private:
        typename std::vector<std::tuple<Args...> >::iterator it;
    };

    InputIterator begin();
    InputIterator end();

private:
    std::shared_ptr<CSVCellMaker> maker;
    std::vector<std::tuple<Args...>> table;
    std::ifstream file;
    int skipFirsLinesCount{};
    int lineNumber{};

    void customizeInputStream(const std::string& fileName);

    void goToBeginningFile();

    void parseFile();

};

template<class... Args>
CSVParser<Args...>::CSVParser(const std::string &fileName, int skipFirstLinesCount) : maker(new CSVStringCellMakerImpl){
    customizeInputStream(fileName);
    parseFile();
}


template<class... Args>
void CSVParser<Args...>::customizeInputStream(const std::string &fileName) {
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    file.open(fileName);
}

template<class... Args>
CSVParser<Args...>::~CSVParser() {
    file.close();
}

template<class... Args>
typename CSVParser<Args...>::InputIterator CSVParser<Args...>::begin() {
    //goToBeginningFile();
    return CSVParser::InputIterator(table.begin());
}

template<class... Args>
void CSVParser<Args...>::goToBeginningFile() {
    file.clear();
    file.seekg(0);
    for(int i = 0; i < skipFirsLinesCount; i++){
        std::string str;
        std::getline(file, str);
    }
}

template<class... Args>
CSVParser<Args...>::InputIterator::InputIterator(typename std::vector<std::tuple<Args...>>::iterator it) {
    this->it = it;
}

template<class... Args>
typename CSVParser<Args...>::InputIterator::value_type CSVParser<Args...>::InputIterator::operator*() const {
    return *it;
}


template<class... Args>
typename CSVParser<Args...>::InputIterator CSVParser<Args...>::end() {
    return CSVParser::InputIterator(table.end());
}

template<class... Args>
void CSVParser<Args...>::parseFile() {
    goToBeginningFile();
    CSVLineParser<Args...> parser(maker, file);
    parser.parse();
    table = parser.getTable();
}

template<class... Args>
CSVParser<Args...>::InputIterator::InputIterator(const CSVParser::InputIterator &other) {
    this->it = other.it;
}

template<class... Args>
bool CSVParser<Args...>::InputIterator::operator==(const CSVParser::InputIterator &other) {
    return this->it == other.it;
}

template<class... Args>
bool CSVParser<Args...>::InputIterator::operator!=(const CSVParser::InputIterator &other) {
    return this->it != other.it;
}

template<class... Args>
typename CSVParser<Args...>::InputIterator &CSVParser<Args...>::InputIterator::operator++() {
    it++;
    return *this;
}


#endif //LAB_4_CSVPARSER_H
