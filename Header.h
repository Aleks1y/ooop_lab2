#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <algorithm>
#include <list>

using namespace std;

class Worker
{
public:
    bool isReader = 0;
    bool isWriter = 0;
    virtual void doWork(vector<string>& text) = 0;
};

class FileReader : public Worker
{
private:
    string filename;
public:
    FileReader(string filename);
    void doWork(vector<string>& text) override;
};

class FileWriter : public Worker
{
private:
    string filename;
public:
    FileWriter(string filename);
    void doWork(vector<string>& text) override;
};

class Grep : public Worker
{
private:
    string word;
public:
    Grep(string word) : word(word) {};
    void doWork(vector<string>& text) override;
};

class Sort : public Worker
{
public:
    void doWork(vector<string>& text) override;
};

class Replace : public Worker
{
private:
    string word1;
    string word2;
public:
    Replace(string word1, string word2) : word1(word1), word2(word2) {};
    void doWork(vector<string>& text) override;
};

class Dump : public Worker
{
private:
    string filename;
public:
    Dump(string filename) : filename(filename) {};
    void doWork(vector<string>& text) override;
};

class BlockProgram
{
private:
    list <shared_ptr<Worker>> queue;
    string input;
    string output;
public:
    BlockProgram(list <shared_ptr<Worker>> queue, string input, string output) : queue(queue), input(input), output(output) {}
    shared_ptr<Worker> BlocksParser(ifstream& fin);
    void execute();
};

class Parser
{
public:
    static BlockProgram parser(string file, string input, string output, bool inputFromWorkflow, bool outputFromWorkflow);
};
