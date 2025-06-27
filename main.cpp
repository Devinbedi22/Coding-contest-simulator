#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <chrono>
#include <filesystem>
#include <thread>
#include <atomic>
#include "leaderboard.h"
#include "auth.h"

using namespace std;
namespace fs = std::filesystem;

atomic<bool> timeUp(false);

struct Problem {
    string title, description, inputFormat, outputFormat;
    string sampleInput, sampleOutput;
    vector<string> testcaseInputPaths;
    vector<string> expectedOutputPaths;
};

vector<Problem> loadProblems(const string& filepath) {
    ifstream file(filepath);
    vector<Problem> problems;

    if (!file) {
        cerr << "❌ Problem file not found: " << filepath << "\n";
        exit(1);
    }

    string line;
    while (getline(file, line)) {
        if (line == "===" || line.empty()) continue;

        Problem p;
        p.title = line;
        getline(file, p.description);
        getline(file, p.inputFormat);
        getline(file, p.outputFormat);
        getline(file, p.sampleInput);
        getline(file, p.sampleOutput);

        string inputPaths, outputPaths;
        getline(file, inputPaths);
        getline(file, outputPaths);

        stringstream ssIn(inputPaths), ssOut(outputPaths);
        string path;
        while (getline(ssIn, path, ',')) p.testcaseInputPaths.push_back(path);
        while (getline(ssOut, path, ',')) p.expectedOutputPaths.push_back(path);

        problems.push_back(p);
    }

    return problems;
}

void showProblem(const Problem& p) {
    cout << "\n=== " << p.title << " ===\n";
    cout << p.description << "\n";
    cout << "Input: " << p.inputFormat << "\n";
    cout << "Output: " << p.outputFormat << "\n";
    cout << "\nSample Input:\n" << (p.sampleInput.empty() ? "<none>" : p.sampleInput);
    cout << "\nSample Output:\n" << (p.sampleOutput.empty() ? "<none>" : p.sampleOutput) << "\n";
}

bool compareOutput(const string& outputPath, const string& expectedPath) {
    ifstream outFile(outputPath), expectedFile(expectedPath);
    string outLine, expLine;
    while (getline(outFile, outLine) && getline(expectedFile, expLine)) {
        if (outLine != expLine) return false;
    }
    return outFile.eof() && expectedFile.eof();
}

void startCountdown(int minutes) {
    int seconds = minutes * 60;
    while (seconds > 0 && !timeUp) {
        this_thread::sleep_for(chrono::seconds(1));
        --seconds;
    }
    if (!timeUp) {
        cout << "\n⏰ Time's up! Auto-submitting...\n";
        timeUp = true;
    }
}

int main() {
    cout << "=== Welcome to CLICODING Contest Simulator ===\n";
    cout << "1. Login\n2. Register\n3. View Leaderboard\n4. Exit\nChoose: ";
    int choice;
    cin >> choice;

    string username;
    if (choice == 1) {
        username = loginUser();
        if (username.empty()) return 1;
    } else if (choice == 2) {
        username = registerUser();
        if (username.empty()) return 1;
    } else if (choice == 3) {
        showLeaderboard();
        return 0;
    } else {
        cout << "Exiting...\n";
        return 0;
    }

    vector<Problem> problems = loadProblems("problems.txt");
    if (problems.empty()) {
        cerr << "❌ No problems found.\n";
        return 1;
    }

    cout << "\nAvailable Problems:\n";
    for (size_t i = 0; i < problems.size(); ++i) {
        cout << i + 1 << ". " << problems[i].title << "\n";
    }

    cout << "\nChoose a problem (1-" << problems.size() << "): ";
    int index;
    cin >> index;

    if (index < 1 || index > (int)problems.size()) {
        cerr << "❌ Invalid selection.\n";
        return 1;
    }

    Problem p = problems[index - 1];
    showProblem(p);

    cout << "\nYou have 10 minutes to solve this problem.\n";
    thread countdownThread(startCountdown, 10);

    cout << "\nEnter path to your C++ submission file (e.g., submission.cpp): ";
    string filepath;
    cin >> filepath;
    timeUp = true; // stop timer when user submits
    countdownThread.join();

    cout << "\nCompiling your code...\n";
    string compileCmd = "g++ \"" + filepath + "\" -o submission.exe";
    if (system(compileCmd.c_str()) != 0) {
        cout << "❌ Compilation Error!\n";
        return 1;
    }

    int passed = 0;
    for (size_t i = 0; i < p.testcaseInputPaths.size(); ++i) {
        if (!fs::exists(p.testcaseInputPaths[i]) || !fs::exists(p.expectedOutputPaths[i])) {
            cerr << "❌ Missing test case files.\n";
            continue;
        }

        string cmd = ".\\submission.exe < \"" + p.testcaseInputPaths[i] + "\" > output.txt";
        if (system(cmd.c_str()) != 0) {
            cout << "❌ Runtime Error on test case " << i + 1 << "\n";
            continue;
        }

        if (compareOutput("output.txt", p.expectedOutputPaths[i])) {
            cout << "✅ Test Case " << i + 1 << " Passed\n";
            passed++;
        } else {
            cout << "❌ Test Case " << i + 1 << " Failed\n";
        }
    }

    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::minutes>(end - chrono::steady_clock::now());

    int score = static_cast<int>((double)passed / p.testcaseInputPaths.size() * 100);
    cout << "Total Passed: " << passed << "/" << p.testcaseInputPaths.size() << "\n";
    cout << "Score: " << score << "\n";
    cout << "Time taken: " << duration.count() << " minute(s).\n";

    saveScore(username, p.title, score);
    return 0;
}
