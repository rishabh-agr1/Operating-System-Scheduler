#include <iostream>
#include <iomanip>
#include <climits>
using namespace std;

struct Process {
    int id;
    int arrivalTime;
    int burstTime;
    int remainingTime;
    int priority;
    int startTime;
    int completionTime;
    int waitingTime;
    int turnaroundTime;
    int responseTime;
};

void inputProcesses(Process processes[], int numProcesses, bool& hasPriority);
void calculateFirstComeFirstServed(Process processes[], int numProcesses);
void calculateShortestJobFirst(Process processes[], int numProcesses);
void calculateShortestJobRemainingFirst(Process processes[], int numProcesses);
void calculateRoundRobin(Process processes[], int numProcesses, int quantum);
void calculatePrioritySchedulingNonPreemptive(Process processes[], int numProcesses);
void calculatePrioritySchedulingPreemptive(Process processes[], int numProcesses);
void printResults(const char* algorithm, Process processes[], int numProcesses);
void resetProcesses(Process processes[], int numProcesses);
void suggestBestAlgorithm(float fcfsAvgTime, float sjfAvgTime, float sjrfAvgTime, float rrAvgTime, float priorityNPAvgTime, float priorityPAvgTime);

int main() {
    int numProcesses;
    cout << "Enter the number of processes: ";
    cin >> numProcesses;

    Process processes[numProcesses];
    bool hasPriority = false;  // Variable to check if priority is present
    inputProcesses(processes, numProcesses, hasPriority);

    float avgTurnAroundFCFS, avgTurnAroundSJF, avgTurnAroundSJRF, avgTurnAroundRR;
    float avgTurnAroundPriorityNP = 0.0f, avgTurnAroundPriorityP = 0.0f;

    resetProcesses(processes, numProcesses);
    calculateFirstComeFirstServed(processes, numProcesses);
    printResults("First-Come, First-Served", processes, numProcesses);
    
    resetProcesses(processes, numProcesses);
    calculateShortestJobFirst(processes, numProcesses);
    printResults("Shortest Job First", processes, numProcesses);
    
    resetProcesses(processes, numProcesses);
    calculateShortestJobRemainingFirst(processes, numProcesses);
    printResults("Shortest Job Remaining First", processes, numProcesses);
    
    int quantum = 2;
    resetProcesses(processes, numProcesses);
    calculateRoundRobin(processes, numProcesses, quantum);
    printResults("Round Robin (Quantum = 2)", processes, numProcesses);
    
    if (hasPriority) {  // Check if priority scheduling is to be executed
        resetProcesses(processes, numProcesses);
        calculatePrioritySchedulingNonPreemptive(processes, numProcesses);
        printResults("Priority Scheduling (Non-Preemptive)", processes, numProcesses);

        resetProcesses(processes, numProcesses);
        calculatePrioritySchedulingPreemptive(processes, numProcesses);
        printResults("Priority Scheduling (Preemptive)", processes, numProcesses);
    }

    suggestBestAlgorithm(avgTurnAroundFCFS, avgTurnAroundSJF, avgTurnAroundSJRF, avgTurnAroundRR, avgTurnAroundPriorityNP, avgTurnAroundPriorityP);

    return 0;
}

void inputProcesses(Process processes[], int numProcesses, bool& hasPriority) {
    char priorityResponse;
    cout << "Do processes have priority? (y/n): ";
    cin >> priorityResponse;
    
    hasPriority = (priorityResponse == 'y' || priorityResponse == 'Y');

    for (int i = 0; i < numProcesses; i++) {
        cout << "Enter Arrival Time and Burst Time for Process " << (i + 1) << ": ";
        processes[i].id = i + 1;
        cin >> processes[i].arrivalTime >> processes[i].burstTime;
        processes[i].remainingTime = processes[i].burstTime;
        
        if (hasPriority) {
            cout << "Enter Priority for Process " << (i + 1) << ": ";
            cin >> processes[i].priority;
        } else {
            processes[i].priority = 0; // Default priority if not provided
        }
    }
}

void resetProcesses(Process processes[], int numProcesses) {
    for (int i = 0; i < numProcesses; i++) {
        processes[i].remainingTime = processes[i].burstTime;
        processes[i].completionTime = 0;
        processes[i].waitingTime = 0;
        processes[i].turnaroundTime = 0;
        processes[i].responseTime = -1;
        processes[i].startTime = -1;
    }
}

