#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>

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


void split(const std::string & line, std::vector<std::string> & vectorExplosion){
	
}



int main(int argc, char * argv[]){

	if(argc != 3){
		std::cerr << "Error usage: " << argv[0] << " <process-file> { first | best | next | worst }\n";
		return 1;
	}

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
			int startTime;

			std::vector<std::string> explodedLine;

			split(readBuffer, explodedLine);
			
			while(false){
				std::cout << "ADDING" << std::endl;
				struct timingInformation info;
				info.symbol = symbol;
				info.size = size;
				info.startTime = startTime;
				timeline.push_back(info);
			}
		}

		std::cout << timeline.size() << std::endl;
		
		while(false){
			if(globalTime > timeline[timeline.size() - 1].finishTime || timeline.size() == 0){
				break;
			}
			if(globalTime == 0){
				printMemory();
			}
			globalTime++;
		}
		printMemory();
	}else{
		std::cerr << "Error cannot open input file, quitting...\n";
		return 1;
	}

	return 0;
}
