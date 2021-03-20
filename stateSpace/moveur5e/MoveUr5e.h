// Class for simulating a linear state-space model
//  x_{k+1}=Ax_{k}+Bu_{k}
//  y_{k} = Cx_{k}
//  starting from the initial state x0;
// The class is implemented using the Eigen library:
// http://eigen.tuxfamily.org/index.php?title=Main_Page
//*******************************************************************************************************************************
// AUTHOR: Aleksandar Haber,
// DEVELOPMENT: June 2020-
// VERSION DATE: June 25, 2020
// NOTE: This implementation is not optimized for extremely large-scale problems, since all the matrices are passed by value 
//*******************************************************************************************************************************

#ifndef MOVEUR5E_H
#define MOVEUR5E_H
#include <iostream>
#include<eigen3/Eigen/Dense>
#include<tuple>
#include<string>
#include<fstream>
#include<vector>
#include <ur_rtde/rtde_control_interface.h>
#include <ur_rtde/rtde_receive_interface.h>

using namespace Eigen;

// MatrixXd is an Eigen typdef for Matrix<double, Dynamic, Dynamic>


class MoveUr5e {
public:
	
	MoveUr5e();
	// default constructor
	// sets all the variables to 1x1 dimensional matrices and sets all the variables to zero

	~MoveUr5e();

	void saveData(std::string YYFile) const;
	// this function saves the data in "*.csv" files

	MatrixXd openData(std::string fileToOpen);
	// this function opens the "*.csv" file "fileToOpen" that stores a matrix, and loads the entries into the Eigen matrix MatrixXd
	
	void move();

private:
	// MatrixXd is an Eigen typdef for Matrix<double, Dynamic, Dynamic>
	MatrixXd inputSequence;  // input sequnce, dimensions: m\times  timeSamples
	std::vector< float > YY;
	int m, n, r, timeSamples; //m - input dimension, n- state dimension, r-output dimension, timeSamples- number of time samples 

};
#endif
