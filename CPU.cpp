#include <bits/stdc++.h>
using namespace std;

struct Process {
    int pid, arrival, burst, remaining, waiting, turnaround, priority;
};

// ---------- Helper: Print Results ----------
void printResults(const vector<Process>& p) {
    double total_wait = 0, total_tat = 0;

    cout << "\n\nPID\tArr\tBurst\tPrio\tWait\tTAT";
    cout << "\n-------------------------------------------";
    for (auto &x : p) {
        cout << "\nP" << x.pid << "\t" << x.arrival << "\t" << x.burst
             << "\t" << x.priority << "\t" << x.waiting << "\t" << x.turnaround;
        total_wait += x.waiting;
        total_tat += x.turnaround;
    }
    cout << "\n-------------------------------------------";
    cout << "\nAverage Waiting Time: " << fixed << setprecision(2) << (total_wait / p.size());
    cout << "\nAverage Turnaround Time: " << fixed << setprecision(2) << (total_tat / p.size());
    cout << "\n";
}

// ---------- FCFS ----------
void FCFS(vector<Process> p) {
    sort(p.begin(), p.end(), [](auto &a, auto &b){ return a.arrival < b.arrival; });

    int time = 0;
    for (auto &x : p) {
        time = max(time, x.arrival);
        x.waiting = time - x.arrival;
        time += x.burst;
        x.turnaround = x.waiting + x.burst;
    }

    cout << "\n===== FCFS Scheduling =====";
    printResults(p);
}

// ---------- SJF (Preemptive) ----------
void SJF(vector<Process> p) {
    int n = p.size();
    for (auto &x : p) x.remaining = x.burst;

    int completed = 0, time = 0;
    while (completed < n) {
        int idx = -1, mn = INT_MAX;
        for (int i = 0; i < n; ++i)
            if (p[i].arrival <= time && p[i].remaining > 0 && p[i].remaining < mn)
                mn = p[i].remaining, idx = i;

        if (idx == -1) { time++; continue; }

        p[idx].remaining--;
        time++;

        if (p[idx].remaining == 0) {
            completed++;
            p[idx].turnaround = time - p[idx].arrival;
            p[idx].waiting = p[idx].turnaround - p[idx].burst;
        }
    }

    cout << "\n===== SJF (Preemptive) Scheduling =====";
    printResults(p);
}

// ---------- Priority (Non-preemptive) ----------
void PriorityScheduling(vector<Process> p) {
    int n = p.size();
    vector<bool> done(n, false);
    int completed = 0, time = 0;

    while (completed < n) {
        int idx = -1, best = INT_MAX;
        for (int i = 0; i < n; ++i)
            if (!done[i] && p[i].arrival <= time && p[i].priority < best)
                best = p[i].priority, idx = i;

        if (idx == -1) { time++; continue; }

        done[idx] = true;
        completed++;
        p[idx].waiting = time - p[idx].arrival;
        time += p[idx].burst;
        p[idx].turnaround = p[idx].waiting + p[idx].burst;
    }

    cout << "\n===== Priority (Non-Preemptive) Scheduling =====";
    printResults(p);
}

// ---------- Round Robin ----------
void RoundRobin(vector<Process> p, int quantum) {
    int n = p.size();
    for (auto &x : p) x.remaining = x.burst;
    sort(p.begin(), p.end(), [](auto &a, auto &b){ return a.arrival < b.arrival; });

    queue<int> q;
    int time = 0, completed = 0;
    vector<bool> inQueue(n, false);

    q.push(0);
    inQueue[0] = true;

    while (completed < n) {
        if (q.empty()) {
            for (int i = 0; i < n; ++i)
                if (!inQueue[i] && p[i].arrival <= time) {
                    q.push(i);
                    inQueue[i] = true;
                    break;
                }
            time++;
            continue;
        }

        int i = q.front(); q.pop();
        inQueue[i] = false;

        int run = min(quantum, p[i].remaining);
        p[i].remaining -= run;
        time += run;

        for (int j = 0; j < n; ++j)
            if (!inQueue[j] && p[j].arrival <= time && p[j].remaining > 0)
                q.push(j), inQueue[j] = true;

        if (p[i].remaining > 0) {
            q.push(i);
            inQueue[i] = true;
        } else {
            completed++;
            p[i].turnaround = time - p[i].arrival;
            p[i].waiting = p[i].turnaround - p[i].burst;
        }
    }

    cout << "\n===== Round Robin (q=" << quantum << ") Scheduling =====";
    printResults(p);
}

// ---------- MAIN ----------
int main() {
    int n;
    cout << "Enter number of processes: ";
    cin >> n;

    vector<Process> p(n);
    for (int i = 0; i < n; ++i) {
        p[i].pid = i + 1;
        cout << "Enter arrival, burst, and priority for P" << i + 1 << ": ";
        cin >> p[i].arrival >> p[i].burst >> p[i].priority;
    }

    FCFS(p);
    SJF(p);
    PriorityScheduling(p);
    RoundRobin(p, 2); // You can change quantum here

    return 0;
}
