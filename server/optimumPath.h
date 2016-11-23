#ifndef _OPTIMUMPATH_H
#define _OPTIMUMPATH_H

#include <iostream>
#include <QList>
#include <queue>
#include <QVector>
#include <string.h>
#include <stdlib.h>
#include <algorithm>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define Temperature     (10000)    //初始温度
#define EPS   (1e-8)    //终止温度
#define DELTA (0.99)    //温度衰减率
#define LIMIT (10000)   //概率选择上限
#define OLOOP (1000)    //外循环次数
#define ILOOP (15000)   //内循环次数

#define INFINITE (10000000)	//最大路径
#define NOTVERTEX (-1)	//无点

using namespace std;

struct dist
{
    long vertexA;
    long vertexB;
    double length;
};

struct Node
{
    long vertexNum;
    double edge;
};

struct ListNode
{
    long vertexNum;
    double edge;
    ListNode *next;
};

struct Vertex
{
    ListNode *adj;
    bool known;
    double dist;
    long path;

public:
    Vertex();
    void newMyListNode(long n, double m);
    ~Vertex();
};

struct myPoint
{
    long numSrc;
    long to;
    double distance;
};

struct Path
{
    int *citys;
    double len;
};

class Graph
{
private:
    long numVertex, numEdge;
    long origin, destination;
    int numPoint;
    double theDist;
    priority_queue<Node> myQueue;
    struct Vertex *myVertex;
    myPoint* pointArray;
    Path actualPath;
    QVector<long> SectionalPath;
    bool found;
    void InitmyVertex();
    void InitOriDes();
    void Dijkstra();
    void ClearMyRoom(QVector<long> &path);
    void Print(QVector<long> &path);
    void PrintPath(int vertex, QVector<long> &path);
    void SavePath(QVector<long> &path);
    void CleanRoom(QVector<long> &vPath);
    void InitNum(QVector<long> &point);
    void Initdis();
    void InitOriPath_Cal();
    void SavaBestPath(QVector<long> &path);
    Path GetNext(Path &p);
    void SA();

public:

    void GenerateGraph(long n, long m, QList<dist> &myList);
    void GetDist(long n, long m, QVector<long> &path, double &theDist);
    void BestPath(QVector<long> &point, QVector<long> &path);
    ~Graph();
};

bool operator<(const Node a, const Node b);

#endif
