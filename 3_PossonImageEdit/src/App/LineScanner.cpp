#include "LineScanner.h"
#include <assert.h>
LineScanner::LineScanner()
{
}

LineScanner::~LineScanner()
{
}



MatrixXi LineScanner::getMask(int height, int width, vector<int> p_x_list, vector<int> p_y_list)
{
	// make sure the same size of x list and y list
	assert(p_x_list.size() == p_y_list.size());

	// the mask to be returned
	MatrixXi mask = MatrixXi::Zero(height, width);

	//get ymax and ymin in points list
	int ymin_all = 10000;
	int ymax_all = 0;
	for (int i = 0; i < p_y_list.size(); i++)
	{
		ymin_all = p_y_list[i] < ymin_all ? p_y_list[i] : ymin_all;
		ymax_all = p_y_list[i] > ymax_all ? p_y_list[i] : ymax_all;
	}
	int num_line = ymax_all - ymin_all + 1;

	//get new edge table from vertices
	vector<list<tagEdge>> NET(num_line, list<tagEdge>(0));
	for (size_t i = 0; i < p_x_list.size(); i++)
	{
		int front = (int)i - 1;
		if (front < 0)front += (int)p_y_list.size();
		if (p_y_list[i] > p_y_list[front])
		{
			tagEdge tmp;
			tmp.ymax = p_y_list[i];
			tmp.xmin = p_x_list[front];
			tmp.dx = (p_x_list[front] - p_x_list[i]) / (double)(p_y_list[front] - p_y_list[i]);
			tmp.x = p_x_list[front];
			int order = p_y_list[front] - ymin_all;
			NET[order].push_back(tmp);
		}
		else if(p_y_list[i]< p_y_list[front])
		{
			tagEdge tmp;
			tmp.ymax = p_y_list[front];
			tmp.xmin = p_x_list[i];
			tmp.dx = (double)(p_x_list[front] - p_x_list[i]) / (p_y_list[front] - p_y_list[i]);
			tmp.x = p_x_list[i];
			int order = p_y_list[i] - ymin_all;
			NET[order].push_back(tmp);
		}
		else
		{
			continue;
		}
	}
	//cout << "NET is correct" << endl;

	// get the active edge table
	vector<list<tagEdge>> AET(num_line, list<tagEdge>(0));
	list<tagEdge> AE;
	for (int i = 0; i < num_line; i++)
	{
		
		if (AE.size() > 0)
		{
			// juge which edges need to be eliminated
			for (auto itr = AE.begin(); itr != AE.end();)
			{
				if (itr->ymax <= (i + ymin_all))
					AE.erase(itr++);
				else
					itr++;
			}

			// update the data in AED
			for (auto itr = AE.begin(); itr != AE.end(); itr++)
				itr->x += itr->dx;
		}

		// judege which edges need to be added in
		{
			if (NET[i].size() > 0)
			{
				for (auto itr = NET[i].begin(); itr != NET[i].end(); itr++)
				{
					AE.push_back(*itr);
				}
			}
		}
			
		//sort active edges according to the x value and put it in to AET
		AE.sort();
		AET[i] = AE;
	}
	//cout << "AET is correct" << endl;

	// fill the mask region with 1
	for (int i = 0; i < num_line; i++)
	{
		int sum = 0;
		for (auto itr = AET[i].begin(); itr != AET[i].end(); itr++)
		{
			if (sum % 2 == 0)
			{
				double xmin = itr->x + 1;
				double xmax = next(itr)->x;
				for (int j = xmin; j < xmax; j++)
				{
					mask(i + ymin_all, j) = 1;
				}
			}
			sum += 1;
		}
	}
	//cout << "fill process is correct" << endl;

	// wipe off the region boundary
	for (size_t i = 0; i < p_y_list.size(); i++)
	{
		int front = (int)i - 1;
		if (front < 0)front += (int)p_y_list.size();
		if (p_y_list[front] == p_y_list[i])
		{
			int min, max;
			if (p_x_list[front] < p_x_list[i])
			{
				min = p_x_list[front];
				max = p_x_list[i];
			}
			else
			{
				min = p_x_list[i];
				max = p_x_list[front];
			}
			for (int j = min; j < max; j++)
			{
				mask(p_y_list[i], j) = 0;
			}
		}
	}
	return mask;
}