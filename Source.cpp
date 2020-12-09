#include "Header.h"

using namespace std;

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

void FileWriter::doWork(vector<string>& text)
{
    string str;
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
    for (size_t i = 0; i < text.size(); i++)
    {
        if (text[i].find(word) == string::npos)
            text.erase(text.begin() + i);
    }
}

void Sort::doWork(vector<string>& text)
{
    sort(text.begin(), text.end());
}

void Replace::doWork(vector<string>& text)
{
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
    string str;
    ofstream fout(filename);
    if (!fout)
        throw("Can not open " + filename);

    for (size_t i = 0; i < text.size(); i++)
    {
        fout << text[i];
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
        return make_shared<FileReader>(tmp1);;
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
    else
    {
        throw("Bad name: " + tmp0);
        return nullptr;
    }
}

BlockProgram Parser::parser(string file, string input, string output)
{
    unsigned int index;
    vector<int> queue;
    map<unsigned int, shared_ptr<Worker>> blocks;
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
        queue.push_back(index);

        if (fin.eof())
            break;

        fin >> tmp0;
        if (tmp0 != "->")
            throw("Unknown symbol in" + file+ ":" + tmp0);
        
    }

    return BlockProgram(blocks, queue, move(input), move(output));
}

void BlockProgram::execute()
{
    vector<string> text;
    for (size_t i = 0; i < queue.size(); i++)
    {
        blocks[queue[i]]->doWork(text);
    }
}