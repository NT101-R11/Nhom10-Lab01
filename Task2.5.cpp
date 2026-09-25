#include <cctype>
#include <limits>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstdlib>

using namespace std;

void clrscr() {
#if defined(_WIN32) || defined(_WIN64)
    system("cls");
#else
    system("clear");
#endif
}

// Normalizes the string by keeping ONLY alphabetic characters and converting them to lowercase
string normalize(const string& s) {
    string result;
    result.reserve(s.length());
    for (char c : s) {
        if (isalpha(static_cast<unsigned char>(c))) {
            result.push_back(tolower(static_cast<unsigned char>(c)));
        }
    }
    return result;
}

// Converts normalized string to vector of int (0-25)
vector<int> stovi(const string& k) {
    vector<int> result;
    result.reserve(k.length());
    for (char c : k) {
        if (c < 'a' || c > 'z') {
            throw runtime_error("Invalid character(s) in key, aborting");
        }
        result.push_back(c - 'a');
    }
    return result;
}

void printVector(const vector<int>& v) {
    for (int i : v) {
        cout << i << " ";
    }
}

string shift(const string& original, const vector<int>& key, bool is_encrypt = true) {
    string result;
    result.reserve(original.length());
    int key_idx = 0; // Tracks key position independently of string index

    for (char c : original) {
        if (isalpha(static_cast<unsigned char>(c))) {
            int k = key[key_idx % key.size()];
            if (!is_encrypt) {
                k = -k;
            }

            bool is_upper = isupper(static_cast<unsigned char>(c));
            char base = is_upper ? 'A' : 'a';

            // Robust modulo arithmetic that correctly handles negative numbers in C++
            int shifted = ((c - base + k) % 26 + 26) % 26;
            result.push_back(static_cast<char>(base + shifted));

            key_idx++; // Only advance key index for alphabetic characters
        } else {
            // Preserve spaces, punctuation, and numbers exactly as they are
            result.push_back(c);
        }
    }
    return result;
}

int main() {
    int option = -1;
    do {
        clrscr();
        cout << "==== Vigenere Encrypt/Decrypt Tool ====\n"
             << "[1] Encrypt\n"
             << "[2] Decrypt\n"
             << "[0] Exit\n"
             << "Choose your option: ";

        // Handle invalid non-integer inputs gracefully
        if (!(cin >> option)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
    } while (option != 0 && option != 1 && option != 2);

    if (option == 0) {
        return 0;
    }

    // Clear the newline character left in the buffer by `cin >> option`
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    try {
        clrscr();
        cout << "Enter key: ";
        string key;
        getline(cin, key);

        key = normalize(key);
        if (key.empty()) {
            cout << "\nError: Key must contain at least one alphabetic character.\n";
            cout << "Press any key to exit...";
            cin.get();
            return 1;
        }

        vector<int> pkey = stovi(key);
        cout << "Processed key: ";
        printVector(pkey);
        cout << "\n";

        string prompt = (option == 1) ? "Enter plaintext: " : "Enter ciphertext: ";
        cout << prompt;

        string text;
        getline(cin, text);

        string result_text = shift(text, pkey, option == 1);

        cout << "\n==== " << (option == 1 ? "ENCRYPTED" : "DECRYPTED") << " ====\n"
             << "Result:\n"
             << result_text << "\n\n";

        cout << "Press any key to exit: ";
        cin.get();

    } catch (const exception& e) {
        cerr << "\nCritical Error: " << e.what() << "\n";
        cout << "Press any key to exit: ";
        cin.get();
        return 1;
    }

    return 0;
}
