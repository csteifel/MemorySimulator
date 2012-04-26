/*

Colin Steifel
Greg Yauney
Operating systems project 2

*/

#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>

struct timingInformation {
	unsigned int startTime;
	unsigned int finishTime;
	char symbol;
	int size;
};

void printMemory();
void defragment();
void split(const std::string &, std::vector<std::string> &);

unsigned int globalTime = 0;
std::vector<char> memory(2400, '.');

void defragment(){
	std::cout << "Performing defragmentation...\n";
	int symbols = 0;
	char lastSymbol = '.';
	int free = 0;
	for(size_t i = 0; i < memory.size(); i++){
		//Find the first free spot and move from first nonfree all the way down
		int span = -1;
		if(memory[i] == '.'){
			for(size_t j = i+1; j < memory.size(); j++){
				//Find the first nonfree spot and start moving down from there
				if(memory[j] != '.' && span == -1){
					span = j - i;
				}
				if(span != -1){
					memory[j-span] = memory[j];
					//Do the counting
					if(memory[j] != '.' && memory[j] != lastSymbol){
						symbols++;
						lastSymbol = memory[j];
					}
				}
			}
			//If there wasn't a nonfree spot from i then there are no more to move down
			if(span == -1){
				free = memory.size() - i;
				break;
			}
		}
	}


	std::cout << "Defragmentation completed.\n";
	std::cout << "Relocated " << symbols << " processes to create a free memory block of " << free << " units (" << ((double) free/(double) memory.size() * 100) << "% of total memory).\n";
	printMemory();
}


void deleteMemory(char symbol){
	for(size_t i = 0; i < memory.size(); i++){
		if(memory[i] == symbol){
			memory[i] = '.';
		}
	}
}

void printMemory(){
	int count = 0;
	std::cout << "Memory at time " << globalTime << ":\n";
	for(size_t i = 0; i < memory.size(); i++){
		std::cout << memory[i];
		count++;
		if(count == 80){
			std::cout << std::endl;
			count = 0;
		}
	}
	std::cout << std::endl;
}

int addFirst(char symbol, int size, int done=0){
	bool inserted = false;
	for(size_t i = 0; i < memory.size(); i++){
		size_t available = 0;
		if(memory[i] == '.'){
			for(size_t j = i; j < memory.size(); j++){
				if(memory[j] == '.'){
					if((int) available < size){
						//Not enough available space yet but keep going
						++available;
					}else{
						//There is enough space here
						break;
					}
				}else{
					break;
				}
			}

			//There was enough space so insert the process now
			if((int) available == size){
				for(size_t x = 0; (int) x < size; x++){
					memory[x+i] = symbol;
				}
				inserted = true;
				return 1;
			}
		}
	}
	if(!inserted && done == 0){
		defragment();
		return addFirst(symbol, size, 1);
	}else{
		return -1;
	}
}


int addNext(char symbol, int size, int done=0){
	bool inserted = false;
	bool wrap = true;
	static size_t startFrom;
	if(startFrom == 0){
		wrap = false;
	}

	int i = startFrom;
	while(i < (int) memory.size()){
		size_t available = 0;
		if(memory[i] == '.'){
			for(size_t j = (size_t) i; j < memory.size(); j++){
				if(memory[j] == '.'){
					if((int) available < size){
						//Not enough available space yet but keep going
						++available;
					}else{
						//There is enough space here
						break;
					}
				}else{
					break;
				}
			}

			//There was enough space so insert the process now
			if((int) available == size){
				for(size_t x = 0; (int) x < size; x++){
					memory[x+i] = symbol;
				}
				startFrom = i+size;
				inserted = true;
				return 1;
			}
		}
		if(i+1 == (int) memory.size() && wrap){
			i = 0;
			wrap = false;
		}else{
			i++;
		}
	}
	if(!inserted && done == 0){
		defragment();
		return addFirst(symbol, size, 1);
	}else{
		return -1;
	}
}



int addBest(char symbol, int size, int done=0){
	bool inserted = false;
	int bestStart = -1;
	int bestAvailable = -1;
	for(size_t i = 0; i < memory.size(); i++){
		size_t available = 0;
		if(memory[i] == '.'){
			for(size_t j = i; j < memory.size(); j++){
				if(memory[j] == '.'){
					++available;
				}else{	
					break;
				}
			}

		}

		if(available >= (size_t) size){
			if(bestAvailable > (int) available || bestStart == -1){
				bestStart = i;
				bestAvailable = available;
			}
		}
		if(memory[i] == '.')
			i = i + available;
	}

	if(bestStart != -1){
		for(size_t x = 0; x < (size_t) size; x++){
			memory[bestStart+x] = symbol;
		}
		inserted = true;
		return 0;
	}

	if(!inserted && done == 0){
		defragment();
		return addFirst(symbol, size, 1);
	}else{
		return -1;
	}
}

