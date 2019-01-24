
//Includes
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>
#include <opencv/cv.h>
#include <fstream>
#include "SunMoon.h"
#define _USE_MATH_DEFINES



//#include "../MoonShot/Datahub/DataHub.h"

#pragma warning(disable : 4996)

//namespace
using namespace cv;
using namespace std;

//MoonLocation
using namespace Astronomy;
struct tm m_datumzeit;
int m_Zone;
double latitude = 55.782604799999994; //Output Datahub
double longitude = 12.528025600000001;  //Output Datahub
double deltaT = 65.0;
string inputLALO;
ifstream infile;


//moonPos
double moonPosAz;  //Input Datahub
double moonPosAlt;  //Intput Datahub


//clock
struct tm gmt;
int startSec;

//txt file
std::ofstream outfile;
std::string logLineFirst = "Clock \t \t \t latitude \t \t longitude \t \t MoonPosAz \t \t MoonPosAl\n";
int alreadyRan = 0;

const std::string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%d-%X", &tstruct);
	return buf;
	//22-20:28:11
}

void logFile() {
	std::string logLine = "";
	outfile.open("LogFile.txt", std::ios_base::app);
	std::string TimeStamp = currentDateTime();
	logLine += (TimeStamp) + "\t\t";
	logLine += std::to_string(latitude) + "\t\t" + std::to_string(longitude) + "\t\t";
	logLine += std::to_string(moonPosAz) + "\t\t" + std::to_string(moonPosAlt) + "\t\t";
	outfile << logLine;
	//std::cout << logLine;
	outfile.close();
}

void Time() {
	time_t now = time(NULL);
#ifdef _MSC_VER
	memset(&gmt, 0, sizeof(struct tm));
	memset(&m_datumzeit, 0, sizeof(struct tm));
	localtime_s(&m_datumzeit, &now);
	gmtime_s(&gmt, &now);
#else
	m_datumzeit = *localtime(&now);
	struct tm gmt = *gmtime(&now);
#endif
	time_t utc = mktime(&gmt);
	if (gmt.tm_isdst == 1) utc -= 3600;
	m_Zone = (int)(difftime(now, utc) / 3600.0);
	
}

void MoonLocation() {
	Astronomy::SunMoon(latitude, longitude, &m_datumzeit, deltaT, m_Zone);
}

bool is_number(const std::string& s){
	try{
		std::stod(s);
	} catch(...){
		return false;
	}
	std::cout << s << " was a number\n";
	return true;
}


void readWriteData(){
	if(3 == stoi(currentDateTime().substr(10,10)) || 8 == stoi(currentDateTime().substr(10,10))){		
		infile.open("LALO.txt");
		getline(infile, inputLALO);
		cout << inputLALO << "\n";
		if(inputLALO.length() != 0 ){
			if(is_number(inputLALO.substr(0,inputLALO.find('/'))) == true && is_number(inputLALO.substr(inputLALO.find('/')+1))){
				latitude = stod(inputLALO.substr(0,inputLALO.find('/')));
				longitude = stod(inputLALO.substr(inputLALO.find('/')+1));
				std::cout << latitude << "		" << longitude << "\n";
			} else {
				std::cout << "ERROR NO DOUBLE FOUND\n";
			}
		} else {
			readWriteData();
			std::cout << "Trying to Read Again\n";
		}
		infile.close();
		ofstream newFile("AzAlt.txt");
		if(newFile.is_open()){
			newFile << moonPosAz << "/" << moonPosAlt;
			std::cout << "SUCCESS\n";
		} else {
			std::cout << "ERROR\n";
		}
		newFile.close();
		alreadyRan = 1;
	}
	if(startSec != stoi(currentDateTime().substr(10,10))){
		alreadyRan = 0;
	}
}


int main() {
	outfile.open("LogFile.txt", std::ios_base::app);
	outfile << logLineFirst;
	outfile.close();

	VideoCapture cap(0); // open the default camera
	if (!cap.isOpened()) {  // check if we succeeded
		return -1;
	}
	startSec = stoi(currentDateTime().substr(10,10));
	
	while (1) {
		//Time
		Time();

		//MoonLocation
		MoonLocation();
		//picture
		Mat imgImport;
		cap.read(imgImport);

		imshow("img", imgImport);
		if (imgImport.empty()) {
			std::cout << "error: image did'nt load ";
			return -2;
		}
		std::string saveNameMoon = "../MoonShot/Picture/";
		cv::String Path2(saveNameMoon);
		vector<cv::String> fileNameSaveMoon;
		cv::glob(Path2, fileNameSaveMoon, true);
		cv::Mat imgGrayScale;


		//Convert to grayscale
		cv::cvtColor(imgImport, imgGrayScale, CV_BGR2GRAY);
		//mean
		Scalar average = mean(imgGrayScale);

		//Scalar->string
		string AVG = std::to_string(average[0]);


		//std::cout << TorF + "\n";
		cv::imwrite(saveNameMoon + AVG + ".jpg", imgImport);

		//std::cout << startSec << ":		" << stoi(currentDateTime().substr(10,10)) << "\n";
		readWriteData();
		//running logfile
		logFile();
	}
	// hold windows open
	system("pause");
	return(0);
}
