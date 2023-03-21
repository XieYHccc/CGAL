#include "Poisson_Fuse.h"
#include <iostream>
#include <assert.h>
#include <vector>

Poisson_Fuse::Poisson_Fuse()
{
}

Poisson_Fuse::Poisson_Fuse(MatrixXi mask)
	:mask(mask)
{
	get_number();
	get_in_pos();
	get_A();
}

Poisson_Fuse::~Poisson_Fuse()
{
}

MatrixXd Poisson_Fuse::discrete_Poisson_Solve(MatrixXd f_data, MatrixXd g_data, bool mix)
{
	// make sure the same size of the data
	assert(f_data.rows() == mask.rows() && g_data.rows() == mask.rows());
	assert(f_data.cols() == mask.cols() && g_data.cols() == mask.cols());
	mixed = mix;

	int w = mask.cols();
	int h = mask.rows();

	VectorXd X;																	// AX=b
	VectorXd b = get_in_value(f_data, g_data);
	X.resize(num_in, 1);
	b.resize(num_in, 1);
	// adjust b with g_data
	int order1 = 0;
	for (int i = 0; i <h ; i++)
	{
		for (int j = 0; j < w; j++)
		{
			if (mask(i, j) == 1)
			{
				if (mask(i, j + 1) == 0)
					b[order1] += g_data(i, j + 1);
				if (mask(i, j - 1) == 0)
					b[order1] += g_data(i, j - 1);
				if (mask(i + 1, j) == 0)
					b[order1] += g_data(i + 1, j);
				if (mask(i - 1, j) == 0)
					b[order1] += g_data(i - 1, j);
				order1++;
			}
		}
	}
	//get X
	X = solver.solve(b);

	// make sure the value is in [0,255]
	for (int i = 0; i < X.rows(); i++)
	{
		if (X[i] < 0)
			X[i] = 0;
		if (X[i] > 255)
			X[i] = 255;
	}

	// fill region matrix with X
	MatrixXd result = MatrixXd::Zero(h, w);

	int order = 0;
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			if (mask(i, j) == 1)
			{
				result(i, j) = X[order];
				order++;
			}
		}
	}

	return result;
}

void Poisson_Fuse::get_in_pos()
{
	int w = mask.cols();
	int h = mask.rows();
	int order = 0;
	in_pos = MatrixXi::Zero(num_in, 2);


	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			if (mask(i,j)==1)
			{
				in_pos(order, 0) = j;
				in_pos(order, 1) = i;
				order++;
			}
		}
	}
}

void Poisson_Fuse::get_number()
{
	int w = mask.cols();
	int h = mask.rows();
	int num_i = 0;

	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			if (mask(i, j) == 1)
				num_i++;
		}
	}
	num_in = num_i;
}

VectorXd Poisson_Fuse::get_in_value(MatrixXd f_data, MatrixXd g_data)
{
	int w = mask.cols();
	int h = mask.rows();
	VectorXd in_value_list(num_in);
	if (mixed)
	{
		int order = 0;

		for (int i = 0; i < h; i++)
		{
			for (int j = 0; j < w; j++)
			{
				if (mask(i, j) == 1)
				{
					//compare gradient value first
					double gradient_f = pow(f_data(i, j + 1) - f_data(i, j), 2) + pow(f_data(i + 1, j) - f_data(i, j), 2);
					double gradient_g = pow(g_data(i, j + 1) - g_data(i, j), 2) + pow(g_data(i + 1, j) - g_data(i, j), 2);
					//get divergence of f_data
					if(gradient_f>gradient_g)
						in_value_list[order] = 4 * f_data(i, j) - f_data(i + 1, j) - f_data(i - 1, j) - f_data(i, j + 1) - f_data(i, j - 1);
					else
						in_value_list[order] = 4 * g_data(i, j) - g_data(i + 1, j) - g_data(i - 1, j) - g_data(i, j + 1) - g_data(i, j - 1);

					order++;
				}
			}
		}
	}
	else
	{
		// get divergence of f_data, no need to calculate gradient
		int order = 0;

		for (int i = 0; i < h; i++)
		{
			for (int j = 0; j < w; j++)
			{
				if (mask(i, j) == 1)
				{
					in_value_list[order] = 4 * f_data(i, j) - f_data(i + 1, j) - f_data(i - 1, j) - f_data(i, j + 1) - f_data(i, j - 1);
					order++;
				}
			}
		}
	}
	cout <<"get_in_value() down" << endl;
	return in_value_list;
}

void Poisson_Fuse::get_A()
{
	A=SparseMatrix<double>(num_in, num_in);
	vector<Triplet<double>> T_list;

	int h = mask.rows();
	int w = mask.cols();
	int order = 0;												// the order of the unknown variables

	// since A is symmetrical, we just need to calculate right top triangle
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			if (mask(i, j) == 1)
			{
				T_list.push_back(Triplet<double>(order, order, 4));
				if (mask(i, j + 1) == 1)
				{
					T_list.push_back(Triplet<double>(order, order+1, -1));
					if(order<num_in)
						T_list.push_back(Triplet<double>(order+1, order, -1));

				}
				if (mask(i + 1, j) == 1)
				{
					// get the order of the point (i+1,j)
					int tmp_order = order;
					for (int k = j + 1; k < w; k++)
					{
						if (mask(i, k) == 1)
							tmp_order++;
					}
					for (int k = 0; k <= j; k++)
					{
						if (mask(i+1, k) == 1)
							tmp_order++;
					}
					T_list.push_back(Triplet<double>(order, tmp_order, -1));
					if(order<num_in)
						T_list.push_back(Triplet<double>(tmp_order, order, -1));
				}
				order++;
			}
		}
	}
	A.setFromTriplets(T_list.begin(), T_list.end());
	// compress the matrix to save memory
	A.makeCompressed();
	solver.compute(A);
	cout << "get A done" << endl;

}