void calculateFirstComeFirstServed(Process processes[], int numProcesses) {
    int currentTime = 0;
    for (int i = 0; i < numProcesses; i++) {
        if (currentTime < processes[i].arrivalTime) {
            currentTime = processes[i].arrivalTime;
        }
        processes[i].startTime = currentTime;
        processes[i].completionTime = currentTime + processes[i].burstTime;
        processes[i].turnaroundTime = processes[i].completionTime - processes[i].arrivalTime;
        processes[i].waitingTime = processes[i].turnaroundTime - processes[i].burstTime;
        processes[i].responseTime = processes[i].startTime - processes[i].arrivalTime;
        currentTime = processes[i].completionTime;
    }
}

void calculateShortestJobFirst(Process processes[], int numProcesses) {
    int currentTime = 0, completed = 0;
    bool visited[numProcesses] = {false};
    
    while (completed < numProcesses) {
        int idx = -1;
        int minBurstTime = INT_MAX;
        for (int i = 0; i < numProcesses; i++) {
            if (!visited[i] && processes[i].arrivalTime <= currentTime && processes[i].burstTime < minBurstTime) {
                minBurstTime = processes[i].burstTime;
                idx = i;
            }
        }

        if (idx != -1) {
            processes[idx].startTime = currentTime;
            processes[idx].completionTime = currentTime + processes[idx].burstTime;
            processes[idx].turnaroundTime = processes[idx].completionTime - processes[idx].arrivalTime;
            processes[idx].waitingTime = processes[idx].turnaroundTime - processes[idx].burstTime;
            processes[idx].responseTime = processes[idx].startTime - processes[idx].arrivalTime;
            currentTime = processes[idx].completionTime;
            visited[idx] = true;
            completed++;
        } else {
            currentTime++;
        }
    }
}

void calculateShortestJobRemainingFirst(Process processes[], int numProcesses) {
    int currentTime = 0, completed = 0;
    int minRemainingTime, idx = -1;

    while (completed < numProcesses) {
        minRemainingTime = INT_MAX;
        idx = -1;
        for (int i = 0; i < numProcesses; i++) {
            if (processes[i].arrivalTime <= currentTime && processes[i].remainingTime > 0 && processes[i].remainingTime < minRemainingTime) {
                minRemainingTime = processes[i].remainingTime;
                idx = i;
            }
        }

        if (idx != -1) {
            if (processes[idx].startTime == -1) {
                processes[idx].startTime = currentTime;
            }
            processes[idx].remainingTime--;
            currentTime++;

            if (processes[idx].remainingTime == 0) {
                processes[idx].completionTime = currentTime;
                processes[idx].turnaroundTime = processes[idx].completionTime - processes[idx].arrivalTime;
                processes[idx].waitingTime = processes[idx].turnaroundTime - processes[idx].burstTime;
                processes[idx].responseTime = processes[idx].startTime - processes[idx].arrivalTime;
                completed++;
            }
        } else {
            currentTime++;
        }
    }
}

void calculateRoundRobin(Process processes[], int numProcesses, int quantum) {
    int currentTime = 0, completed = 0;
    bool visited[numProcesses] = {false};
    
    while (completed < numProcesses) {
        for (int i = 0; i < numProcesses; i++) {
            if (processes[i].arrivalTime <= currentTime && processes[i].remainingTime > 0) {
                if (processes[i].startTime == -1) {
                    processes[i].startTime = currentTime;
                }

                if (processes[i].remainingTime > quantum) {
                    currentTime += quantum;
                    processes[i].remainingTime -= quantum;
                } else {
                    currentTime += processes[i].remainingTime;
                    processes[i].remainingTime = 0;
                    processes[i].completionTime = currentTime;
                    processes[i].turnaroundTime = processes[i].completionTime - processes[i].arrivalTime;
                    processes[i].waitingTime = processes[i].turnaroundTime - processes[i].burstTime;
                    processes[i].responseTime = processes[i].startTime - processes[i].arrivalTime;
                    completed++;
                }
            }
        }
    }
}

