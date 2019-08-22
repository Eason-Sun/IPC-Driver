#include "Vertex.h"
#include <limits>
#include <iostream>

using namespace std;

Vertex::Vertex(int id)
{
  this->id = id;
  color = Vertex::white;
  distance = numeric_limits<int>::max();
  predecessor = NULL;
}

void Vertex::setColor(Color color)
{
  this->color = color;
}

void Vertex::setDistance(int distance)
{
  this->distance = distance;
}

void Vertex::setPredecessor(Vertex *predecessor)
{
  this->predecessor = predecessor;
}

int Vertex::getId()
{
  return id;
}

Vertex::Color Vertex::getColor()
{
  return color;
}

int Vertex::getDistance()
{
  return distance;
}

Vertex* Vertex::getPredecessor()
{
  return predecessor;
}