int addWorst(char symbol, int size, int done=0){
	bool inserted = false;
	int bestStart = -1;
	int bestAvailable = -1;
	for(size_t i = 0; i < memory.size(); i++){
		size_t available = 0;
		if(memory[i] == '.'){
			for(size_t j = i; j < memory.size(); j++){
				if(memory[j] == '.'){
					++available;
				}else{	
					break;
				}
			}

		}

		if(available >= (size_t) size){
			if(bestAvailable < (int) available || bestStart == -1){
				bestStart = i;
				bestAvailable = available;
			}
		}
		if(memory[i] == '.')
			i = i + available;
	}

	if(bestStart != -1){
		for(size_t x = 0; x < (size_t) size; x++){
			memory[bestStart+x] = symbol;
		}
		inserted = true;
		return 0;
	}

	if(!inserted && done == 0){
		defragment();
		return addFirst(symbol, size, 1);
	}else{
		return -1;
	}
}


int addNoncontig(char symbol, int size){
	int avail = 0;
	for(size_t i = 0; i < memory.size(); i++){
		if(memory[i] == '.'){
			avail++;
		}
	}

	if(avail < size) return -1;
	for(size_t i = 0; i < memory.size() && size > 0; i++){
		if(memory[i] == '.'){
			memory[i] = symbol;
			size--;
		}
	}
	return 0;
}


void split(const std::string & line, std::vector<std::string> & vectorExplosion){
	size_t offset = 0;
	size_t position;
	while( (position = line.find_first_of(" \t\n", offset))){
		if(position == std::string::npos){
			position = line.length();
			vectorExplosion.push_back(line.substr(offset, position-offset));
			break;
		}
		vectorExplosion.push_back(line.substr(offset, position-offset));
		offset = position+1;
	}
}


/*bool sortingComp(timingInformation a, timingInformation b){
	if(a.finishTime < b.finishTime) return true;
	return false;
}*/

int main(int argc, char * argv[]){

	if(argc != 3){
		std::cerr << "Error usage: " << argv[0] << " <process-file> { first | best | next | worst }\n";
		return 1;
	}
	
	std::string type = argv[2]; 
	//Initialize operating system memory
	for(int i = 0; i < 80; i++){
		memory[i] = '#';
	}
		
	std::vector<struct timingInformation> timeline;
	std::string readBuffer;
	std::ifstream inputFile;
	//int numProcesses = 0;
	inputFile.open(argv[1]);
	if(inputFile.is_open()){
		bool first = true;
		unsigned int max;
		while(inputFile.eof() == false){
			std::getline(inputFile, readBuffer);
			if(readBuffer.length() == 0){
				break;
			}
			if(first){
				first = false;
				continue;
			}
			
			char symbol;
			int size;
			std::vector<std::string> explodedLine;
			split(readBuffer, explodedLine);
			symbol = explodedLine[0][0];
			size = atoi(explodedLine[1].c_str());
			for(size_t q = 2; q < explodedLine.size(); q+=2){
				struct timingInformation info;
				info.symbol = symbol;
				info.size = size;
				info.startTime = atoi(explodedLine[q].c_str());
				info.finishTime = atoi(explodedLine[q+1].c_str());
				if(max < info.finishTime)
					max = info.finishTime;
				timeline.push_back(info);
			}
		}

		unsigned int printNext = 0;
		while(true){
			if(timeline.size() == 0){
				break;
			}
			for(size_t x = 0; x < timeline.size(); x++){
				if(timeline[x].finishTime == globalTime){
					deleteMemory(timeline[x].symbol);
				}
			}
			for(size_t x = 0; x < timeline.size(); x++){
				if(timeline[x].startTime == globalTime){
					int res = 0;
					if(type == "first"){
						res = addFirst(timeline[x].symbol, timeline[x].size);
					}else if(type == "best"){
						res = addBest(timeline[x].symbol, timeline[x].size);
					}else if(type == "worst"){
						res = addWorst(timeline[x].symbol, timeline[x].size);
					}else if(type == "next"){
						res = addNext(timeline[x].symbol, timeline[x].size);
					}else if(type == "noncontig"){
						res = addNoncontig(timeline[x].symbol, timeline[x].size);
					}else{
						std::cerr << "Unknown algorithm type, quitting...\n";
						return 1;
					}
					if(res == -1){
						std::cerr << "OUT-OF-MEMORY\n";
						return 1;
					}
				}
			}


			if(globalTime == printNext){
				printMemory();
				if(printNext >= max){
					std::cout << "End of simulation\n";
					break;
				}
				std::cin >> printNext;
				if(printNext == 0){
					break;
				}
				while(printNext < globalTime){
					std::cout << "Not a valid time to print...enter again\n";
					std::cin >> printNext;
				}
			}
			globalTime++;
		}
	}else{
		std::cerr << "Error cannot open input file, quitting...\n";
		return 1;
	}

	return 0;
}