void calculatePrioritySchedulingNonPreemptive(Process processes[], int numProcesses) {
    int currentTime = 0, completed = 0;
    bool visited[numProcesses] = {false};

    while (completed < numProcesses) {
        int idx = -1;
        int highestPriority = INT_MAX;
        for (int i = 0; i < numProcesses; i++) {
            if (!visited[i] && processes[i].arrivalTime <= currentTime && processes[i].priority < highestPriority) {
                highestPriority = processes[i].priority;
                idx = i;
            }
        }

        if (idx != -1) {
            processes[idx].startTime = currentTime;
            processes[idx].completionTime = currentTime + processes[idx].burstTime;
            processes[idx].turnaroundTime = processes[idx].completionTime - processes[idx].arrivalTime;
            processes[idx].waitingTime = processes[idx].turnaroundTime - processes[idx].burstTime;
            processes[idx].responseTime = processes[idx].startTime - processes[idx].arrivalTime;
            currentTime = processes[idx].completionTime;
            visited[idx] = true;
            completed++;
        } else {
            currentTime++;
        }
    }
}

void calculatePrioritySchedulingPreemptive(Process processes[], int numProcesses) {
    int currentTime = 0, completed = 0;

    while (completed < numProcesses) {
        int idx = -1;
        int highestPriority = INT_MAX;
        for (int i = 0; i < numProcesses; i++) {
            if (processes[i].arrivalTime <= currentTime && processes[i].remainingTime > 0 && processes[i].priority < highestPriority) {
                highestPriority = processes[i].priority;
                idx = i;
            }
        }

        if (idx != -1) {
            if (processes[idx].startTime == -1) {
                processes[idx].startTime = currentTime;
            }
            processes[idx].remainingTime--;
            currentTime++;

            if (processes[idx].remainingTime == 0) {
                processes[idx].completionTime = currentTime;
                processes[idx].turnaroundTime = processes[idx].completionTime - processes[idx].arrivalTime;
                processes[idx].waitingTime = processes[idx].turnaroundTime - processes[idx].burstTime;
                processes[idx].responseTime = processes[idx].startTime - processes[idx].arrivalTime;
                completed++;
            }
        } else {
            currentTime++;
        }
    }
}

void printResults(const char* algorithm, Process processes[], int numProcesses) {
    float totalTurnAroundTime = 0, totalWaitingTime = 0, totalResponseTime = 0;

    cout << "\n" << algorithm << " Results:" << endl;
    cout << "-----------------------------------------------------" << endl;
    cout << "ID\tArrival\tBurst\tPriority\tCompletion\tWaiting\tTurnaround\tResponse" << endl;

    for (int i = 0; i < numProcesses; i++) {
        cout << processes[i].id << "\t" 
             << processes[i].arrivalTime << "\t" 
             << processes[i].burstTime << "\t" 
             << processes[i].priority << "\t\t"
             << processes[i].completionTime << "\t\t"
             << processes[i].waitingTime << "\t\t" 
             << processes[i].turnaroundTime << "\t\t"
             << processes[i].responseTime << endl;

        totalTurnAroundTime += processes[i].turnaroundTime;
        totalWaitingTime += processes[i].waitingTime;
        totalResponseTime += processes[i].responseTime;
    }

    float avgTurnAroundTime = totalTurnAroundTime / numProcesses;
    float avgWaitingTime = totalWaitingTime / numProcesses;
    float avgResponseTime = totalResponseTime / numProcesses;

    cout << fixed << setprecision(2);
    cout << "\nAverage Turnaround Time: " << avgTurnAroundTime << endl;
    cout << "Average Waiting Time: " << avgWaitingTime << endl;
    cout << "Average Response Time: " << avgResponseTime << endl;
}

void suggestBestAlgorithm(float fcfsAvgTime, float sjfAvgTime, float sjrfAvgTime, float rrAvgTime, float priorityNPAvgTime, float priorityPAvgTime) {
    float minTime = min(min(min(fcfsAvgTime, sjfAvgTime), min(sjrfAvgTime, rrAvgTime)), min(priorityNPAvgTime, priorityPAvgTime));

    cout << "\nBased on the average turnaround time, the best algorithm is: ";
    if (minTime == fcfsAvgTime) {
        cout << "First-Come, First-Served (FCFS)" << endl;
    } else if (minTime == sjfAvgTime) {
        cout << "Shortest Job First (SJF)" << endl;
    } else if (minTime == sjrfAvgTime) {
        cout << "Shortest Job Remaining First (SJRF)" << endl;
    } else if (minTime == rrAvgTime) {
        cout << "Round Robin (Quantum = 2)" << endl;
    } else if (minTime == priorityNPAvgTime) {
        cout << "Priority Scheduling (Non-Preemptive)" << endl;
    } else {
        cout << "Priority Scheduling (Preemptive)" << endl;
    }
}
