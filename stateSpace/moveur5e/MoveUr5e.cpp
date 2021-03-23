#include "MoveUr5e.h"
#include <iostream>
#include <ur_rtde/rtde_control_interface.h>
#include <ur_rtde/rtde_receive_interface.h>
#include <cmath>        // std::abs
#include <thread>
#include <chrono>
using namespace std;
using namespace ur_rtde;
using namespace std::chrono;

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

MoveUr5e::MoveUr5e()
{
}

MoveUr5e::~MoveUr5e()
{
}

void MoveUr5e::saveData(std::string realPlantFile) const
{
	const static IOFormat CSVFormat(FullPrecision, DontAlignCols, ", ", "\n");

	std::ofstream filerealPlant(realPlantFile);
	filerealPlant.open(realPlantFile, std::ofstream::out | std::ofstream::trunc);
	filerealPlant.close();
	filerealPlant.open(realPlantFile, std::ofstream::out);
	if (filerealPlant.is_open())
	{
		// Send data to the stream
		for(int i = 0; i < realPlant.size(); ++i)
		{
			filerealPlant << realPlant.at(i) << "\n";
		}
		filerealPlant.close();
	}
}

MatrixXd MoveUr5e::openData(std::string fileToOpen)
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

void MoveUr5e::move()
{
	string path_to_csv = "csv/";
	std::string inputSequenceFile = path_to_csv + "inputFile.csv";
	MatrixXd inputSequence; 
	inputSequence=openData(inputSequenceFile);
    RTDEControlInterface rtde_control("172.30.10.1");
    RTDEReceiveInterface rtde_receive("172.30.10.1");
    std::vector<double> init_q = rtde_receive.getActualQ();

    float Ts = 0.001; //sample Time
    float Tstop = 10;
    float steps = Tstop / Ts;
	printf("steps %f\n", steps);
    int max_vel = 3;
    std::vector<double> startpos = {-2.57, -1.57, 1.57, 0, 0, 0};
    std::vector<double> joint_speed = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    rtde_control.moveJ(startpos);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));


	
	for (int j = 0; j < steps; j++)
	{		
		//printf("\n");
		joint_speed[0] = sgn<float>(inputSequence(0,j))*max_vel;
		//std::cout << "Here is joint_speed[0]:\n" << joint_speed[0] << std::endl;
		auto t_start = high_resolution_clock::now();
		rtde_control.speedJ(joint_speed, std::abs(inputSequence(0,j)),0.0001);
		auto t_stop = high_resolution_clock::now();
    	auto t_duration = std::chrono::duration<double>(t_stop - t_start);
		if (t_duration.count() < Ts)
		{
			std::this_thread::sleep_for(std::chrono::duration<double>(Ts - t_duration.count()));
		}
		realPlant.push_back(rtde_receive.getActualQ().at(0));
		
	}
	rtde_control.speedStop();
    rtde_control.stopScript();
	rtde_receive.disconnect();
	saveData(path_to_csv + "realPlant.csv");
	return;
}	
