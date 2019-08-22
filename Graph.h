#ifndef GRAPG_H
#define GRAPG_H
#include "Vertex.h"
#include <map>
#include <vector>

using namespace std;

class Graph {
private:
    map<int, vector<int> > m;
    vector<Vertex*> vertexVect;
    int maxId;

public:
    Graph(int);
    void graphPrinter();
    void setM(vector<int>);
    void configVertexVect();
    void updateVertexVect(int dstId);
    string shortestPath(int, int);
    void printShortestPath(int, int);
};

template <typename T>
string ToString(T val);

bool isRemoved(char);

vector<int> inputParser(string);
#endif