#include "optimumPath.h"

Vertex::Vertex()
{
    adj = new ListNode;
    adj->vertexNum = 0;
    adj->edge = 0;
    adj->next = nullptr;
}

void Vertex::newMyListNode(long n, double m)
{
    ListNode *ptr = new ListNode;
    ptr->vertexNum = n;
    ptr->edge = m;
    ptr->next = adj->next;
    adj->next = ptr;
}

Vertex::~Vertex()
{
    ListNode *ptr;
    while (adj->next != nullptr)
    {
        ptr = adj->next;
        delete[] adj;
        adj = ptr;
    }
}

void Graph::GenerateGraph(long n, long m, QList<dist> &myList)
{
	numVertex = n;

	numEdge = m;

    InitmyVertex();

    QList<dist>::iterator plist;

    for (plist = myList.begin(); plist != myList.end(); ++plist)
    {
        myVertex[(*plist).vertexA].newMyListNode((*plist).vertexB, (*plist).length);
    }
}

void Graph::InitmyVertex()
{
    myVertex = new struct Vertex[numVertex + 1];
}

void Graph::GetDist(long n, long m, QVector<long> &path, double &dist)	//interface
{
    origin = n;
    destination = m;

    ClearMyRoom(path);

    InitOriDes();

    Dijkstra();

    Print(path);

    SavePath(path);

	dist = theDist;
}

void Graph::ClearMyRoom(QVector<long> &path)
{
    QVector<long>().swap(path);
    while(myQueue.size())myQueue.pop();
}

void Graph::InitOriDes()
{
    long i;

    for (i = 0; i < numVertex; ++i)
    {
        myVertex[i].known = 0;
        myVertex[i].dist = INFINITE;
        myVertex[i].path = NOTVERTEX;
    }

    found = 1;

    myVertex[origin].dist = 0;

    Node originNode;
    originNode.vertexNum = origin;
    originNode.edge = 0;
    myQueue.push(originNode);

    for (ListNode *ptr = myVertex[origin].adj->next; ptr != nullptr; ptr = ptr->next)
    {
        myVertex[ptr->vertexNum].dist = ptr->edge;
        myVertex[ptr->vertexNum].path = origin;
    }
}

void Graph::Dijkstra()
{
    Node topQueueNode, pushQueueNode;

    while (!myQueue.empty())
    {
        topQueueNode = myQueue.top();
        myQueue.pop();

        if (myVertex[topQueueNode.vertexNum].known == true)
        {
            continue;
        }

        myVertex[topQueueNode.vertexNum].known = 1;

        if (topQueueNode.vertexNum == destination)
        {
            return;
        }

        for (ListNode *ptr = myVertex[topQueueNode.vertexNum].adj->next; ptr != nullptr; ptr = ptr->next)
        {
            if (!myVertex[ptr->vertexNum].known)
            {
                if ((myVertex[topQueueNode.vertexNum].dist == 0) || ((myVertex[topQueueNode.vertexNum].dist + ptr->edge) < myVertex[ptr->vertexNum].dist))
                {
                    myVertex[ptr->vertexNum].dist = myVertex[topQueueNode.vertexNum].dist + ptr->edge;

                    pushQueueNode.vertexNum = ptr->vertexNum;
                    pushQueueNode.edge = myVertex[ptr->vertexNum].dist;

                    myQueue.push(pushQueueNode);
                    myVertex[ptr->vertexNum].path = topQueueNode.vertexNum;
                }
            }
        }
    }

    found = 0;
	theDist = INFINITE;
}

void Graph::Print(QVector<long> &path)
{
    if(found)
    {
		theDist = myVertex[destination].dist;
        path.push_back(destination);
        PrintPath(destination, path);
    }
}

void Graph::PrintPath(int vertex, QVector<long> &path)
{
    if (myVertex[vertex].path != NOTVERTEX)
    {
        path.push_back(myVertex[vertex].path);
        PrintPath(myVertex[vertex].path, path);
    }
}

void Graph::SavePath(QVector<long> &path)
{
    reverse(path.begin(), path.end());
}

Graph::~Graph()
{
    delete[] myVertex;
	delete[] pointArray;
}

void Graph::CleanRoom(QVector<long> &vPath)
{
    QVector<long>().swap(vPath);
}

void Graph::InitNum(QVector<long> &point)
{
	numPoint = point.size();
    QVector <long>::iterator Iter = point.begin();
    QVector <long>::iterator Iter1 = point.begin();
	myPoint onePoint;
	pointArray = new myPoint[numPoint * numPoint];

	for (int i = 0; i < numPoint; ++i)	//编号初始化
	{
		onePoint.numSrc = *Iter;
		for (int j = 0; j < numPoint; ++j)
		{
			*(pointArray + i * numPoint + j) = onePoint;
		}
		++Iter;
	}

    for (int i = 0; i < numPoint; ++i)	//编号初始化
        {
            for (int j = 0; j < numPoint; ++j)
            {
                (pointArray + i * numPoint + j)->to = *(Iter1++);
            }
            Iter1 = point.begin();
        }
}

