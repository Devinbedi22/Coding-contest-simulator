#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

using namespace std;

string loginUser() {
    unordered_map<string, string> credentials;
    ifstream file("users.txt");
    string line, user, pass;

    while (getline(file, line)) {
        size_t comma = line.find(',');
        if (comma != string::npos) {
            user = line.substr(0, comma);
            pass = line.substr(comma + 1);
            credentials[user] = pass;
        }
    }

    cout << "Enter username: ";
    cin >> user;
    cout << "Enter password: ";
    cin >> pass;

    if (credentials.count(user) && credentials[user] == pass) {
        cout << "✅ Login successful.\n";
        return user;
    } else {
        cout << "❌ Invalid credentials.\n";
        return "";
    }
}

string registerUser() {
    string user, pass;
    unordered_map<string, string> credentials;
    ifstream file("users.txt");
    string line;

    while (getline(file, line)) {
        size_t comma = line.find(',');
        if (comma != string::npos) {
            string existing = line.substr(0, comma);
            credentials[existing] = line.substr(comma + 1);
        }
    }

    cout << "Choose username: ";
    cin >> user;
    if (credentials.count(user)) {
        cout << "❌ Username already exists.\n";
        return "";
    }

    cout << "Choose password: ";
    cin >> pass;

    ofstream outFile("users.txt", ios::app);
    outFile << user << "," << pass << "\n";

    cout << "✅ Registration successful.\n";
    return user;
}
