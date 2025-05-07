#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <string>
#include <sstream>
using namespace std;

const int N = 4;
vector<vector<int>> goal_state;

vector<vector<int>> generateGoalState() {
    vector<vector<int>> goal(N, vector<int>(N));
    int val = 1;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            goal[i][j] = (val == N * N) ? 0 : val++;
    return goal;
}

int countMisplacedTiles(const vector<vector<int>>& state) {
    int misplaced = 0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (state[i][j] != 0 && state[i][j] != goal_state[i][j])
                ++misplaced;
    return misplaced;
}

struct PuzzleState {
    vector<vector<int>> state;
    int g, h, f;
    PuzzleState* parent;

    PuzzleState(vector<vector<int>> s, int g_val, int h_val = 0, PuzzleState* p = nullptr)
        : state(s), g(g_val), h(h_val), f(g_val + h_val), parent(p) {}
};

struct CompareAStar {
    bool operator()(PuzzleState* a, PuzzleState* b) {
        return a->f > b->f;
    }
};

bool isGoal(const vector<vector<int>>& state) {
    return state == goal_state;
}

string stateToString(const vector<vector<int>>& state) {
    stringstream ss;
    for (const auto& row : state)
        for (int val : row)
            ss << val;
    return ss.str();
}

vector<PuzzleState*> expand(PuzzleState* node) {
    vector<PuzzleState*> children;
    int dx[] = {-1, 1, 0, 0}, dy[] = {0, 0, -1, 1};
    int x, y;
    vector<vector<int>> s = node->state;

    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (s[i][j] == 0) { x = i; y = j; }

    for (int d = 0; d < 4; ++d) {
        int nx = x + dx[d], ny = y + dy[d];
        if (nx >= 0 && nx < N && ny >= 0 && ny < N) {
            vector<vector<int>> new_state = s;
            swap(new_state[x][y], new_state[nx][ny]);
            children.push_back(new PuzzleState(new_state, node->g + 1, node));
        }
    }
    return children;
}

struct Compare {
    bool operator()(PuzzleState* a, PuzzleState* b) {
        return a->g > b->g;
    }
};

void printState(const vector<vector<int>>& state) {
    for (const auto& row : state) {
        cout << "[";
        for (int val : row)
            cout << val << " ";
        cout << "]\n";
    }
}

void uniformCostSearch(const vector<vector<int>>& initial_state) {
    priority_queue<PuzzleState*, vector<PuzzleState*>, Compare> frontier;
    unordered_set<string> explored;
    frontier.push(new PuzzleState(initial_state, 0));

    int nodes_expanded = 0;
    int max_queue_size = 1;

    while (!frontier.empty()) {
        PuzzleState* node = frontier.top();
        frontier.pop();

        string sig = stateToString(node->state);
        if (explored.count(sig)) continue;
        explored.insert(sig);

        cout << "The best state to expand with a g(n) = " << node->g << " and h(n) = 0 is...\n";
        printState(node->state);
        nodes_expanded++;

        if (isGoal(node->state)) {
            cout << "Goal state!\n";
            cout << "Solution depth was " << node->g << endl;
            cout << "Number of nodes expanded: " << nodes_expanded << endl;
            cout << "Max queue size: " << max_queue_size << endl;
            return;
        }

        auto children = expand(node);
        for (PuzzleState* child : children) {
            frontier.push(child);
        }
        max_queue_size = max(max_queue_size, (int)frontier.size());
    }
    cout << "Failure\n";
}

vector<vector<int>> inputPuzzle(const string& label) {
    vector<vector<int>> puzzle(N, vector<int>(N));
    cout << "Enter the " << label << " puzzle row by row (use 0 for blank):\n";
    for (int i = 0; i < N; ++i) {
        cout << "Row " << i + 1 << ": ";
        for (int j = 0; j < N; ++j) {
            cin >> puzzle[i][j];
        }
    }
    return puzzle;
}

int main() {
    cout << "Welcome to my " << N * N - 1 << "-Puzzle Solver.\n";
    cout << "Type '1' for default goal state or '2' to enter your own goal state.\n";
    int goalChoice;
    cin >> goalChoice;

    if (goalChoice == 1) {
        goal_state = generateGoalState();
    } else {
        goal_state = inputPuzzle("goal");
    }

    vector<vector<int>> initial_state = inputPuzzle("initial");

    cout << "Select algorithm. (1) for Uniform Cost Search, (2) for Misplaced Tile Heuristic, or (3) the Manhattan Distance Heuristic.\n";
    int algo;
    cin >> algo;

    if (algo == 1)
        uniformCostSearch(initial_state);
    else
        cout << "Selected heuristic not implemented yet.\n";
}