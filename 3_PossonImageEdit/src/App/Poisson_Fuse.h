#pragma once

#include <Eigen\Sparse>
#include <Eigen\Dense>

using namespace Eigen;
using namespace std;

//implemment the algorithm in paper "Poisson Image Editing" written by P.Perez



class Poisson_Fuse								// guided intepolation for 2 dimensions									
{												       
public:											
	Poisson_Fuse();
	Poisson_Fuse(MatrixXi mask);
	~Poisson_Fuse();

	MatrixXd discrete_Poisson_Solve(MatrixXd f_data, MatrixXd g_data, bool mixed);	
																			// get_in_Value() and get_edge_value()
	
private:

	void get_in_pos();									// get edge_pos and in_pos
	void get_number();									// get num_in and num_edge
	void get_A();										// get coefficient matrix and solver

	VectorXd get_in_value(MatrixXd f_data, MatrixXd g_data);					// get in_value_list

private:
	bool mixed;											// to judge whether use the value from both f and g
	SparseMatrix<double> A;								// AX=b
	SimplicialCholesky<SparseMatrix<double>> solver;	// solver.solve(b)
	int num_in;											// number of region points, also it is the number of unknown variables

	MatrixXi mask;										// mask of the region
	Matrix<int, Dynamic, 2> in_pos;						// store the position list of region points





};