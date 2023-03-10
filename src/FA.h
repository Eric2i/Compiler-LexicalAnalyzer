#ifndef NF_H
#define NF_H

struct Edge
{
    int dest;   // destination of the edge
    char label; // label of the edge
};

struct State
{
    std::vector<Edge> outEdges;
};

#endif