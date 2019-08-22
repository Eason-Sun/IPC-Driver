#include "Graph.h"
#include <queue>
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;

Graph::Graph(int maxId)
{
    this->maxId = maxId;
    this->m = map<int, vector<int>>();
    this->vertexVect = vector<Vertex *>();
}

void Graph::setM(vector<int> v)
{
    map<int, vector<int>> m;
    bool isEligibleEdge = true;
    for (unsigned int i = 0; i < v.size(); i += 1)
    {
        if (v[i] >= this->maxId)
        {
            isEligibleEdge = false;
            break;
        }
    }
    if (isEligibleEdge)
    {
        for (unsigned int i = 0; i < v.size(); i += 2)
        {
            if (m.count(v[i]) == 0)
            {
                m.insert(pair<int, vector<int>>(v[i], vector<int>()));
            }
            if (m.count(v[i + 1]) == 0)
            {
                m.insert(pair<int, vector<int>>(v[i + 1], vector<int>()));
            }
            m[v[i]].push_back(v[i + 1]);
            m[v[i + 1]].push_back(v[i]);
        }
        this->m = m;
    }
    else
    {
        throw "There exists a vertex in edges that exceeds the maximum vertex id.";
    }
}

void Graph::configVertexVect()
{
    if (!(this->vertexVect).empty())
    {
        for (vector<Vertex *>::iterator it = (this->vertexVect).begin(); it != (this->vertexVect.end()); it++)
        {
            delete *it;
        }
        this->vertexVect = vector<Vertex *>();
    }

    vector<Vertex *> v(this->maxId);
    for (map<int, vector<int>>::iterator it = (this->m).begin(); it != (this->m).end(); ++it)
    {
        if (v[it->first] == NULL)
        {
            v[it->first] = new Vertex(it->first);
            for (unsigned int i = 0; i < (this->m)[it->first].size(); i++)
            {
                int id = (this->m)[it->first][i];
                if (v[id] == NULL)
                {
                    v[id] = new Vertex(id);
                }
            }
        }
    }
    this->vertexVect = v;
}

void Graph::updateVertexVect(int dstId)
{
    Vertex *dst = this->vertexVect[dstId];
    dst->setColor(Vertex::grey);
    dst->setDistance(0);
    queue<Vertex *> q;
    q.push(dst);

    while (!q.empty())
    {
        Vertex *u = q.front();
        q.pop();
        vector<int> adj = this->m[u->getId()];
        for (vector<int>::iterator it = adj.begin(); it != adj.end(); ++it)
        {
            int vId = *it;
            Vertex *v = this->vertexVect[vId];
            if (v->getColor() == Vertex::white)
            {
                v->setColor(Vertex::grey);
                v->setDistance(1 + u->getDistance());
                v->setPredecessor(u);
                q.push(v);
            }
        }
        u->setColor(Vertex::black);
    }
}

string Graph::shortestPath(int srcId, int dstId)
{
    string str = "";
    Vertex *srcV = this->vertexVect[srcId];
    if (srcId == dstId)
    {
        str += ToString<int>(dstId);
        return str;
    }
    else if (srcV->getPredecessor() == NULL)
    {
        throw "No such path existed in this graph.";
    }
    else
    {
        int srcIdNew = srcV->getPredecessor()->getId();
        return ToString<int>(srcId) + "-" + shortestPath(srcIdNew, dstId);
    }
}

void Graph::printShortestPath(int srcId, int dstId)
{
    if (srcId >= this->maxId)
    {
        throw "Source vertex exceeds the maximum vertex id.";
    }
    else if (dstId >= this->maxId)
    {
        throw "Destination vertex exceeds the maximum vertex id.";
    }
    else if ((this->vertexVect[srcId] == NULL) || (this->vertexVect[dstId] == NULL))
    {
        throw "No such path existed in this graph.";
    }
    else
    {
        this->updateVertexVect(dstId);
        cout << this->shortestPath(srcId, dstId) << endl;
    }
}

void Graph::graphPrinter()
{
    map<int, vector<int>>::iterator it = (this->m).begin();
    while (it != (this->m).end())
    {
        cout << (it->first) << " :: ";
        vector<int>::iterator it2 = (it->second).begin();
        while (it2 != (it->second).end())
        {
            cout << (*it2);
            if (it2 != (it->second).end() - 1)
            {
                cout << ", ";
            }
            it2++;
        }
        cout << endl;
        it++;
    }
    cout << endl;
}

template <typename T>
string ToString(T val)
{
    stringstream stream;
    stream << val;
    return stream.str();
}

bool isRemoved(char c)
{
    return (c == '<') || (c == '>') || (c == '{') || (c == '}');
}

vector<int> inputParser(string cmd)
{
    vector<int> v;
    string str = cmd;
    str = str.substr(2);
    str.erase(remove_if(str.begin(), str.end(), isRemoved), str.end());
    stringstream ss(str);
    string segment;
    char delimiter = ',';
    while (getline(ss, segment, delimiter))
    {
        v.push_back(atoi(segment.c_str()));
    }
    return v;
}