#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <iomanip>
using namespace std;

// Hàm đọc đồ thị và điểm bắt đầu/kết thúc từ tệp
void readGraphAndEndpoints(const string& filename, unordered_map<string, vector<string>>& graph, string& start, string& end) {
    ifstream inputFile(filename);
    if (!inputFile) {
        cerr << "Cann't open file: " << filename << endl;
        exit(1);
    }

    string line;
    while (getline(inputFile, line)) {
        stringstream ss(line);
        if (line.rfind("Start:", 0) == 0) {
            start = line.substr(6); // Lấy điểm bắt đầu
        }
        else if (line.rfind("End:", 0) == 0) {
            end = line.substr(4); // Lấy điểm kết thúc
        }
        else {
            string node, neighbor;
            ss >> node;
            while (ss >> neighbor) {
                graph[node].push_back(neighbor);
            }
        }
    }
    inputFile.close();
}

// Hàm để căn chỉnh bảng động dựa trên nội dung
void calculateColumnWidths(const vector<string>& table, vector<size_t>& columnWidths) {
    for (const auto& row : table) {
        istringstream iss(row);
        string word;
        size_t colIndex = 0;

        while (iss >> word) {
            if (colIndex >= columnWidths.size()) {
                columnWidths.push_back(0);
            }
            columnWidths[colIndex] = max(columnWidths[colIndex], word.size());
            colIndex++;
        }
    }
}

// Định dạng một dòng trong bảng
string formatRow(const string& node, const vector<string>& nextStates, const unordered_set<string>& visited, const vector<string>& toVisit) {
    stringstream ss;
    vector<size_t> columnWidths = { 4, 19, 27, 25 }; // Thêm cột cho danh sách cần xét

    // Cột Node
    ss << left << setw(columnWidths[0]) << node << " | ";

    // Cột Next States
    string nextStatesStr;
    for (const auto& state : nextStates) {
        nextStatesStr += state + " ";
    }
    ss << left << setw(columnWidths[1]) << nextStatesStr << "| ";

    // Cột Visited Nodes
    string visitedStr;
    for (const auto& visit : visited) {
        visitedStr += visit + " ";
    }
    ss << left << setw(columnWidths[2]) << visitedStr << "| ";

    // Cột To Visit (Danh sách các node cần xét)
    string toVisitStr;
    for (const auto& visit : toVisit) {
        toVisitStr += visit + " ";
    }
    ss << left << setw(columnWidths[3]) << toVisitStr;

    return ss.str();
}

// Hàm BFS để tìm đường đi ngắn nhất
vector<string> BFS(unordered_map<string, vector<string>>& graph, const string& start, const string& end, vector<string>& steps, vector<string>& table) {
    queue<vector<string>> q; // Hàng đợi lưu đường đi
    unordered_set<string> visited; // Tập các điểm đã thăm
    queue<string> toVisitQueue; // Hàng đợi cho danh sách cần xét
    unordered_set<string> lastNodes; // Tập hợp để lưu các lastNode đã xét

    q.push({ start });
    visited.insert(start);
    toVisitQueue.push(start); // Thêm node bắt đầu vào danh sách cần xét

    while (!q.empty()) {
        vector<string> path = q.front();
        q.pop();

        string lastNode = path.back(); // Đỉnh đang xét
        lastNodes.insert(lastNode); // Thêm lastNode vào tập hợp

        // Tạo trạng thái cho bảng
        vector<string> nextStates;
        for (const string& next : graph[lastNode]) {
            if (visited.find(next) == visited.end()) {
                nextStates.push_back(next);
                visited.insert(next);
                toVisitQueue.push(next); // Thêm vào hàng đợi cần xét

                vector<string> newPath = path;
                newPath.push_back(next);
                q.push(newPath);
            }
        }
        // Cập nhật danh sách các node cần xét
        vector<string> toVisit;
        queue<string> tempQueue = toVisitQueue;
        while (!tempQueue.empty()) {
            string node = tempQueue.front();
            tempQueue.pop();
            if (lastNodes.find(node) == lastNodes.end()) {
                toVisit.push_back(node);
            }
        }
        // Thêm thông tin vào bảng
        table.push_back(formatRow(lastNode, nextStates, visited, toVisit));

        if (lastNode == end) return path;
    }
    return {};
}
// Hàm in bảng vào tệp
void printTableToFile(ofstream& outputFile, const vector<string>& table, const vector<size_t>& columnWidths) {
    outputFile << left << setw(columnWidths[0]) << "Xet"
        << " | " << setw(columnWidths[1]) << "Trang thai ke tiep"
        << " | " << setw(columnWidths[2]) << "Danh sach cac dinh da xet"
        << " | " << setw(columnWidths[3]) << "Danh sach can xet" << endl;
    outputFile << string(columnWidths[0] + columnWidths[1] + columnWidths[2] + columnWidths[3], '-') << endl;

    for (const string& row : table) {
        outputFile << row << endl;
    }
}
// Hàm in đường đi vào tệp
void printPathToFile(ofstream& outputFile, const vector<string>& path, const string& label) {
    outputFile << "\n=== " << label << " Path ===\n";
    if (!path.empty()) {
        for (size_t i = 0; i < path.size(); ++i) {
            outputFile << path[i];
            if (i != path.size() - 1) outputFile << " => ";
        }
        outputFile << endl;
    }
    else {
        outputFile << "No path found." << endl;
    }
}

int main() {
    string inputFilename = "input.txt";
    string outputFilename = "output.txt";

    unordered_map<string, vector<string>> graph;
    string start, end;
    readGraphAndEndpoints(inputFilename, graph, start, end);

    vector<string> steps;  // Lưu lại các bước thực hiện
    vector<string> table;
    vector<string> bfsPath = BFS(graph, start, end, steps, table);

    vector<size_t> columnWidths = { 4, 18, 26, 25 };
    calculateColumnWidths(table, columnWidths);

    ofstream outputFile(outputFilename);
    if (!outputFile) {
        cerr << "Khong mo duoc tep: " << outputFilename << endl;
        return 1;
    }

    printTableToFile(outputFile, table, columnWidths);
    printPathToFile(outputFile, bfsPath, "BFS");

    outputFile.close();
    cout << "Ket qua da duoc ghi vao tep: " << outputFilename << endl;

    return 0;
}
