#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <stdexcept>

using namespace std;

bool isValidPermutation(const vector<int>& key, int d) {
    if (key.size() != d) {
        return false;
    }

    vector<bool> used(d + 1, false);

    for (int x : key) {
        if (x < 1 || x > d || used[x]) {
            return false;
        }
        used[x] = true;
    }

    return true;
}

string encrypt(const string& plaintext, const vector<int>& key, int d) {
    string text = plaintext;

    while (text.length() % d != 0) {
        text += 'X';
    }

    string ciphertext = text;

    for (size_t start = 0; start < text.length(); start += d) {
        for (int i = 0; i < d; i++) {
            ciphertext[start + i] =
                text[start + key[i] - 1];
        }
    }

    return ciphertext;
}

string decrypt(const string& ciphertext, const vector<int>& key, int d) {
    if (ciphertext.length() % d != 0) {
        throw runtime_error(
            "Ciphertext length must be divisible by d."
        );
    }

    string plaintext = ciphertext;

    for (size_t start = 0; start < ciphertext.length(); start += d) {
        for (int i = 0; i < d; i++) {
            plaintext[start + key[i] - 1] =
                ciphertext[start + i];
        }
    }

    return plaintext;
}

int main() {
    int option;

    cout << "===== PERMUTATION CIPHER =====\n";
    cout << "[1] Encrypt\n";
    cout << "[2] Decrypt\n";
    cout << "[0] Exit\n";
    cout << "Choose: ";

    if (!(cin >> option) || (option != 0 && option != 1 && option != 2)) {
        cout << "Invalid option.\n";
        return 1;
    }

    if (option == 0) return 0;

    int d;

    cout << "\nEnter permutation degree d: ";
    cin >> d;

    if (d <= 0) {
        cout << "d must be greater than 0.\n";
        return 1;
    }

    vector<int> key(d);

    cout << "Enter permutation key (" << d << " numbers from 1 to " << d << "): ";

    for (int i = 0; i < d; i++) {
        cin >> key[i];
    }

    if (!isValidPermutation(key, d)) {
        cout << "Invalid permutation key.\n";
        return 1;
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string text;

    if (option == 1) {
        cout << "Enter plaintext: ";
        getline(cin, text);
        string result = encrypt(text, key, d);
        cout << "\n===== ENCRYPTED =====\n";
        cout << result << '\n';
    }
    else {
        cout << "Enter ciphertext: ";
        getline(cin, text);
        try {
            string result = decrypt(text, key, d);

            cout << "\n===== DECRYPTED =====\n";
            cout << result << '\n';

            cout << "\nNote: trailing X characters "
                    "may be padding.\n";
        }
        catch (const exception& e) {
            cout << "Error: " << e.what() << '\n';
            return 1;
        }
    }

    return 0;
}