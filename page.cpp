#include <iostream>
    #include <vector>
    #include <algorithm>
    #include <queue>
    using namespace std;

    // ---------- FIFO ----------
    void fifo(vector<int> pages, int n) {
        vector<int> frames;
        queue<int> q;
        int faults = 0;

        cout << "\nFIFO Page Replacement:\n";
        for (int page : pages) {
            if (find(frames.begin(), frames.end(), page) != frames.end()) {
                cout << "Page " << page << " -> HIT  | Frames: ";
            } else {
                faults++;
                if (frames.size() == n) {
                    q.pop();
                    frames.erase(frames.begin());
                }
                frames.push_back(page);
                q.push(page);
                cout << "Page " << page << " -> FAULT  | Frames: ";
            }
            for (int f : frames) cout << f << " ";
            cout << endl;
        }
        cout << "Total Page Faults = " << faults << endl;
    }

    // ---------- LRU ----------
    void lru(vector<int> pages, int n) {
        vector<int> frames;
        int faults = 0;

        cout << "\nLRU Page Replacement:\n";
        for (int i = 0; i < pages.size(); i++) {
            int page = pages[i];
            auto it = find(frames.begin(), frames.end(), page);

            if (it != frames.end()) {
                cout << "Page " << page << " -> HIT  | Frames: ";
                frames.erase(it);
                frames.push_back(page);
            } else {
                faults++;
                if (frames.size() == n) {
                    int toRemove = frames.front();
                    frames.erase(frames.begin());
                }
                frames.push_back(page);
                cout << "Page " << page << " -> FAULT  | Frames: ";
            }
            for (int f : frames) cout << f << " ";
            cout << endl;
        }
        cout << "Total Page Faults = " << faults << endl;
    }

    // ---------- OPTIMAL ----------
    void optimal(vector<int> pages, int n) {
        vector<int> frames;
        int faults = 0;

        cout << "\nOPTIMAL Page Replacement:\n";
        for (int i = 0; i < pages.size(); i++) {
            int page = pages[i];
            auto it = find(frames.begin(), frames.end(), page);

            if (it != frames.end()) {
                cout << "Page " << page << " -> HIT  | Frames: ";
            } else {
                faults++;
                if (frames.size() == n) {
                    int farthest = -1, index = -1;
                    for (int j = 0; j < frames.size(); j++) {
                        int k;
                        for (k = i + 1; k < pages.size(); k++)
                            if (pages[k] == frames[j])
                                break;
                        if (k > farthest) { 
                            farthest = k; 
                            index = j; 
                        }
                    }
                    if (index != -1 && index < frames.size()) {
                        frames[index] = page;
                    }
                } else frames.push_back(page);

                cout << "Page " << page << " -> FAULT  | Frames: ";
            }
            for (int f : frames) cout << f << " ";
            cout << endl;
        }
        cout << "Total Page Faults = " << faults << endl;
    }

    // ---------- MAIN ----------
    int main() {
        vector<int> pages = {7, 0, 1, 2, 0, 3, 0, 4};
        int n = 3;

        cout << "Page Reference String: ";
        for (int p : pages) cout << p << " ";
        cout << "\nNo. of Frames: " << n << endl;

        fifo(pages, n);
        lru(pages, n);
        optimal(pages, n);

        return 0;
    }