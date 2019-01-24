#ifndef DATAHUB_H
#define DATAHUB_H

/*
This headerfile uses Datahub.cpp, so you can send and received data as a master in a multi-master network.
This file is made by Simon Gotfred and commented by Casper Bang-Hansen
*/


//include
#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <cstring>
#include <sstream>

//namespace
using namespace std;

//Class Datahub

class DataHub
{
	public:
		DataHub(int address);
		std::string read(int ID);
		bool write(int ID, std::string data);
		
	private:
		int fd;
		int timeOutPin;
		int timeOutLength;
		bool transmit(std::string message);
		std::string request();
};

extern DataHub datahub;

#endif
