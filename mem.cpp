#include <iostream>
#include <climits>
using namespace std;

// ===================== FIRST FIT =====================
void firstFit(int blockSize[], int nb, int processSize[], int np) {
    int allocation[10]; // Stores which block is assigned to each process

    // Initially, no process is allocated (-1 means not allocated)
    for (int i = 0; i < np; i++)
        allocation[i] = -1;

    // Copy original block sizes (so other methods can use the same values)
    int temp[10];
    for (int i = 0; i < nb; i++)
        temp[i] = blockSize[i];

    cout << "\n--- FIRST FIT ---\n";

    // For each process
    for (int i = 0; i < np; i++) {
        // Find the first block that can fit the process
        for (int j = 0; j < nb; j++) {
            if (temp[j] >= processSize[i]) {
                allocation[i] = j;       // assign this block
                temp[j] -= processSize[i]; // reduce remaining block size
                break; // move to next process
            }
        }
    }

    // Print result
    for (int i = 0; i < np; i++) {
        if (allocation[i] != -1)
            cout << "Process " << i + 1 << " -> Block " << allocation[i] + 1 << endl;
        else
            cout << "Process " << i + 1 << " -> Not Allocated\n";
    }
}

// ===================== BEST FIT =====================
void bestFit(int blockSize[], int nb, int processSize[], int np) {
    int allocation[10];
    for (int i = 0; i < np; i++)
        allocation[i] = -1;

    int temp[10];
    for (int i = 0; i < nb; i++)
        temp[i] = blockSize[i];

    cout << "\n--- BEST FIT ---\n";

    // For each process
    for (int i = 0; i < np; i++) {
        int bestIdx = -1; // best block position
        // Check all blocks to find the smallest one that fits
        for (int j = 0; j < nb; j++) {
            if (temp[j] >= processSize[i]) {
                if (bestIdx == -1 || temp[j] < temp[bestIdx])
                    bestIdx = j;
            }
        }
        // Allocate if a suitable block is found
        if (bestIdx != -1) {
            allocation[i] = bestIdx;
            temp[bestIdx] -= processSize[i];
        }
    }

    // Print result
    for (int i = 0; i < np; i++) {
        if (allocation[i] != -1)
            cout << "Process " << i + 1 << " -> Block " << allocation[i] + 1 << endl;
        else
            cout << "Process " << i + 1 << " -> Not Allocated\n";
    }
}

// ===================== WORST FIT =====================
void worstFit(int blockSize[], int nb, int processSize[], int np) {
    int allocation[10];
    for (int i = 0; i < np; i++)
        allocation[i] = -1;

    int temp[10];
    for (int i = 0; i < nb; i++)
        temp[i] = blockSize[i];

    cout << "\n--- WORST FIT ---\n";

    // For each process
    for (int i = 0; i < np; i++) {
        int worstIdx = -1; // worst block position
        // Find the block with the largest size that fits the process
        for (int j = 0; j < nb; j++) {
            if (temp[j] >= processSize[i]) {
                if (worstIdx == -1 || temp[j] > temp[worstIdx])
                    worstIdx = j;
            }
        }
        // Allocate if found
        if (worstIdx != -1) {
            allocation[i] = worstIdx;
            temp[worstIdx] -= processSize[i];
        }
    }

    // Print result
    for (int i = 0; i < np; i++) {
        if (allocation[i] != -1)
            cout << "Process " << i + 1 << " -> Block " << allocation[i] + 1 << endl;
        else
            cout << "Process " << i + 1 << " -> Not Allocated\n";
    }
}

// ===================== NEXT FIT =====================
void nextFit(int blockSize[], int nb, int processSize[], int np) {
    int allocation[10];
    for (int i = 0; i < np; i++)
        allocation[i] = -1;

    int temp[10];
    for (int i = 0; i < nb; i++)
        temp[i] = blockSize[i];

    cout << "\n--- NEXT FIT ---\n";
    int pos = 0; // starting block index

    // For each process
    for (int i = 0; i < np; i++) {
        bool allocated = false;

        // Search blocks starting from last allocated position
        for (int j = pos; j < nb; j++) {
            if (temp[j] >= processSize[i]) {
                allocation[i] = j;
                temp[j] -= processSize[i];
                pos = j; // next search starts from here
                allocated = true;
                break;
            }
        }

        // If not found till the end, start again from the first block
        if (!allocated)
            pos = 0;
    }

    // Print result
    for (int i = 0; i < np; i++) {
        if (allocation[i] != -1)
            cout << "Process " << i + 1 << " -> Block " << allocation[i] + 1 << endl;
        else
            cout << "Process " << i + 1 << " -> Not Allocated\n";
    }
}

// ===================== MAIN FUNCTION =====================
int main() {
    int nb, np; // nb = number of blocks, np = number of processes
    int blockSize[10], processSize[10];

    // Input block information
    cout << "Enter number of memory blocks: ";
    cin >> nb;
    cout << "Enter size of each block: ";
    for (int i = 0; i < nb; i++)
        cin >> blockSize[i];

    // Input process information
    cout << "\nEnter number of processes: ";
    cin >> np;
    cout << "Enter memory required by each process: ";
    for (int i = 0; i < np; i++)
        cin >> processSize[i];

    // Call all 4 allocation methods
    firstFit(blockSize, nb, processSize, np);
    bestFit(blockSize, nb, processSize, np);
    worstFit(blockSize, nb, processSize, np);
    nextFit(blockSize, nb, processSize, np);

    return 0;
}