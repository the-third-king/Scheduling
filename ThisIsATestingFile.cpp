#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <vector>
using namespace std;

struct Process {
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

/*  This process swaps two values
    @Param a: first element being swapped
    @Param b: second element being swapped
*/
void swap(Process* a, Process* b) {
    Process temp = *a;
    *a = *b;
    *b = temp;
}

/*  This method swaps values if they are out of order
    @Param pVector: data structure that is being sorted
    @Param low: lower bound index
    @Param high: higher bound index
*/
int partition(vector<Process>& pVector, int low, int high) {
    int pivot = pVector[high].arrivalTime;
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++) {
        if (pVector[j].arrivalTime < pivot) {
            i++;
            swap(&pVector[i], &pVector[j]);
        }else if(pVector[j].arrivalTime == pivot){
            if(pVector[j].arrivalTime < pVector[high].priority){
                i++;
                swap(&pVector[i], &pVector[j]);
            }
        }
    }
    swap(&pVector[i + 1], &pVector[high]);
    return (i + 1);
}

/*  This method sorts a data structure
    @Param pVector: data structure being sorted
    @Param low: low bound index
    @Param high: higher bound index
*/
void quickSort(vector<Process>& pVector, int low, int high) {
    if (low < high) {
        int part = partition(pVector, low, high);
        quickSort(pVector, low, part - 1);
        quickSort(pVector, part + 1, high);
    }
}

/*  This is a helper method to get process attributes
    @Param mystring: line from the input file
    @Param intAttribute: the process attribute we are changing
    @Param strAttribute: the attribute value as pulled from the file in string format
    @Param startpoint: the start point of reading in the line to keep track of correct attribute we are reading
    @Param i: iterator in the for loop to read characters of a line
    @Param skipProcess: if a process characteristic breaks a rule for a process attribute
*/
void getProcessHelper(string& mystring, int& intAttribute, string& strAttribute, int& startpoint, int& i, bool& skipProcess) {
    //find the process attribute
    if (mystring[i] == '\t') {
        stringstream words(strAttribute);
        words >> intAttribute;
        if (intAttribute < 0) {
            skipProcess = true;
        }
        startpoint++;
        strAttribute = "";
    }
    else {
        strAttribute += mystring[i];
    }
}

/*  This method parses the process file to get all of the processes then puts them into a sorted hashmap
    @Param processFile: the file being parsed
    @Param processMap: hashmap holding the sorted process by arrival time
*/
void getProcesses(ifstream& processFile, vector<Process>& pVector) {
    string mystring;
    bool skipLine = true;
    //if (processFile.is_open()) {
    while (1) {
        getline(processFile, mystring);
        if (skipLine) {
            skipLine = false;
            continue;
        }
        Process holder;
        int startpoint = 0;
        string processNum = "";
        bool skipProcess = false;
        for (int i = 0; i < (int)mystring.length(); i++) {
            if (startpoint == 0) {
                getProcessHelper(mystring, holder.processNum, processNum, startpoint, i, skipProcess);
            }
            //find the burst time
            else if (startpoint == 1) {
                getProcessHelper(mystring, holder.burstTime, processNum, startpoint, i, skipProcess);
            }
            //find the arrival time
            else if (startpoint == 2) {
                getProcessHelper(mystring, holder.arrivalTime, processNum, startpoint, i, skipProcess);
            }
            //find the priority
            else if (startpoint == 3) {
                getProcessHelper(mystring, holder.priority, processNum, startpoint, i, skipProcess);
            }
            //find the deadline
            else if (startpoint == 4) {
                getProcessHelper(mystring, holder.deadline, processNum, startpoint, i, skipProcess);
            }
            //find the I/O
            else if (startpoint == 5) {
                processNum += mystring[i];
                if (i == (int)(mystring.length() - 1)) {
                    stringstream words(processNum);
                    words >> holder.io;
                    if (holder.io < 0 || holder.io >= holder.burstTime || holder.arrivalTime >= holder.deadline || holder.burstTime == 0) {
                        skipProcess = true;
                    }
                    if (!skipProcess) {
                        pVector.push_back(holder);
                    }
                }
            }
        }
        if (processFile.eof()) {
            break;
        }
    }
}

