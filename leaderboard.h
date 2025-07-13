#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

struct ScoreEntry {
    string name;
    string problem;
    int score;
};

 
vector<ScoreEntry> loadScores() {
    ifstream file("scores.txt");
    unordered_map<string, unordered_map<string, int>> userProblemMap;
    vector<ScoreEntry> scores;
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string name, problem;
        int score;

        if (getline(ss, name, ',') && getline(ss, problem, ',') && ss >> score) {
            int& existingScore = userProblemMap[name][problem];
            existingScore = max(existingScore, score); // keep highest score only
        }
    }

    // Convert map to vector
    for (const auto& [user, problems] : userProblemMap) {
        for (const auto& [problem, score] : problems) {
            scores.push_back({ user, problem, score });
        }
    }

    return scores;
}

// Updates scores.txt: rewrites with higher score if applicable
void saveScore(const string& username, const string& problemTitle, int newScore) {
    vector<ScoreEntry> scores = loadScores();
    bool updated = false;

    for (auto& entry : scores) {
        if (entry.name == username && entry.problem == problemTitle) {
            if (newScore > entry.score) {
                entry.score = newScore;
            }
            updated = true;
            break;
        }
    }

    if (!updated) {
        scores.push_back({ username, problemTitle, newScore });
    }

    ofstream file("scores.txt");
    for (const auto& entry : scores) {
        file << entry.name << "," << entry.problem << "," << entry.score << "\n";
    }
}

// Displays total score per user, sorted descending
void showLeaderboard() {
    vector<ScoreEntry> scores = loadScores();
    unordered_map<string, int> totalScores;

    for (const auto& entry : scores) {
        totalScores[entry.name] += entry.score;
    }

    vector<pair<string, int>> leaderboard(totalScores.begin(), totalScores.end());
    sort(leaderboard.begin(), leaderboard.end(), [](const auto& a, const auto& b) {
        return b.second < a.second; // descending
    });

    cout << "\n=== Leaderboard ===\n";
    cout << "Name\t\tTotal Score\n";
    cout << "-----------------------------\n";
    for (const auto& [name, score] : leaderboard) {
        cout << name << "\t\t" << score << "\n";
    }
}
