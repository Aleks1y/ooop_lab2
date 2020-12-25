#include "Header.h"

using namespace std;

FileReader::FileReader(string filename)
{
    this->filename = filename;
    this->isReader = 1;
}

void FileReader::doWork(vector<string>& text)
{
    string str;
    ifstream fin(filename);
    if (!fin)
        throw("Can not open " + filename);
    while (getline(fin, str))
        text.push_back(str);

    fin.close();
}

FileWriter::FileWriter(string filename)
{
    this->filename = filename;
    this->isWriter = 1;
}

void FileWriter::doWork(vector<string>& text)
{
    ofstream fout(filename);
    if (!fout)
        throw("Can not open " + filename);

    for (size_t i = 0; i < text.size(); i++)
    {
        fout << text[i] << endl;
    }

    fout.close();
}

void Grep::doWork(vector<string>& text)
{
    size_t i = 0;
    while (i < text.size())
    {
        if (text[i].find(word) == string::npos)
        {
            text.erase(text.begin() + i);
        }
        else
        {
            i++;
        }
    }
}

void Sort::doWork(vector<string>& text)
{
    sort(text.begin(), text.end());
}

void Replace::doWork(vector<string>& text)
{
    if (text.size() == 0)
    for (size_t i = 0; i < text.size(); i++)
    {
        unsigned int index = text[i].find(word1);
        while (index != string::npos)
        {
            text[i].replace(index, word1.size(), word2);
            index = text[i].find(word1);
        }
    }
}

void Dump::doWork(vector<string>& text)
{
    ofstream fout(filename);
    if (!fout)
        throw("Can not open " + filename);

    for (size_t i = 0; i < text.size(); i++)
    {
        fout << text[i] << endl;
    }

    fout.close();
}

shared_ptr<Worker> BlocksParser(ifstream& fin)
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
            
        blocks[index] = BlocksParser(fin);
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

    if (!blocks[order[0]]->isReader)
        throw("Reader is not first");

    if (!blocks[order[order.size() - 1]]->isWriter)
        throw("Writer is not last");

    for (size_t j = 1; j < order.size() - 1; j++)
    {
        if (blocks[order[j]]->isReader)
            throw("Reader in the middle");

        if (blocks[order[j]]->isWriter)
            throw("Writer in the middle");
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
    vector<string> text;
    for (auto it : queue)
    {
        it->doWork(text);
    }
}
