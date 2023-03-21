#pragma once
#include"Warp.h"

class Rbf : public Warp
{
public:
	Rbf();
	Rbf(std::vector<QPoint> p_list, std::vector<QPoint> q_list);
	~Rbf();
	Vector2d warp_func(QPoint p) const;					// f(p)=Σαi(d(p,pi)^2+ri^2)^u +p

private:
	void calculate_r();									
	void calculate_alp();				
	double basis(int i, double d) const;

private:
	std::vector<double> r_list;							// store the ri for pi
	Matrix<double,Dynamic,2> alp_list;						// store the R^2 coefficient alpha
	const double u = 0.5;
};