#include <Idw.h>
#include <iostream>
#include<assert.h>
Idw::Idw()
{
}

Idw::Idw(std::vector<QPoint> p_list, std::vector<QPoint> q_list)
	:Warp(p_list, q_list),func(interp_func(p_list,q_list))
{
	func.cal_T();
}

Idw::~Idw()
{
}

Vector2d Idw::warp_func(QPoint p) const
{
	return func.y(p);
}

Idw::interp_func::interp_func()
{
}

Idw::interp_func::interp_func(std::vector<QPoint> p_list, std::vector<QPoint> q_list)
	:p_points(p_list),q_points(q_list)
{
	cal_T();
}

Idw::interp_func::~interp_func()
{
}

double Idw::interp_func::Euclid_dis(QPoint p1, QPoint p2) const
{
	return sqrt(pow((p1.x() - p2.x()), 2) + pow((p1.y() - p2.y()), 2));
}
void Idw::interp_func::cal_T()
{
	T = std::vector<Matrix2d>(p_points.size());
	//calculate T[i]
	for (int i = 0; i < p_points.size(); i++)
	{
		Matrix2d A;
		Vector2d t1, t2, b1, b2;
		A.fill(0.0);				// ((A, O), (O, A)) is coefficient matrix
		b1.fill(0.0);				// (b1^T, b2^T)^T is constant term vector
		b2.fill(0.0);
		for (int j = 0; j < p_points.size(); j++)
		{
			if (i == j)
				continue;
			int dpx = (p_points[j] - p_points[i]).x();
			int dpy = (p_points[j] - p_points[i]).y();
			int dqx = (q_points[j] - q_points[i]).x();
			int dqy = (q_points[j] - q_points[i]).y();
			double cof = 1.0/Euclid_dis(p_points[i], p_points[j]);

			//  the 1st equation
			A(0, 0) += dpx * dpx * cof;
			A(0, 1) += dpx * dpy * cof;
			b1[0] += dpx * dqx * cof;

			//  the 2nd equation
			A(1, 0) += dpy * dpx * cof;
			A(1, 1) += dpy * dpy * cof;
			b1[1] += dpy * dqx * cof;

			//  the 3rd equation
			b2[0] += dpx * dqy * cof;

			//  the 4th equation
			b2[1] += dpy * dqy * cof;
		}


		t1 = A.lu().solve(b1);	// (t1^T, t2^T)^T is the solution
		t2 = A.lu().solve(b2);

		T[i](0, 0) = t1[0];
		T[i](0, 1) = t1[1];
		T[i](1, 0) = t2[0];
		T[i](1, 1) = t2[1];
	}

}

Vector2d Idw::interp_func::local_approximation_func(int i, QPoint p) const				//fi(p) = qi + Ti(p - pi)
{
	assert(0 <= i && i < p_points.size());

	QPoint diff = p - p_points[i], qi = q_points[i];
	Vector2d ans{ 0,0 };
	ans[0] = T[i](0, 0) * diff.x() + T[i](0, 1) * diff.y() + qi.x();
	ans[1] = T[i](1, 0) * diff.x() + T[i](1, 1) * diff.y() + qi.y();

	return ans;
}

double Idw::interp_func::w_cof(int i, const QPoint& p) const
{
	assert(0 <= i && i < p_points.size());

	double sum = 0.0;
	for (int j = 0; j < p_points.size(); j++)
	{
		if (p == p_points[j])
		{
			return i == j ? 1.0 : 0.0;
		}
		else
		{
			sum += 1.0/ pow(Euclid_dis(p_points[j], p),2);
		}
	}

	assert(fabs(sum) >= W_EPSILON);	// otherwise invert_dist(i,p) <= W_EPSILON, which means p == p_[i] and the function would have returned
	
	return (1.0 / pow(Euclid_dis(p_points[i], p),2)) / sum;
}
 
Vector2d Idw::interp_func::y(QPoint p) const		//f(p)=Σ(wi(p)*fi(p))
{
	Vector2d result{ 0,0 };

	for (int i = 0; i < p_points.size(); i++)
	{
		result += w_cof(i, p) * local_approximation_func(i, p);
	}
	
	return result;
}
