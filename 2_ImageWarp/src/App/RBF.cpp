#include"RBF.h"
#include<iostream>

Rbf::Rbf()

{
}

Rbf::Rbf(std::vector<QPoint> p_list, std::vector<QPoint> q_list)
	:Warp(p_list, q_list)
{
	calculate_r();
	calculate_alp();
}

Rbf::~Rbf()
{
}

void Rbf::calculate_r()
{
	for (int i = 0; i < mp_points.size(); i++)
	{
		double min_r = 1e16;
		for (int j = 0; j < mp_points.size(); j++)
		{
			if (i == j)
				continue;
			
			double dis = Eu_Dis(mp_points[i], mp_points[j]);
			if (dis < min_r)
				min_r = dis;
		}
		
		r_list.push_back(min_r);
	}
}

double Rbf::basis(int i, double d) const
{
	return pow((pow(d, 2) + pow(r_list[i], 2)), u);
}
void Rbf::calculate_alp()
{
	int n = mp_points.size();
	alp_list.resize(n, 2);
	MatrixXd A(n, n);										//A1*alpha1+p1=q1 A2*alpha2+p2=q2
	MatrixXd b(n, 2); 										//bi=qi-pi

	Vector2d tmp;
	//calculate A,b
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			A(i, j) = basis(j, Eu_Dis(mp_points[i], mp_points[j]));
		}
		tmp = Vector2d(mq_points[i].x(), mq_points[i].y()) - Vector2d(mp_points[i].x(), mp_points[i].y());
		b(i, 0) = tmp[0];
		b(i, 1) = tmp[1];
	}

	//calculate alp_list
	alp_list = A.lu().solve(b);
}

Vector2d Rbf::warp_func(QPoint p) const
{
	Vector2d result{ p.x(),p.y() };

	for (int i = 0; i < mp_points.size(); i++)
	{
		result += Vector2d(alp_list(i, 0), alp_list(i, 1)) * basis(i, Eu_Dis(mp_points[i], p));
	}
	
	return result;
}