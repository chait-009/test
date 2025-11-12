#include <iostream>
#include <vector>
using namespace std;

class Node {
    int nodeId;
    bool active;
    bool coordinator;
public:
    Node(int id) {
        nodeId = id;
        active = true;
        coordinator = false;
    }
    int getId() const { return nodeId; }
    bool isActive() const { return active; }
    bool isCoordinator() const { return coordinator; }
    void setActive(bool act) { active = act; }
    void setCoordinator(bool coord) { coordinator = coord; }
};

// Bully Algorithm
void bullyElection(vector<Node>& nodes, int initiatorId) {
    cout << "Node " << initiatorId << " initiates election (Bully Algorithm).\n";
    int maxId = -1;
    for (const auto& n : nodes){
        if (n.isActive() && n.getId() > maxId){
            maxId = n.getId();
        }
    }
    for (auto& n : nodes) {
        if (n.isActive())
            n.setCoordinator(false);
    }
            

    nodes[maxId].setCoordinator(true);
    cout << "Node " << maxId << " becomes the coordinator.\n";
}

// Ring Algorithm
void ringElection(vector<Node>& nodes, int initiatorId) {
    cout << "Node " << initiatorId << " initiates election (Ring Algorithm).\n";
    int n = nodes.size();
    int curr = initiatorId, msg = initiatorId;
    vector<bool> visited(n, false);

    do {
        curr = (curr + 1) % n;
        if (nodes[curr].isActive()) {
            cout << "Election message passes from Node " << msg << " to Node " << curr << endl;
            if (curr > msg) msg = curr;
        }
    } while (curr != initiatorId);

    for (auto& node : nodes) {
        node.setCoordinator(false);
    }
    nodes[msg].setCoordinator(true);

    cout << "Node " << msg << " becomes the coordinator.\n";
    curr = msg;
    int announce = msg;
    do {
        curr = (curr + 1) % n;
        if (nodes[curr].isActive()) {
            cout << "Coordinator message (Coordinator: " << announce << ") passes to Node " << curr << endl;
        }
    } while (curr != announce);
}

void printNodes(const vector<Node>& nodes) {
    cout << "Nodes: ";
    for (const auto& n : nodes) {
        cout << n.getId() << (n.isActive() ? (n.isCoordinator() ? "[C]" : "") : "[X]") << " ";
    }
    cout << "\n";
}

int main() {
    int n, choice, failedId, initiatorId;
    cout << "Enter number of nodes: ";
    cin >> n;
    vector<Node> nodes;
    for (int i = 0; i < n; ++i)
        nodes.push_back(Node(i));

        printNodes(nodes);

    do {
        cout << "\n--- Menu ---\n";
        cout << "1. Fail a Node\n";
        cout << "2. Recover a Node\n";
        cout << "3. Bully Algorithm Election\n";
        cout << "4. Ring Algorithm Election\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Enter node id to fail: ";
                cin >> failedId;
                if (failedId >= 0 && failedId < n)
                    nodes[failedId].setActive(false);
                else
                    cout << "Invalid ID\n";

                printNodes(nodes);
                break;
            case 2:
                cout << "Enter node id to recover: ";
                cin >> failedId;
                if (failedId >= 0 && failedId < n)
                    nodes[failedId].setActive(true);
                else
                    cout << "Invalid ID\n";
                    
                printNodes(nodes);
                break;
            case 3:
                cout << "Enter initiator node id: ";
                cin >> initiatorId;
                if (initiatorId >= 0 && initiatorId < n && nodes[initiatorId].isActive())
                    bullyElection(nodes, initiatorId);
                else
                    cout << "Invalid/Inactive initiator\n";

                printNodes(nodes);
                break;
            case 4:
                cout << "Enter initiator node id: ";
                cin >> initiatorId;
                if (initiatorId >= 0 && initiatorId < n && nodes[initiatorId].isActive())
                    ringElection(nodes, initiatorId);
                else
                    cout << "Invalid/Inactive initiator\n";
                    
                printNodes(nodes);
                break;
            case 0:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice\n";
        }
    } while (choice != 0);

    return 0;
}