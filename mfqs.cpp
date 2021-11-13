#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <sstream>
#include <map>
#include <iterator>
#include <algorithm>
using namespace std;

struct Process{
	int processNum;
	int burstTime;
	int arrivalTime;
	int priority;
	int deadline;
	int io;
	int waitTime;
	int enter;
	int leave;
};

int main() {
	ifstream processFile ("25Test.txt");
	string mystring;
	queue<Process> processQueue;
	map<int, Process> processMap;
	bool skipLine = true;
	if(processFile.is_open()){
		while(1){
			getline(processFile, mystring);
			if (skipLine){
				skipLine = false;
				continue;
			}
			Process holder;
			int startpoint = 0;
			string processNum = "";
			for(int i = 0; i < mystring.length(); i++){
				if(startpoint == 0){
					if(mystring[i] == '\t'){
						stringstream words(processNum);
						words >> holder.processNum;
						if(holder.processNum < 0){
							continue;
						}
						startpoint++;
						processNum = "";
						continue;
					}
					processNum += mystring[i];
				}else if(startpoint == 1){
					if(mystring[i] == '\t'){
						stringstream words(processNum);
						words >> holder.burstTime;
						if(holder.burstTime < 0){
							continue;
						}
						startpoint++;
						processNum = "";
						continue;
					}
					processNum += mystring[i];
				}else if(startpoint == 2){
					if(mystring[i] == '\t'){
						stringstream words(processNum);
						words >> holder.arrivalTime;
						if(holder.arrivalTime < 0){
							continue;
						}
						startpoint++;
						processNum = "";
						continue;
					}
					processNum += mystring[i];
				}else if(startpoint == 3){
					if(mystring[i] == '\t'){
						stringstream words(processNum);
						words >> holder.priority;
						if(holder.priority < 0){
							continue;
						}
						startpoint++;
						processNum = "";
						continue;
					}
					processNum += mystring[i];
				}else if(startpoint == 4){
					if(mystring[i] == '\t'){
						stringstream words(processNum);
						words >> holder.deadline;
						if(holder.deadline < 0){
							continue;
						}
						startpoint++;
						processNum = "";
						continue;
					}
					processNum += mystring[i];
				}else if(startpoint == 5){
					processNum += mystring[i];
					if(i = (mystring.length() - 1)){
						stringstream words(processNum);
						words >> holder.io;
						if(holder.io < 0){
							continue;
						}
						processMap.insert(pair<int, Process>(holder.arrivalTime, holder));
						startpoint ++;
						processNum = "";
						continue;
					}
				}
		        }
		if(processFile.eof()){
			break;
		}		
		}
	}
	else{
		cout << "Couldn't open file\n";
	}
	map<int, Process>::iterator it = processMap.begin();
	while(it != processMap.end()){
		int arrivalTime =  it->first;
		Process process = it->second;
		processQueue.push(process);
		it++;
	}
	
	while(!processQueue.empty()){
		Process remover = processQueue.front();
		cout << "PID: " << remover.processNum << endl;
		cout << "Burst Time: " << remover.burstTime << endl;
		cout << "Arrival Time: " << remover.arrivalTime << endl;
		cout << "Priority: " << remover.priority << endl;
		cout << "Deadline: " << remover.deadline << endl;
		cout << "I/O: " << remover.io << endl;
		cout << endl;
		
		processQueue.pop();
		processFile.close();
	}
	return 0;
}



