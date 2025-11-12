#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>
#include <climits>
using namespace std;

struct Process{
    int pid,arrival,burst,priority,remaining,turnaround,waiting;
};

static bool cmp(Process &a, Process &b){return a.arrival<b.arrival;}

void FCFS(vector<Process> &p){
    int time=0;
    sort(p.begin(),p.end(),cmp);
    for(int i=0;i<p.size();i++){
        if(time < p[i].arrival) time = p[i].arrival;
        time+=p[i].burst;
        p[i].turnaround=time - p[i].arrival;
        p[i].waiting=p[i].turnaround - p[i].burst;
    }
}

void SJF(vector<Process> &p){
    int n = p.size();
    int time = 0;
    int completed=0;
    for(auto &x:p) x.remaining=x.burst;
    while(completed<n){
        int shortest=INT_MAX,idx=-1;
        for(int i=0;i<n;i++){
            if(p[i].arrival<=time&&p[i].remaining>0&&p[i].remaining<shortest){
                shortest=p[i].remaining;
                idx=i;
            }
        }
        if(idx==-1){time++;continue;}
        p[idx].remaining--;
        time++;
        if(p[idx].remaining==0){
            completed++;
            p[idx].turnaround=time-p[idx].arrival;
            p[idx].waiting=p[idx].turnaround-p[idx].burst;
        }
    }
}

void priority(vector<Process> &p){
    int n=p.size();
    int time=0;
    vector<bool> done(n,false);
    int completed=0;
    while(completed<n){
        int idx=-1,best=INT_MAX;
        for(int i=0;i<n;i++){
            if(!done[i]&&p[i].arrival<=time&&p[i].priority<best){
                idx=i;
                best=p[i].priority;
            }
        }
        if(idx==-1){time++;continue;}
        done[idx]=true;
        completed++;
        p[idx].waiting = time - p[idx].arrival;
        p[idx].turnaround = p[idx].waiting + p[idx].burst;
        time+=p[idx].burst;
    }
}

void round_robin(vector<Process> &p,int tq){
    int n=p.size(),completed=0,time=0,time_quantum=tq;
    sort(p.begin(),p.end(),cmp);
    for(auto &x:p) x.remaining=x.burst;
    queue<int> q;
    vector<bool> inQueue(n,false);
    q.push(0);
    inQueue[0]=true;
    while(completed<n){
        if(q.empty()){
            for(int i=0;i<n;i++){
                if(!inQueue[i]&&p[i].arrival<=time&&p[i].remaining>0){
                    q.push(i);
                    inQueue[i]=true;
                    break;
                }
            }
            if(q.empty()){time++;continue;}
        }
        int idx=q.front();q.pop();inQueue[idx]=false;
        int runtime=min(time_quantum,p[idx].remaining);
        p[idx].remaining-=runtime;
        time+=runtime;
        
        // Add newly arrived processes to queue
        for(int i=0;i<n;i++){
            if(!inQueue[i]&&p[i].arrival<=time&&p[i].remaining>0&&i!=idx){
                q.push(i);
                inQueue[i]=true;
            }
        }
        
        if(p[idx].remaining==0){
            completed++;
            p[idx].turnaround=time-p[idx].arrival;
            p[idx].waiting=p[idx].turnaround-p[idx].burst;
        }
        else{
            q.push(idx);
            inQueue[idx]=true;
        }
    }
}

void printResults(vector<Process> &p){
    cout<<"\n----- RESULTS -----\n";
    cout<<"PID\tArrival\tBurst\tPriority\tTurnaround\tWaiting\n";
    float totalTAT=0, totalWT=0;
    for(auto &proc:p){
        cout<<proc.pid<<"\t"<<proc.arrival<<"\t"<<proc.burst<<"\t"<<proc.priority<<"\t\t"<<proc.turnaround<<"\t\t"<<proc.waiting<<"\n";
        totalTAT+=proc.turnaround;
        totalWT+=proc.waiting;
    }
    cout<<"\nAverage Turnaround Time: "<<totalTAT/p.size()<<"\n";
    cout<<"Average Waiting Time: "<<totalWT/p.size()<<"\n";
}

int main(){
    int n;
    cout<<"Enter number of processes: ";
    cin>>n;
    
    vector<Process> processes(n);
    
    cout<<"\nEnter details for each process:\n";
    for(int i=0;i<n;i++){
        processes[i].pid = i+1;
        cout<<"Process "<<i+1<<":\n";
        cout<<"  Arrival Time: ";
        cin>>processes[i].arrival;
        cout<<"  Burst Time: ";
        cin>>processes[i].burst;
        cout<<"  Priority (lower value = higher priority): ";
        cin>>processes[i].priority;
    }
    
    while(true){
        cout<<"\n===== CPU SCHEDULING ALGORITHMS =====\n";
        cout<<"1. First Come First Serve (FCFS)\n";
        cout<<"2. Shortest Job First (SJF) - Preemptive\n";
        cout<<"3. Priority Scheduling - Non-Preemptive\n";
        cout<<"4. Round Robin (RR)\n";
        cout<<"5. Exit\n";
        cout<<"Enter your choice: ";
        
        int choice;
        cin>>choice;
        
        vector<Process> p = processes; // Create a copy
        
        switch(choice){
            case 1:
                cout<<"\n----- FCFS Scheduling -----\n";
                FCFS(p);
                printResults(p);
                break;
            case 2:
                cout<<"\n----- SJF (Preemptive) Scheduling -----\n";
                SJF(p);
                printResults(p);
                break;
            case 3:
                cout<<"\n----- Priority Scheduling -----\n";
                priority(p);
                printResults(p);
                break;
            case 4:{
                int tq;
                cout<<"Enter Time Quantum: ";
                cin>>tq;
                cout<<"\n----- Round Robin Scheduling -----\n";
                round_robin(p, tq);
                printResults(p);
                break;
            }
            case 5:
                cout<<"Exiting...\n";
                return 0;
            default:
                cout<<"Invalid choice! Please try again.\n";
        }
    }
    
    return 0;
}