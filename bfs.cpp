#include <iostream>
#include <sstream>
#include "Vertex.h"
#include "Graph.h"

using namespace std;

int main()
{
    string line = "";
    while (true)
    {
        getline(cin, line);
        while (line[0] == 'V')
        {
            cout << line << endl;
            Graph *g = new Graph(atoi((line.substr(2)).c_str()));
            getline(cin, line);
            while ((line[0] == 'E') && (line[1] == ' '))
            {
		cout << line << endl;    
                vector<int> v = inputParser(line);
                try
                {
                    g->setM(v);
                }
                catch (const char *exp)
                {
                    cout << "Error: " << exp << endl;
                }
                getline(cin, line);
                while (line[0] == 's')
                {
                    stringstream ss(line.substr(2));
                    string segment;
                    char delimiter = ' ';
                    vector<int> v;
                    while (getline(ss, segment, delimiter))
                    {
                        v.push_back(atoi(segment.c_str()));
                    }
                    int srcId = v[0];
                    int dstId = v[1];
                    try
                    {
                        g->configVertexVect();
                        g->printShortestPath(srcId, dstId);
                    }
                    catch (const char *exp)
                    {
                        cout << "Error: " << exp << endl;
                    }
                    getline(cin, line);
                    while (line == "")
                    {
                        getline(cin, line);
                    }
                }
            }
            delete g;
        }
        if (line == "EOF")
        {
            break;
        }
    }
    return 0;
}
