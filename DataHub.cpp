/*
This class makes it easy to send and received data as a master in a multi-master network.
This file is made by Simon Gotfred and commented by Casper Bang-Hansen
*/



//Includes
#include <string>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <cstring>
#include <sstream>
#include "DataHub.h"


//Class DataHub


/*
Constructor (Initial setup)

----->Please dont use the constructor since its already generated.<-----

The constructors sets the datahubs address up and sets the wiringPiI2C library up.
The constructor also sets the pins up.
*/

DataHub::DataHub(int address)
{
	timeOutPin = 7;
	timeOutLength = 25;

	fd = wiringPiI2CSetup(address);
	wiringPiSetup();
	pinMode(timeOutPin,INPUT);
	usleep(2000000);
}

/*
Read fuction.

The read fuction checks if the input ID is wallet (1-19),
after this the fuction calles the transmit fuction were the input is: "ID:".
If the transmit was succesfull the read fuction calles the request fuction
and returns the data from the specific data point from the datahub.

I/O:

Input:
	Channel ID (int)

Output:
	Succesfull:
		Data from datahub (string)
	Failure:
		"ERROR" (string)
*/

std::string DataHub::read(int ID)
{
	if (ID < 20 && ID > 0)
	{
		if (transmit((to_string(ID) + ":")))
		{
			return request();
		}
		else
		{
			return "ERROR";
		}
	}

	return "ERROR";
}

/*
Write fuction.

The write fuction checks if the input ID is wallet (1-19) and if there is data,
after this the fuction calles the transmit fuction were the input is: "ID:Data".
If this was succesfull the datahub will now update the data on the specific data point.

I/O:

Input:
	Channel ID (int)
	data (string)

Output:
	Succesfull:
		true (boolean)
	Failure:
		false (boolean)
*/

bool DataHub::write(int ID, std::string data)
{
	if (data.length() > 0 && ID < 20 && ID > 0)
	{
		return transmit((to_string(ID) + ";" + data));
	}

	return false;
}

/*
Transmit fuction.

The Transmit fuction starts of by adding a '#' to the end of a message, so the datahub knows when the message ends.
The fuction also checks if pin 7 goes low (the control pin), if it does i breaks from the loop, otherwise return false
If the control pin is low the transmit fuction sends one byte at a time to the datahub.

I/O:

Input:
	message (string)

Output:
	Succesfull:
		true (boolean)
	Failure:
		false (boolean)
*/

bool DataHub::transmit(std::string message)
{
	message = message + "#";

	/*
	checks if pin 7 goes (control pin) low within 25 cycles, if the pin goes low it breaks from the loop,
	otherwise the transmit fuction returns false. 
	*/

	for(int i = 0; i < 25; i++)
	{		
		if(digitalRead(7) == 0)
		{
			break;
		}
				
		if( i == 24)
		{
			return false;
		}
	}

	//std::cout << "TRANSMITTING...\n";

	//send the message on byte at a time.

	for (int i = 0 ; i < message.length() ; i++)
	{
		wiringPiI2CWrite(fd,message[i]);
	}
	return true;
}

/*
Request fuction.

The request fuction starts of by sleeping for 6 micro sec.
This is because the datahub have to have enough time to set pin 7 (control pin) to high. 
Afterwards the requets fuction checks if the data is shorter then 64 and if the data doesn't contains a '#'
if this is true i keeps looping.

The request function now receives data from the datahub and returns the data without a the '#' at the end of the string.

I/O:

Input:

Output:
	Succesfull:
		Data from datahub (string)
	Failure:
		"ERROR" (string)
*/

std::string DataHub::request()
{
	//sleeps
	usleep(6000);
	std::string data = "";

	//Checks if the data is shorter then 64 and if the data doesnt contains a '#'
	while (data.length() < 64 && (int(data.find("#")) < 0))
	{
		/*
		checks if pin 7 goes (control pin) low within 25 cycles, if the pin goes low it breaks from the loop,
		otherwise the request fuction returns "ERROR".
		*/
		for(int i = 0; i < 25; i++)
		{
			if(digitalRead(7) == 1)
			{
				break;
			}
			if( i == 24)
			{
				return "ERROR";
			}
		}
		
		//Every new char the client gets from the datahub gets added to this sessions data
		data = data + (char)wiringPiI2CRead(fd);
		
		//TO DO
		if (data.length() == 0)
		{
			break;
		}
	}
	//returns data without the '#' at the end.
	return  data.substr(0,data.find("#",0));
}

//makes a new datahub object.
DataHub datahub = DataHub(0x08);


//----------------TO-DO----------------
/*
	Fix data.length() == 0 in the request fuction




*/