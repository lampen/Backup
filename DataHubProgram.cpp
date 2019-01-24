#include "DataHub.h"
#include <time.h>
#include <fstream>

string TEMP = "";
string longLat = "55.782604799999994/12.528025600000001";

const std::string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%d-%X", &tstruct);
	return buf;
	//22-20:28:11
}

int main(void){

string inputLALO;
ifstream infile;
	int startSec = stoi(currentDateTime().substr(10,10));
	while(1)
	{	
		//std::cout << startSec << ":		" << stoi(currentDateTime().substr(10,10)) << "\n";
		if(1 == stoi(currentDateTime().substr(10,10)) || 6 == stoi(currentDateTime().substr(10,10))){		
			infile.open("AzAlt.txt");
			getline(infile, inputLALO);
			cout << inputLALO << "\n";
			infile.close();	
			
			datahub.write(8,inputLALO);
			usleep(50000);
			
			TEMP = datahub.read(2);
			if(TEMP != "ERROR"){
				longLat = TEMP;
			}
			usleep(50000);
			
			ofstream newFile("LALO.txt");
			if(newFile.is_open()){
				newFile << longLat;
			} else {
				std::cout << "ERROR\n";
			}
			newFile.close();
		}
	}
	return 0;
}


