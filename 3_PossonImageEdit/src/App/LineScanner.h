#pragma once

#include<list>
#include<vector>
#include <Eigen\Sparse>
#include <Eigen\Dense>
#include<iostream>

using namespace std;
using namespace Eigen;

struct tagEdge				// The complete information of the edge should be with line value, namely ymin
{
	int ymax;					
	int xmin;				// x value of the point with max y value of the line
	double dx;				// 1/k  
	double x;				// current x position of intersection point

	bool operator<(const tagEdge& other) const
	{
		return this->x < other.x;
	}
};

class LineScanner
{
public:
	LineScanner();
	~LineScanner();

	MatrixXi getMask(int width, int height, vector<int> p_x_list, vector<int> p_y_list);		// get the mask of interior region
};