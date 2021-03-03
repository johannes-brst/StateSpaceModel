#include "SimulateSystem.h"
#include <iostream>
#include <ur_rtde/rtde_control_interface.h>
#include <ur_rtde/rtde_receive_interface.h>
#include <cmath>        // std::abs
#include <thread>
#include <chrono>
using namespace ur_rtde;
using namespace std::chrono;

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

SimulateSystem::SimulateSystem()
{
	m = 0; n = 0; r = 0;
	A.resize(1, 1); A.setZero();
	B.resize(1, 1); B.setZero();
	C.resize(1, 1); C.setZero();
	D.resize(1, 1); D.setZero();
	x0.resize(1, 1); x0.setZero();
	inputSequence.resize(1, 1); inputSequence.setZero();
	simulatedStateSequence.resize(1, 1); simulatedStateSequence.setZero();
	simulatedOutputSequence.resize(1, 1); simulatedOutputSequence.setZero();
	timeRowVector.resize(1, 1); timeRowVector.setZero();
}

SimulateSystem::SimulateSystem(MatrixXd Amatrix, MatrixXd Bmatrix, MatrixXd Cmatrix, MatrixXd Dmatrix, MatrixXd initialState, MatrixXd inputSequenceMatrix)
{
	A = Amatrix; B = Bmatrix; C = Cmatrix; x0 = initialState; inputSequence = inputSequenceMatrix;
	n = A.rows();
	m = B.cols();
	r = C.rows();
	YY = {};
	timeSamples = inputSequence.cols();

	simulatedOutputSequence.resize(r, timeSamples); simulatedOutputSequence.setZero();
	simulatedStateSequence.resize(n, timeSamples);	simulatedStateSequence.setZero();
	
	timeRowVector.resize(1, timeSamples);

	for (int i = 0; i < timeSamples; i++)
	{
		timeRowVector(0, i) = i + 1;
	}

}

SimulateSystem::~SimulateSystem()
{
}

std::tuple<MatrixXd, MatrixXd, MatrixXd> SimulateSystem::getStateOuputTime()
{
	std::tuple<MatrixXd, MatrixXd, MatrixXd>   result(simulatedStateSequence, simulatedOutputSequence, timeRowVector);
	return result;
}

void SimulateSystem::saveData(std::string AFile, std::string BFile, std::string CFile, std::string DFile, std::string x0File, std::string inputSequenceFile, std::string simulatedStateSequenceFile, std::string simulatedOutputSequenceFile, std::string YYFile) const
{
	const static IOFormat CSVFormat(FullPrecision, DontAlignCols, ", ", "\n");

	std::ofstream fileA(AFile);
	if (fileA.is_open())
	{
		fileA << A.format(CSVFormat);
		fileA.close();
	}

	std::ofstream fileB(BFile);
	if (fileB.is_open())
	{
		fileB << B.format(CSVFormat);
		fileB.close();
	}

	
	std::ofstream fileC(CFile);
	if (fileC.is_open())
	{
		fileC << C.format(CSVFormat);
		fileC.close();
	}



	std::ofstream fileX0(x0File);
	if (fileX0.is_open())
	{
		fileX0 << x0.format(CSVFormat);
		fileX0.close();
	}


	std::ofstream fileInputSequence(inputSequenceFile);
	if (fileInputSequence.is_open())
	{
		fileInputSequence << inputSequence.format(CSVFormat);
		fileInputSequence.close();
	}

	std::ofstream fileSimulatedStateSequence(simulatedStateSequenceFile);
	if (fileSimulatedStateSequence.is_open())
	{
		fileSimulatedStateSequence << simulatedStateSequence.format(CSVFormat);
		fileSimulatedStateSequence.close();
	}

	std::ofstream fileSimulatedOutputSequence(simulatedOutputSequenceFile);
	if (fileSimulatedOutputSequence.is_open())
	{
		fileSimulatedOutputSequence << simulatedOutputSequence.format(CSVFormat);
		fileSimulatedOutputSequence.close();
	}
	std::ofstream fileYY(YYFile);
	fileYY.open(YYFile, std::ofstream::out | std::ofstream::trunc);
	fileYY.close();
	fileYY.open(YYFile, std::ofstream::out);
	if (fileYY.is_open())
	{
		// Send data to the stream
		for(int i = 0; i < YY.size(); ++i)
		{
			fileYY << YY.at(i) << "\n";
		}
		fileYY.close();
	}

    
    


}