void Graph::Initdis()
{
	double theDist;

	for (int i = 0; i < numPoint; ++i)	//任意两点间距离初始化
	{
		for (int j = i + 1; j < numPoint; ++j)
		{
            (pointArray + i * numPoint + j)->to = (pointArray + j * numPoint + i)->numSrc;
			GetDist((pointArray + i * numPoint + j)->numSrc, (pointArray + j * numPoint + i)->numSrc, SectionalPath, theDist);
			(pointArray + i * numPoint + j)->distance = (pointArray + j * numPoint + i)->distance = theDist;
		}
	}
}

void Graph::InitOriPath_Cal()
{
	actualPath.citys = new int[numPoint + 1];
	actualPath.len = 0;

	for (int i = 0; i < numPoint; ++i)	//初始化原路径并计算该路径长度
	{
		*(actualPath.citys + i) = (pointArray + i * numPoint)->numSrc;
		if (i != numPoint - 1)
		{
			actualPath.len += (pointArray + i * numPoint + i + 1)->distance;
		}
		else
		{
			actualPath.len += (pointArray + i * numPoint)->distance;
		}
	}
	*(actualPath.citys + numPoint) = pointArray->numSrc;
}

void Graph::SavaBestPath(QVector<long> &path)
{
	for (int i = 0; i < numPoint; ++i)	//近似最优路径保存
	{
		//path.push_back(actualPath.citys[i]);

		GetDist(actualPath.citys[i], actualPath.citys[i + 1], SectionalPath, theDist);

		if (SectionalPath.size() == 0)
		{
			CleanRoom(path);
			return;	
		}
		if (path.size() != 0)
		{
			path.pop_back();
		}
        path.append(SectionalPath);
	}
}

void Graph::BestPath(QVector<long> &point, QVector<long> &path)
{

	CleanRoom(path);	//清理存储最终路径的结构

	InitNum(point);

	if (numPoint > 2)
	{
		Initdis();

		InitOriPath_Cal();

		SA();	//模拟退火

		SavaBestPath(path);	//存储最终路径
	}
	else if (numPoint == 2)
	{
		GetDist(point[0], point[1], path, theDist);
        QVector<long> tmp = path;
		reverse(tmp.begin(), tmp.end());
		path.pop_back();
        path.append(tmp);
	}	

}

Path Graph::GetNext(Path &p)
{
	// Modify by DD: 确保x！=y
	int x = 0, y = 0;
	while (x == y)
	{
		x = rand() % (numPoint - 1) + 1;
		y = rand() % (numPoint - 1) + 1;
	}

    Path ans;
    ans.citys = new int[numPoint + 1];

    for (int i = 0; i < numPoint + 1; ++i)
    {
        ans.citys[i] = p.citys[i];
    }

	swap(ans.citys[x], ans.citys[y]);
	ans.len = 0;

    int t1, t2;
    for (int i = 0; i < numPoint; i++)
    {
        for (t1 = 0; (pointArray[t1].numSrc != ans.citys[i]) && (t1 < numPoint * numPoint); t1 += numPoint);
        for (t2 = 0; (pointArray[t1 + t2].to != ans.citys[i + 1]) && (t2 < numPoint); ++t2);
        ans.len += (pointArray + t1 + t2)->distance;
    }

	return ans;
}

void Graph::SA()
{
    double t = Temperature;
	srand(time(NULL));
	Path curPath = actualPath;
	Path newPath = actualPath;
	int P_L = 0;    // 连续找到更差结果的次数
	int P_F = 0;    // while循环次数

	while (1)       //外循环，主要更新参数t，模拟退火过程
	{
		for (int i = 0; i < ILOOP; i++)    //内循环，寻找在一定温度下的最优值
		{
			newPath = GetNext(curPath);
			double dE = newPath.len - curPath.len;
			if (dE < 0)   //如果找到更优值，直接更新
			{
				curPath = newPath;
				P_L = 0;
				P_F = 0;
			}
			else
			{
				double rd = rand() / (RAND_MAX + 1.0);

				// Modify by DD: dE取负数才有可能接受更差解,否则e>1
				double e = exp(-dE / t);
				if (e > rd && e < 1)   //如果找到比当前更差的解，以一定概率接受该解，并且这个概率会越来越小
				{
					curPath = newPath;
				}
				P_L++;
			}
			if (P_L > LIMIT)
			{
				P_F++;
				break;
			}
		}

		// Modify by DD: 记录全局最优解
		if (curPath.len < actualPath.len)
		{
			actualPath = curPath;
		}

		if (P_F > OLOOP || t < EPS)
		{
			break;
		}
		t *= DELTA;
	}
}

bool operator<(const Node a, const Node b)
{
    return a.edge > b.edge;
}
