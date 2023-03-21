#pragma once

#include "Warp.h"

class Idw :public Warp
{
public:
	Idw();
	Idw(std::vector<QPoint> p_list, std::vector<QPoint> q_list);
	~Idw();

	Vector2d warp_func(QPoint p) const;

private:
	class interp_func												// interpolation function using mp_points and mq_points
	{
	public:
		interp_func();
		interp_func(std::vector<QPoint> p_list, std::vector<QPoint> q_list);
		~interp_func();
		void cal_T();
		Vector2d y(QPoint p) const;											// calculate pixel's new position

	private:
		double Euclid_dis(QPoint p1, QPoint p2) const;					// calculate euclid distance between two points 	
		Vector2d local_approximation_func(int i, QPoint p) const;				// fi(p) = qi + Ti(p - pi)
		double w_cof(int i, const QPoint& p) const;								// wi					


	private:
		std::vector<QPoint> p_points;
		std::vector<QPoint> q_points;
		std::vector<QPoint> local_list;								// store the local function value
		std::vector<Matrix2d> T;									//local approximation matrix 
																	// fi(p) = qi + Ti(p - pi)
																	
	} func;

	
};