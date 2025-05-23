#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <string>
#include <sstream>
#include <chrono>
#include <mach/mach.h>

using namespace std;

const int N = 3;
vector<vector<int>> goal_state;

vector<vector<int>> generateGoalState() {
    vector<vector<int>> goal(N, vector<int>(N));
    int val = 1;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            goal[i][j] = (val == N * N) ? 0 : val++;
    return goal;
}

size_t getMemoryUsage() {
    task_basic_info info;
    mach_msg_type_number_t count = TASK_BASIC_INFO_COUNT;
    task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&info, &count);
    return info.resident_size; // in bytes
}

int countMisplacedTiles(const vector<vector<int>>& state) {
    int misplaced = 0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (state[i][j] != 0 && state[i][j] != goal_state[i][j])
                ++misplaced;
    return misplaced;
}

int manhattanDistance(const vector<vector<int>>& state) {
    int distance = 0;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            int val = state[i][j];
            if (val != 0) {
                int goal_i = (val - 1) / N;
                int goal_j = (val - 1) % N;
                distance += abs(i - goal_i) + abs(j - goal_j);
            }
        }
    }
    return distance;
}

struct PuzzleState {
    vector<vector<int>> state;
    int g, h, f;
    PuzzleState* parent;

    PuzzleState(vector<vector<int>> s, int g_val, int h_val = 0, PuzzleState* p = nullptr)
        : state(s), g(g_val), h(h_val), f(g_val + h_val), parent(p) {}
};

struct CompareUsingF {
    bool operator()(PuzzleState* a, PuzzleState* b) {
        return a->f > b->f; // Min-heap based on f = g + h
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
            children.push_back(new PuzzleState(new_state, node->g + 1, 0, node));
        }
    }
    return children;
}

void printState(const vector<vector<int>>& state) {
    for (const auto& row : state) {
        cout << "[";
        for (int val : row)
            cout << val << " ";
        cout << "]\n";
    }
}

void uniformCostSearch(const vector<vector<int>>& initial_state) {
    priority_queue<PuzzleState*, vector<PuzzleState*>, CompareUsingF> search_queue;
    unordered_set<string> explored;
    search_queue.push(new PuzzleState(initial_state, 0, 0));
    string sig = stateToString(initial_state);
    explored.insert(sig);
    int nodes_expanded = 0;
    int max_queue_size = 1;

    while (!search_queue.empty()) {
        max_queue_size = max(max_queue_size, (int)search_queue.size());
        PuzzleState* node = search_queue.top();
        search_queue.pop();

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
            string sig = stateToString(child->state);
            if (explored.count(sig)) {
                delete child;
                continue;
            }
            child->h = 0;
            child->f = child->g + child->h;
            search_queue.push(child);
            explored.insert(stateToString(child->state));
        }
    }
    cout << "Failure\n";
}

void misplacedTileSearch(const vector<vector<int>>& initial_state) {
    priority_queue<PuzzleState*, vector<PuzzleState*>, CompareUsingF> search_queue;
    unordered_set<string> explored;

    int h = countMisplacedTiles(initial_state);
    search_queue.push(new PuzzleState(initial_state, 0, h));
    string sig = stateToString(initial_state);
    explored.insert(sig);

    int nodes_expanded = 0;
    int max_queue_size = 1;

    while (!search_queue.empty()) {
        max_queue_size = max(max_queue_size, (int)search_queue.size());
        PuzzleState* node = search_queue.top();
        search_queue.pop();

        cout << "The best state to expand with a g(n) = " << node->g << " and h(n) = " << node->h << " is...\n";
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
            string sig = stateToString(child->state);
            if (explored.count(sig)) {
                delete child;
                continue;
            }
            child->h = countMisplacedTiles(child->state);
            child->f = child->g + child->h;
            search_queue.push(child);
            explored.insert(sig);
        }
    }
    cout << "Failure\n";
}

void manhattanDistanceSearch(const vector<vector<int>>& initial_state) {
    priority_queue<PuzzleState*, vector<PuzzleState*>, CompareUsingF> search_queue;
    unordered_set<string> explored;

    int h = manhattanDistance(initial_state);
    search_queue.push(new PuzzleState(initial_state, 0, h));

    string sig = stateToString(initial_state);
    explored.insert(sig);

    int nodes_expanded = 0;
    int max_queue_size = 1;

    while (!search_queue.empty()) {
        max_queue_size = max(max_queue_size, (int)search_queue.size());
        PuzzleState* node = search_queue.top();
        search_queue.pop();

        cout << "The best state to expand with a g(n) = " << node->g << " and h(n) = " << node->h << " is...\n";
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
            string sig = stateToString(child->state);
            if (explored.count(sig)) {
                delete child;
                continue;
            }
            child->h = manhattanDistance(child->state);
            child->f = child->g + child->h;
            search_queue.push(child);
            explored.insert(sig);
        }
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

    using namespace chrono;
    auto start = high_resolution_clock::now();

    size_t before = getMemoryUsage();
    if (algo == 1)
        uniformCostSearch(initial_state);
    else if (algo == 2)
        misplacedTileSearch(initial_state);
    else if (algo == 3)
        manhattanDistanceSearch(initial_state);
    else {
        cout << "Invalid selection.\n";
        return 0;
    }
    size_t after = getMemoryUsage();

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    cout << "Time taken: " << duration.count() << " milliseconds\n";
    std::cout << "Memory used: " << (after - before) << " bytes\n";
}