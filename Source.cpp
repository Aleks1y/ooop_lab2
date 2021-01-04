#include "Header.h"

using namespace std;

void FileReader::doWork(optional<vector<string>>& text)
{
    if(text)
        throw("Reader position error");

    string str;
    ifstream fin(filename);
    if (!fin)
        throw("Can not open " + filename);

    vector<string> t;
    text = t;

    while (getline(fin, str))
        text->push_back(str);

    fin.close();
}

void FileWriter::doWork(optional<vector<string>>& text)
{
    ofstream fout(filename);
    if (!fout)
        throw("Can not open " + filename);

    for (const auto& line : *text)
    {
        fout << line << endl;
    }

    text = nullopt;
    fout.close();
}

void Grep::doWork(optional<vector<string>>& text)
{
    size_t i = 0;
    while (i < text->size())
    {
        if ((*text)[i].find(word) == string::npos)
        {
            text->erase(text->begin() + i);
        }
        else
        {
            i++;
        }
    }
}

void Sort::doWork(optional<vector<string>>& text)
{
    sort(text->begin(), text->end());
}

void Replace::doWork(optional<vector<string>>& text)
{
    for (auto& line : *text)
    {
        unsigned int index = line.find(word1);
        while (index != string::npos)
        {
            line.replace(index, word1.size(), word2);
            index = line.find(word1);
        }
    }
}

void Dump::doWork(optional<vector<string>>& text)
{
    ofstream fout(filename);
    if (!fout)
        throw("Can not open " + filename);

    for(const auto& line: *text)
    {
        fout << line << endl;
    }

    fout.close();
}

shared_ptr<Worker> WorkerFabric(ifstream& fin)
{
    string tmp0, tmp1;
    fin >> tmp0;

    if (tmp0 != "=")
        throw("Haven't '='");

    fin >> tmp0;
    if (tmp0 == "readfile")
    {
        fin >> tmp1;
        return make_shared<FileReader>(tmp1);
    }
    else if (tmp0 == "writefile")
    {
        fin >> tmp1;
        return make_shared<FileWriter>(tmp1);
    }
    else if (tmp0 == "grep")
    {
        fin >> tmp1;
        return make_shared<Grep>(tmp1);
    }
    else if (tmp0 == "sort")
    {
        return make_shared<Sort>();
    }
    else if (tmp0 == "replace")
    {
        fin >> tmp0 >> tmp1;
        return make_shared<Replace>(tmp0, tmp1);
    }
    else if (tmp0 == "dump")
    {
        fin >> tmp1;
        return make_shared<Dump>(tmp1);
    }
    throw("Bad name: " + tmp0);
}

BlockProgram Parser::parser(string file, string input, string output, bool inputFromWorkflow, bool outputFromWorkflow)
{
    unsigned int index;
    vector<int> order;
    map<unsigned int, shared_ptr<Worker>> blocks;
    list <shared_ptr<Worker>> queue;
    ifstream fin(file);
    if(!fin.is_open())
        throw("Can not open " + file +" file!");

    if (fin.eof())
        throw("EOF file!");
    string tmp0;
    fin >> tmp0;
    if (tmp0 != "desc")
        throw("Haven't 'desc' in" + file);
    if (fin.eof())
        throw("EOF file!");

    fin >> tmp0;

    while (tmp0 != "csed")
    {
        if (fin.eof())
            throw("Bad end of " + file);

        index = stoi(tmp0);
        if (blocks.count(index))
            throw("Index " + to_string(index) + " already exists!");
            
        blocks[index] = WorkerFabric(fin);
        fin >> tmp0;
    }

    if (fin.eof())
        throw("Bad end of " + file);

    while (!fin.eof())
    {
        fin >> tmp0;
        index = stoi(tmp0);
        if(!blocks.count(index))
            throw("No description for block number" + tmp0);\

        order.push_back(index);

        if (fin.eof())
            break;

        fin >> tmp0;
        if (tmp0 != "->")
            throw("Unknown symbol in" + file+ ":" + tmp0);
    }

    for (size_t i = 0; i < order.size(); i++)
    {
        queue.push_back(blocks[order[i]]);
    }

    if (!outputFromWorkflow)
    {
        queue.push_back(make_shared<FileWriter>(output));
    }

    if (!inputFromWorkflow)
    {
        queue.push_front(make_shared<FileReader>(input));
    }

    return BlockProgram(queue, move(input), move(output));
}

void BlockProgram::execute()
{
    optional<vector<string>> text;
    for (auto it : queue)
    {
        if (!text)
        {
            it->doWork(text);
            if (!text)
                throw("Writer position error");
        }
        else
        {
            it->doWork(text);
        }
    }
}