/*  This method prints all of the end statistics
    @Param tatList: list of turn-around times
    @Param waitList: list of wait times
*/
void printEndAvg(vector<int>& tatList, vector<int>& waitList) {
    double tatAvg = 0;
    double waitAvg = 0;
    for (auto& it : tatList) {
        tatAvg += (double)it;
    }
    tatAvg = tatAvg / double(tatList.size());
    for (auto& it : waitList) {
        waitAvg += (double)it;
    }
    waitAvg = waitAvg / double(waitList.size());
    cout << "Average turn-around time: " << fixed << tatAvg << endl;
    cout << "Average wait time: " << fixed << waitAvg << endl;
}

/*  This method prints every processes enter, leave, and id when process is done
    @Param process: the process that has completed
    @Param tatList: list of turn-around times
    @Param waitList:
*/
void printProcessCharacteristics(Process process, vector<int>& tatList, vector<int>& waitList, bool printAll) {
    if (printAll) {
        cout << "Process ID: " << process.processNum << endl;
        cout << "Process start time: " << process.arrivalTime << endl;
        cout << "Process end time: " << process.leave << endl;
    }
    tatList.push_back(process.leave - process.arrivalTime);
    waitList.push_back((process.leave - process.arrivalTime) - process.waitTime);
}

/*  This method checks for ageing in the final process
    @Param fcfsQ: FCFS queue to check the first element of
    @Param rrQ1: first queue to put the aged process into
    @Param time: the time of the CPU clock to check age
*/
void checkFcfsFront(queue<Process>& fcfsQ, queue<Process>& rrQ1, int ageing, int time) {
    if (!fcfsQ.empty()) {
        Process process = fcfsQ.front();
        while((time - process.enter) >= ageing){
            rrQ1.push(process);
            process.enter = 0;
            fcfsQ.pop();
            if (!fcfsQ.empty()) {
                process = fcfsQ.front();
            }
            else {
                break;
            }
        }
    }
}

/*  Helper method to mfqs to increment the io queue
    @Param ioQ: the I/O queue
    @Param rrQ1: the first round robin queue
*/
void ioQueueIncrementer(queue<Process>& ioQ, queue<Process>& rrQ1) {
    for (int i = 0; i < (int)ioQ.size(); i++) {
        Process process = ioQ.front();
        ioQ.pop();

        if (process.io <= 0) {
            rrQ1.push(process);
        }
        else {
            process.io -= 1;
            ioQ.push(process);
        }
    }
}

/*  Helper method to mfqs to check for new processes arriving
    @Param processQueue: the queue of unarrived processes
    @Param rrq1: the first round robin queue
    @Param rrq2: the second round robin queue
    @Param rrq3: the third round robin queue
    @Param rrq4: the fourth round robin queue
    @Param fcfsQ: the first come first serve queue
    @Param io: the I/O queue
    @Param time: the time on the CPU clock
*/
void checkProcessQueue(queue<Process> &processQueue, queue<Process>&rrQ1, queue<Process>&rrQ2, queue<Process>&rrQ3, queue<Process>&rrQ4, queue<Process>&fcfsQ, queue<Process>&ioQ, int &time){
    if (!processQueue.empty()) {
        Process process = processQueue.front();
        while (time >= process.arrivalTime) {
            processQueue.pop();
            process.enter = 0;
            process.waitTime = process.burstTime;
            rrQ1.push(process);
            if (!processQueue.empty()) {
                process = processQueue.front();
            }
            else {
                break;
            }
        }
        if (time < process.arrivalTime && rrQ1.empty() && rrQ2.empty() && rrQ3.empty() && rrQ4.empty() && fcfsQ.empty() && ioQ.empty()) {
            time++;
        }
    }
}

