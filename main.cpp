#include "Header.h"

using namespace std;
int main(int argc, char *argv[])
{
    bool inputFromWorkflow = 1,
         outputFromWorkflow = 1;
    try
    {
        if (argc < 2)
        {
            throw exception("Wrong arguments");
        }

        string workflow = argv[1];
        string input, output;

        if (argc > 2)
        {
            bool wasInput = false;
            bool wasOutput = false;

            for (int i = 2; i < argc; ++i)
            {
                if (((string)"-i") == argv[i] && i != argc && !wasInput)
                {
                    wasInput = true;
                    input = argv[++i];
                    inputFromWorkflow = 0;
                }
                else if (((string)"-o") == argv[i] && i != argc && !wasOutput)
                {
                    wasOutput = true;
                    output = argv[++i];
                    outputFromWorkflow = 0;
                }
                else
                    throw exception("Wrong args from prompt!");
            }
        }

        Parser::parser(workflow, input, output, inputFromWorkflow, outputFromWorkflow).execute();
    }
    catch (const char *str)
    {
        cout << str;
    }
    catch (exception& ex)
    {
        cout << ex.what();
    }

	return 0;
}
