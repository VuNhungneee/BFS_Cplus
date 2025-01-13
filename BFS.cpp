// BFS.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu
// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <locale>
#include <codecvt>
#include <iomanip>
using namespace std;
// Hàm đọc đồ thị từ tệp
unordered_map<string, vector<string>> readGraphFromFile(const string& filename) 
{
    unordered_map<string, vector<string>> graph;
    ifstream inputFile(filename);

    if (!inputFile) {
        cerr << "Không thể mở tệp: " << filename << endl;
        exit(1);
    }

    string line;
    while (getline(inputFile, line)) {
        stringstream ss(line);
        string node, neighbor;
        ss >> node; // Đọc đỉnh đầu tiên
        while (ss >> neighbor) {
            graph[node].push_back(neighbor); // Đọc các đỉnh kề
        }
    }

    inputFile.close();
    return graph;
}

// BFS để tìm đường đi ngắn nhất
vector<string> BFS(unordered_map<string, vector<string>>& graph, string start, string end, vector<string>& steps, vector<string>& table) 
{
    queue<vector<string>> q; // Hàng đợi lưu đường đi
    unordered_set<string> visited; // Tập các điểm đã thăm

    q.push({ start });
    visited.insert(start);

    while (!q.empty()) {
        // Lấy phần tử đầu của hàng đợi
        vector<string> path = q.front();
        q.pop();

        string lastNode = path.back(); // Đỉnh đang xét
        stringstream tableRow;         // Dòng để ghi vào bảng

        // Tạo trạng thái cho bảng
        vector<string> nextStates; // Các đỉnh liền kề sẽ được thêm vào hàng đợi
        for (const string& next : graph[lastNode]) {
            if (visited.find(next) == visited.end()) {
                nextStates.push_back(next);
                visited.insert(next);

                vector<string> newPath = path;
                newPath.push_back(next);
                q.push(newPath);
            }
        }

        // Ghi lại các thông tin cho bảng
        tableRow << lastNode << " || ";
        for (const string& state : nextStates) {
            tableRow << state << " ";
        }
        tableRow << "|| ";
        for (const string& node : visited) {
            tableRow << node << " ";
        }
        tableRow << "|| ";
        queue<vector<string>> tempQueue = q;
        while (!tempQueue.empty()) {
            vector<string> tempPath = tempQueue.front();
            tempQueue.pop();
            tableRow << tempPath.back() << " ";
        }

        table.push_back(tableRow.str());

        // Kiểm tra xem đã đến đích chưa
        if (lastNode == end) {
            steps.push_back("Found path!");
            return path;
        }
    }

    steps.push_back("Not found path!");
    return {};
}

int main() 
{
    locale loc(locale(), new codecvt_utf8<wchar_t>);
    string inputFilename = "input.txt";
    string outputFilename = "output.txt";

    unordered_map<string, vector<string>> graph = readGraphFromFile(inputFilename);

    // Tìm đường đi ngắn nhất từ A đến G
    string start = "A";
    string end = "G";
    vector<string> steps;  // Lưu lại các bước thực hiện
    vector<string> table;  // Bảng ghi trạng thái thực hiện
    vector<string> shortestPath = BFS(graph, start, end, steps, table);

    // Ghi kết quả vào tệp output
    ofstream outputFile(outputFilename);
    if (!outputFile) {
        cerr << "Khong mo duoc tep: " << outputFilename << endl;
        return 1;
    }

    outputFile << "=== Bang thuc hien ===\n";
    outputFile << "Xet || Trang thai ke tiep || Danh sach cac dinh da xet || Danh sach can xet\n";
    outputFile << "---------------------------------------------------------------\n";
    for (const string& row : table) {
        outputFile << row << endl;
    }

    outputFile << "\n=== Duong di ngan nhat ===\n";
    if (!shortestPath.empty()) {
        for (size_t i = 0; i < shortestPath.size(); ++i) {
            outputFile << shortestPath[i];
            if (i != shortestPath.size() - 1) outputFile << " => ";
        }
        outputFile << endl;
    }
    else {
        outputFile << "Not found path " << start << " to " << end << endl;
    }

    outputFile.close();
    cout << "Ket qua da duoc ghi vao file: " << outputFilename << endl;

    return 0;
}