MatrixXd SimulateSystem::openData(std::string fileToOpen)
{

	// the inspiration for creating this function was drawn from here (I did NOT copy and paste the code)
	// https://stackoverflow.com/questions/34247057/how-to-read-csv-file-and-assign-to-eigen-matrix
	// NOTE THAT THIS FUNCTION IS CALLED BY THE FUNCTION: SimulateSystem::openFromFile(std::string Afile, std::string Bfile, std::string Cfile, std::string x0File, std::string inputSequenceFile)
	
	// the input is the file: "fileToOpen.csv":
	// a,b,c
	// d,e,f
	// This function converts input file data into the Eigen matrix format



	// the matrix entries are stored in this variable row-wise. For example if we have the matrix:
	// M=[a b c 
	//	  d e f]
	// the entries are stored as matrixEntries=[a,b,c,d,e,f], that is the variable "matrixEntries" is a row vector
	// later on, this vector is mapped into the Eigen matrix format
	std::vector<double> matrixEntries;

	// in this object we store the data from the matrix
	std::ifstream matrixDataFile(fileToOpen);

	// this variable is used to store the row of the matrix that contains commas 
	std::string matrixRowString;

	// this variable is used to store the matrix entry;
	std::string matrixEntry;

	// this variable is used to track the number of rows
	int matrixRowNumber = 0;


	while (std::getline(matrixDataFile, matrixRowString)) // here we read a row by row of matrixDataFile and store every line into the string variable matrixRowString
	{
		std::stringstream matrixRowStringStream(matrixRowString); //convert matrixRowString that is a string to a stream variable.

		while (std::getline(matrixRowStringStream, matrixEntry,',')) // here we read pieces of the stream matrixRowStringStream until every comma, and store the resulting character into the matrixEntry
		{
			matrixEntries.push_back(std::stod(matrixEntry));   //here we convert the string to double and fill in the row vector storing all the matrix entries
			}
		matrixRowNumber++; //update the column numbers
	}

	// here we conver the vector variable into the matrix and return the resulting object, 
	// note that matrixEntries.data() is the pointer to the first memory location at which the entries of the vector matrixEntries are stored;
	return Map<Matrix<double, Dynamic, Dynamic, RowMajor>> (matrixEntries.data(), matrixRowNumber, matrixEntries.size() / matrixRowNumber);

}


void SimulateSystem::openFromFile(std::string Afile, std::string Bfile, std::string Cfile, std::string Dfile,std::string x0File, std::string inputSequenceFile)
{
	// this function acts as a constructor in some way. 
	// call this function after a default constructor is being called

	A = openData(Afile);
	B = openData(Bfile);
	C = openData(Cfile);
	D = openData(Dfile);
	x0= openData(x0File);
	inputSequence=openData(inputSequenceFile);

	n = A.rows();
	m = B.cols();
	r = C.rows();
	timeSamples = inputSequence.cols();

	simulatedOutputSequence.resize(r, timeSamples); simulatedOutputSequence.setZero();
	simulatedStateSequence.resize(n, timeSamples);	simulatedStateSequence.setZero();

	timeRowVector.resize(1, timeSamples);

	for (int i = 0; i < timeSamples; i++)
	{
		timeRowVector(0, i) = i + 1;
	}

	
}

void SimulateSystem::runSimulation()
{
	
    RTDEControlInterface rtde_control("127.0.0.1");
    RTDEReceiveInterface rtde_receive("127.0.0.1");
    std::vector<double> init_q = rtde_receive.getActualQ();

    float sT = 0.002; //sample Time
    float Tstop = 20;
    float steps = Tstop / sT;
	printf("steps %f\n", steps);
	float inputDelay = 0.00;
	float outputDelay = 0.00;
	float stepInputDelay = inputDelay / sT;
	float stepOutputDelay = outputDelay / sT;
    int max_vel = 1;
    std::vector<double> startpos = {-2.57, -1.57, 1.57, 0, 0, 0};
    std::vector<double> joint_speed = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    rtde_control.moveJ(startpos);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	// obwohl hier zeit disrete simuliert wird ergeben die DAten noch keinen Sinn? Doch tustin methode anwenden? was genau passiert dabei?
	// https://de.mathworks.com/help/control/ref/c2d.html
	// https://matlab.mathworks.com/
	// https://de.mathworks.com/help/control/ug/continuous-discrete-conversion-methods.html
	simulatedStateSequence.col(0) = x0;
	simulatedOutputSequence.col(0) = C * x0;

	for (int j = 0; j < steps; j++)
	{		
		//printf("\n");
		
		if (((j - stepInputDelay) > 0) && (j - stepOutputDelay) > 0)
		{
			simulatedStateSequence.col(j+1) = A * simulatedStateSequence.col(j) + B * inputSequence.col(j - stepInputDelay);
			simulatedOutputSequence.col(j) = C * simulatedStateSequence.col(j) + D * inputSequence.col(j - stepOutputDelay);
		}	
		//std::cout << "Here is simulatedStateSequence.col(j):\n" << simulatedStateSequence.col(j) << std::endl;
		//std::cout << "Here is simulatedOutputSequence.col(j):\n" << simulatedOutputSequence.col(j) << std::endl;
	
		joint_speed[0] = sgn<float>(inputSequence(0,j))*max_vel;
		//std::cout << "Here is \n joint_speed.at(0): " << joint_speed.at(0) << " | inputSequence(0,j): " << inputSequence(0,j) << std::endl;
		auto t_start = high_resolution_clock::now();
		rtde_control.speedJ(joint_speed, std::abs(inputSequence(0,j)),0.0001);
		auto t_stop = high_resolution_clock::now();
    	auto t_duration = std::chrono::duration<double>(t_stop - t_start);
		if (t_duration.count() < sT)
		{
			std::this_thread::sleep_for(std::chrono::duration<double>(sT - t_duration.count()));
		}
		YY.push_back(rtde_receive.getActualQ().at(0));
		
	}
	rtde_control.speedStop();
    rtde_control.stopScript();
	rtde_receive.disconnect();

	//float MAPE = mean(abs((YY-YSIM)./YY))*100; %mean absolute percentage error
	float result = 0;
	for (int i = 0; i < YY.size(); i++){
		
		result =+ std::abs((YY.at(i) - simulatedOutputSequence(0,i))/YY.at(i));
		printf("simulatedOutputSequence(0,i): %f\n",simulatedOutputSequence(0,i));
	}
	result = result / YY.size();
	result = result * 100;
	printf("MAPE = %f\n", result);
}	