/*  This method is a muti-feedback queue schedular for CPU processes
    @Param numQueue: user input for number of process queues to user
    @Param timeQuantum: user input for the time a process gets to run in a queue
    @Param ageing: user input for the ageing factor to send a process back up to the first queue
    @Param rrQ1: the first queue of processes
*/
void mfqs(int numQueues, int timeQuantum, int ageing, queue<Process> processQueue, bool printAll) {
    vector<int> tatList;
    vector<int> waitList;
    queue<Process> rrQ1;
    queue<Process> rrQ2;
    queue<Process> rrQ3;
    queue<Process> rrQ4;
    queue<Process> fcfsQ;
    queue<Process> ioQ;

    //Start running through processes
    int time = 0;
    while (1) {
        checkProcessQueue(processQueue, rrQ1, rrQ2, rrQ3, rrQ4, fcfsQ, ioQ, time);
        
        //empty rrQ1
        while (!rrQ1.empty()) {
            Process process = rrQ1.front();
            if (printAll) {
                cout << "Process " << process.processNum << " is in Q1" << endl;
            }
            for (int i = 0; i < timeQuantum; i++) {
                ioQueueIncrementer(ioQ, rrQ1);
                process.burstTime -= 1;
                if (process.burstTime == 1) {
                    ioQ.push(process);
                    if (printAll) {
                        cout << "Process " << process.processNum << " is in I/O" << endl;
                    }
                    time++;
                    break;
                }
                if (process.burstTime == 0) {
                    process.leave = time;
                    printProcessCharacteristics(process, tatList, waitList, printAll);
                    time++;
                    break;
                }
                if (i == (timeQuantum - 1)) {
                    if (numQueues > 2) {
                        rrQ2.push(process);
                    }
                    else {
                        fcfsQ.push(process);
                        if (printAll) {
                            cout << "Process " << process.processNum << " is in fcfsQ" << endl;
                        }
                    }
                }
                time++;
            }
            rrQ1.pop();
            checkFcfsFront(fcfsQ, rrQ1, ageing, time);
            checkProcessQueue(processQueue, rrQ1, rrQ2, rrQ3, rrQ4, fcfsQ, ioQ, time);
        }
        if (numQueues >= 3) {
            //empty rrQ2
            while (!rrQ2.empty()) {
                Process process = rrQ2.front();
                if (printAll) {
                    cout << "Process " << process.processNum << " is in Q2" << endl;
                }
                for (int i = 0; i < (timeQuantum * 2); i++) {
                    ioQueueIncrementer(ioQ, rrQ1);
                    process.burstTime -= 1;
                    if (process.burstTime == 1) {
                        ioQ.push(process);
                        if (printAll) {
                            cout << "Process " << process.processNum << " is in I/O" << endl;
                        }
                        time++;
                        break;
                    }
                    if (process.burstTime == 0) {
                        process.leave = time;
                        printProcessCharacteristics(process, tatList, waitList, printAll);
                        time++;
                        break;
                    }
                    if (i == ((timeQuantum * 2) - 1)) {
                        if (numQueues > 3) {
                            rrQ3.push(process);
                        }
                        else {
                            fcfsQ.push(process);
                        }
                    }
                    time++;
                }
                rrQ2.pop();
                checkFcfsFront(fcfsQ, rrQ1, ageing, time);
                checkProcessQueue(processQueue, rrQ1, rrQ2, rrQ3, rrQ4, fcfsQ, ioQ, time);
                if (!rrQ1.empty()) {
                    break;
                }

            }
            if (!rrQ1.empty()) {
                continue;
            }
        }
        if (numQueues >= 4) {
            //empty rrQ3
            while (!rrQ3.empty()) {
                Process process = rrQ3.front();
                if (printAll) {
                    cout << "Process " << process.processNum << " is in Q3" << endl;
                }
                for (int i = 0; i < (timeQuantum * 4); i++) {
                    ioQueueIncrementer(ioQ, rrQ1);
                    process.burstTime -= 1;
                    if (process.burstTime == 1) {
                        ioQ.push(process);
                        if (printAll) {
                            cout << "Process " << process.processNum << " is in I/O" << endl;
                        }
                        time++;
                        break;
                    }
                    if (process.burstTime == 0) {
                        process.leave = time;
                        printProcessCharacteristics(process, tatList, waitList, printAll);
                        time++;
                        break;
                    }
                    if (i == ((timeQuantum * 4) - 1)) {
                        if (numQueues > 4) {
                            rrQ4.push(process);
                        }
                        else {
                            fcfsQ.push(process);
                        }
                    }
                    time++;
                }
                rrQ3.pop();
                checkFcfsFront(fcfsQ, rrQ1, ageing, time);
                checkProcessQueue(processQueue, rrQ1, rrQ2, rrQ3, rrQ4, fcfsQ, ioQ, time);
                if (!rrQ1.empty() || !rrQ2.empty()) {
                    break;
                }
            }
            if (!rrQ1.empty() || !rrQ2.empty()) {
                continue;
            }
        }
        if (numQueues >= 5) {
            //empty rrQ4
            while (!rrQ4.empty()) {
                Process process = rrQ4.front();
                if (printAll) {
                    cout << "Process " << process.processNum << " is in Q4" << endl;
                }
                for (int i = 0; i < (timeQuantum * 8); i++) {
                    ioQueueIncrementer(ioQ, rrQ1);
                    process.burstTime -= 1;
                    if (process.burstTime == 1) {
                        ioQ.push(process);
                        if (printAll) {
                            cout << "Process " << process.processNum << " is in I/O" << endl;
                        }
                        time++;
                        break;
                    }
                    if (process.burstTime == 0) {
                        process.leave = time;
                        printProcessCharacteristics(process, tatList, waitList, printAll);
                        time++;
                        break;
                    }
                    if (i == ((timeQuantum * 8) - 1)) {
                        fcfsQ.push(process);
                    }
                    time++;
                }
                rrQ4.pop();
                checkFcfsFront(fcfsQ, rrQ1, ageing, time);
                checkProcessQueue(processQueue, rrQ1, rrQ2, rrQ3, rrQ4, fcfsQ, ioQ, time);
                if (!rrQ1.empty() || !rrQ2.empty() || !rrQ3.empty()) {
                    break;
                }

            }
            if (!rrQ1.empty() || !rrQ2.empty() || !rrQ3.empty()) {
                continue;
            }
        }
        //empty out FCFS queue
        while (!fcfsQ.empty()) {
            Process process = fcfsQ.front();
            if (printAll) {
                cout << "Process " << process.processNum << " is in FCFSQ" << endl;
            }
            if (process.enter == 0) {
                process.enter = time;
            }
            ioQueueIncrementer(ioQ, rrQ1);
            process.burstTime -= 1;
            if (process.burstTime == 1) {
                ioQ.push(process);
                if (printAll) {
                    cout << "Process " << process.processNum << " is in I/O" << endl;
                }
                time++;
                break;
            }
            if (process.burstTime == 0) {
                process.leave = time;
                printProcessCharacteristics(process, tatList, waitList, printAll);
                fcfsQ.pop();
            }
            if (numQueues >= 2) {
                if (!rrQ1.empty()) {
                    time++;
                    break;
                }
                if (numQueues >= 3) {
                    if (!rrQ2.empty()) {
                        time++;
                        break;
                    }
                    if (numQueues >= 4) {
                        if (!rrQ3.empty()) {
                            time++;
                            break;
                        }
                        if (numQueues >= 5) {
                            if (!rrQ4.empty()) {
                                time++;
                                break;
                            }
                        }
                    }
                }
            }
            time++;
        }
        //run out the IOQ if there are no more processes in other queues
        if (!ioQ.empty() && rrQ1.empty() && rrQ2.empty() && rrQ3.empty() && rrQ4.empty() && fcfsQ.empty() && processQueue.empty()) {
            time++;
            ioQueueIncrementer(ioQ, rrQ1);
        }
        checkProcessQueue(processQueue, rrQ1, rrQ2, rrQ3, rrQ4, fcfsQ, ioQ, time);
        //no more processes, end program
        if (fcfsQ.empty() && processQueue.empty() && ioQ.empty() && rrQ1.empty() && rrQ2.empty() && rrQ3.empty() && rrQ4.empty()) {
            break;
        }
    }
    cout << "Time: " << time << endl;
    printEndAvg(tatList, waitList);
}



