#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <exception>
#include <filesystem>
#include <iostream>
#include <set>

#include "../lib/document.hpp"

#include "utils.cpp"

namespace fs = std::filesystem;

using DocumentNames = std::map<std::string, std::vector<std::string>>;

TEST_CASE("01 - Inverted List Test without a valid dir") {
    DOCTEST_CHECK_THROWS_AS(DocumentsData("../include/"), DirNotFoundException);
}

// Compare two index:
bool compareDocumentIndex(DocumentNames& names, DocumentsData& data) {
    bool result = true;
    DocumentIndex index = data.get_document_index();
    if (names.size() != index.size())
        result = false;

    for (const auto& [word, doc_names_1] : names) {
        auto it = index.find(word);
        if (it == index.end())
            result = false;

        const auto& doc_indices_2 = it->second;

        if (doc_names_1.size() != doc_indices_2.size())
            result = false;

        std::set<std::string> doc_names_set(doc_names_1.begin(), doc_names_1.end());
        std::set<std::string> doc_indices_set;
        for (int doc_idx : doc_indices_2) {
            std::string doc_name_2 = data.get_doc_name(doc_idx);
            doc_indices_set.insert(doc_name_2);
        }

        if (doc_names_set != doc_indices_set)
            result = false;
    }

    return result;
}

TEST_CASE("02 - Test document index with a temporary directory") {
    fs::path tmp_dir = fs::temp_directory_path() / "test_inverted_list";
    std::map<std::string, std::string> temp_corpus{{"0.txt", "test one"}, {"1.txt", "test two"}, {"2.txt", "test three"}};
    utils::create_temp_corpus(tmp_dir, temp_corpus);

    DocumentNames names = {{"test", {"0.txt", "1.txt", "2.txt"}}, 
                           {"one" , {"0.txt"}}, {"two" , {"1.txt"}}, 
                           {"three", {"2.txt"}}};

    DocumentsData data(tmp_dir.c_str());

    CHECK(compareDocumentIndex(names, data));

    fs::remove_all(tmp_dir);
}

TEST_CASE("03 - Test avarage size of documents in a temporary directory") {
    fs::path tmp_dir = fs::temp_directory_path() / "test_inverted_list";
    std::map<std::string, std::string> temp_corpus{{"0.txt", "test one, testing"}, {"1.txt", "testing this function"}, {"2.txt", "test three, this is a test"}};
    utils::create_temp_corpus(tmp_dir, temp_corpus);

    DocumentsData data(tmp_dir.c_str());
    double size = data.get_avg_size();
    int size1 = data.get_size(0);
    int size2 = data.get_size(1);
    int size3 = data.get_size(2);
    double mean_size = ((double)size1 + (double)size2 + (double)size3) / 3;
    CHECK(size == mean_size);

    fs::remove_all(tmp_dir);
}

TEST_CASE("04 - Test get frequency of terms in documents of a temporary directory") {
    fs::path tmp_dir = fs::temp_directory_path() / "test_inverted_list";
    std::map<std::string, std::string> temp_corpus{{"0.txt", "test one, testing"}, {"1.txt", "testing this function"}, {"2.txt", "test three, this is a test"}};
    utils::create_temp_corpus(tmp_dir, temp_corpus);

    DocumentsData data(tmp_dir.c_str());
    std::string word = "test";
    
    for (int doc_idx = 0; doc_idx < data.get_qt_docs(); doc_idx++) {
        std::string name = data.get_doc_name(doc_idx);
        int freq = data.get_frequence("test", doc_idx);
        if (name == "0.txt") {
            CHECK(freq == 1);
        } else if (name == "1.txt") {
            CHECK(freq == 0);
        } else if (name == "2.txt") {
            CHECK(freq == 2);
        }
    }

    fs::remove_all(tmp_dir);
}

TEST_CASE("05 - Test avarage size of a temporary directory without files") {
    fs::path tmp_dir = fs::temp_directory_path() / "test_inverted_list";
    std::map<std::string, std::string> temp_corpus{};
    utils::create_temp_corpus(tmp_dir, temp_corpus);
    
    DocumentsData data(tmp_dir.c_str());

    double size = data.get_avg_size();
    CHECK(size == 0);

    fs::remove_all(tmp_dir);
}

TEST_CASE("06 - Test size of a empty document in a temporary directory") {
    fs::path tmp_dir = fs::temp_directory_path() / "test_inverted_list";
    std::map<std::string, std::string> temp_corpus{{"0.txt", ""}};
    utils::create_temp_corpus(tmp_dir, temp_corpus);

    DocumentsData data(tmp_dir.c_str());
    std::string word = "test";

    int size = data.get_size(0);
    CHECK(size == 0);
}

/*
TEST_CASE("07 - Test conversion from wordIndex to docIndex in a temporary directory") {
    fs::path tmp_dir = fs::temp_directory_path() / "test_inverted_list";
    std::map<std::string, std::string> temp_corpus{{"0.txt", "test one, testing"}, {"1.txt", "testing this function"}, {"2.txt", "test three, this is a test"}};
    utils::create_temp_corpus(tmp_dir, temp_corpus);

    DocumentsData data(tmp_dir.c_str());
    
    std::unordered_map<std::string, std::unordered_map<int, int>> wordIndex = {
        {"test", {{0, 1}, {2, 1}}},
        {"one", {{0, 1}}},
        {"testing", {{0, 1}, {1, 1}}},
        {"this", {{1, 1}, {2, 1}}},
        {"function", {{1, 1}}},
        {"three", {{2, 1}}},
        {"is", {{2, 1}}},
        {"a", {{2, 1}}},
    };

    DocumentIndex docIndex = data.convertToDocumentIndex(wordIndex);

    DocumentIndex expectedIndex = {
        {"test", {0, 2}},
        {"one", {0}},
        {"testing", {0, 1}},
        {"this", {1, 2}},
        {"function", {1}},
        {"three", {2}},
        {"is", {2}},
        {"a", {2}},
    };

    CHECK(docIndex == expectedIndex);

    fs::remove_all(tmp_dir);
}
*/