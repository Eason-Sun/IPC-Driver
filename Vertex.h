#ifndef VERTEX_H
#define VERTEX_H

class Vertex {
private:
    int id;
    int distance;
    Vertex *predecessor;
    

public:
    enum Color
    {
        white,
        grey,
        black
    };
    Color color;
    
    Vertex(int);

    void setColor(Color);

    void setDistance(int);

    void setPredecessor(Vertex*);

    int getId();

    Color getColor();

    int getDistance();

    Vertex* getPredecessor();

};

#endif