int main() {
    vector<Process> pVector;
    queue<Process> processQueue;
    int numProcess = 0;
    //get process input type
    string processInput;
    string fileName;
    while (1) {
        cout << "Press 0 to use a file for processes or 1 to insert processes yourself: ";
        cin >> processInput;
        if (processInput == "0" || processInput == "1") {
            break;
        }
        else {
            cout << "Please enter the correct value for process input" << endl;
        }
    }
    //get sorting algorithm
    string getProcessMethod;
    while (1) {
        cout << "Press 0 for multiqueue feedback sort or 1 for real time sort scheduler: ";
        cin >> getProcessMethod;
        if (getProcessMethod == "0" || getProcessMethod == "1") {
            break;
        }
        else {
            cout << "Please enter the correct value for sorting algorithm" << endl;
        }
    }
    int userProcesses;
    //get file name if from a file
    if (processInput == "0") {
        while (1) {
            cout << "Please insert a file name: ";
            cin >> fileName;
            cout << "Processing file..." << endl;
            ifstream processFile(fileName);
            if (processFile.is_open()) {
                getProcesses(processFile, pVector);
                quickSort(pVector, 0, pVector.size() - 1);
                vector<Process>::iterator it = pVector.begin();
                while (it != pVector.end()) {
                    Process process = *it;
                    processQueue.push(process);
                    numProcess++;
                    it++;
                }
                processFile.close();
                cout << "File ready!" << endl;
                break;
            }
            else {
                cout << "Couldn't open the file you were looking for.  Please retry" << endl;
            }
        }
    }
    //Get user input processes
    else if (processInput == "1") {
        while (1) {
            cout << "How many processes are there going to be? ";
            cin >> userProcesses;
            if (int(userProcesses) <= 0) {
                cout << "please enter a number greater than 0" << endl;
            }
            else {
                Process process;
                for (int i = 0; i < userProcesses; i++) {
                    cout << "Process " << i + 1 << " entry:" << endl;
                    process.processNum = i + 1;
                    cout << "Please enter the burst time: ";
                    cin >> process.burstTime;
                    cout << "Please enter the arrival time: ";
                    cin >> process.arrivalTime;
                    cout << "Please enter the priority: ";
                    cin >> process.priority;
                    cout << "Please enter the deadline: ";
                    cin >> process.deadline;
                    cout << "Please enter the io time: ";
                    cin >> process.io;
                    pVector.push_back(process);
                }
                quickSort(pVector, 0, pVector.size() -1);
                vector<Process>::iterator it = pVector.begin();
                while (it != pVector.end()) {
                    Process process = *it;
                    processQueue.push(process);
                    numProcess++;
                    it++;
                }
                break;
            }
        }
    }
    //Get mfqs inputs
    if (getProcessMethod == "0") {
        int numQueues, timeQuantum, ageing, printAll;
        while (1) {
            cout << "Please enter the number of queues to use(2-5): ";
            cin >> numQueues;
            if (numQueues < 2 || numQueues > 5) {
                cout << "Please enter a number between 2 and 5" << endl;
            }
            else {
                break;
            }
        }
        while (1) {
            cout << "Please enter the timeQuantum for the queues: ";
            cin >> timeQuantum;
            if (timeQuantum < 1) {
                cout << "Please enter a number greater than 0" << endl;
            }
            else {
                break;
            }
        }
        while (1) {
            cout << "Please enter the age at which you want a process to queue up: ";
            cin >> ageing;
            if (ageing < 1) {
                cout << "Please enter a number greater than 0" << endl;
            }
            else {
                break;
            }
        }
        while (1) {
            cout << "Enter 1 to print all info or enter 0 to print end info only: ";
            cin >> printAll;
            if (printAll == 1) {
                mfqs(numQueues, timeQuantum, ageing, processQueue, true);
                break;
            }
            else if (printAll == 0) {
                mfqs(numQueues, timeQuantum, ageing, processQueue, false);
                break;
            }
            else {
                cout << "Please enter either 0 or 1" << endl;
            }
        }
    }
    else if (getProcessMethod == "1") {
        cout << "This method is for going to real time scheduling" << endl;
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
        }
    }

    cout << "Number of Processes: " << numProcess << endl;
    return 0;
}