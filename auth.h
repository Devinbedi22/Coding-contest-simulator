#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include "sha256.h"  // Include the SHA256 hashing function

using namespace std;

string toLower(const string& s) {
    string result = s;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

string loginUser() {
    unordered_map<string, string> credentials;
    ifstream file("users.txt");
    string line, user, pass;

    while (getline(file, line)) {
        size_t comma = line.find(',');
        if (comma != string::npos) {
            string storedUser = toLower(line.substr(0, comma));
            string storedPass = line.substr(comma + 1);
            credentials[storedUser] = storedPass;
        }
    }

    cout << "Enter username: ";
    cin >> user;
    cout << "Enter password: ";
    cin >> pass;

    string loweredUser = toLower(user);
    string hashedPass = sha256(pass);

    if (credentials.count(loweredUser) && credentials[loweredUser] == hashedPass) {
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
            credentials[toLower(existing)] = line.substr(comma + 1);
        }
    }

    cout << "Choose username: ";
    cin >> user;
    string loweredUser = toLower(user);

    if (credentials.count(loweredUser)) {
        cout << "❌ Username already exists.\n";
        return "";
    }

    if (user.empty() || user.find(',') != string::npos) {
        cout << "❌ Invalid username. Cannot be empty or contain commas.\n";
        return "";
    }

    cout << "Choose password: ";
    cin >> pass;
    if (pass.empty() || pass.find(',') != string::npos) {
        cout << "❌ Invalid password. Cannot be empty or contain commas.\n";
        return "";
    }

    string hashedPass = sha256(pass);

    ofstream outFile("users.txt", ios::app);
    outFile << user << "," << hashedPass << "\n";

    cout << "✅ Registration successful.\n";
    return user;
}
