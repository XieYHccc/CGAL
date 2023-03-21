#pragma once

#include<QLine>
#include<QImage>
#include<vector>
#include<Eigen/Dense>
#define W_EPSILON 1e-16

using namespace Eigen;

class Warp
{

public:
	Warp();
	Warp(std::vector<QPoint> p_list, std::vector<QPoint> q_list);
	virtual ~Warp();

	void Warp_Image(QImage* image) const;					// Warp the image and return the warpped image
	virtual Vector2d warp_func(QPoint p) const = 0;			// the interpolate function implemented by sub-class
	void Fill_holl(QImage* image);

public:
	enum algr
	{
		Idw = 0,
		Rbf = 1
	};

protected:
	double Eu_Dis(QPoint p1, QPoint p2) const;					// calculate euclid distance between two points

protected:
	std::vector<QPoint> mp_points;  //store the point pairs to be used to interpolate
	std::vector<QPoint> mq_points;	

};