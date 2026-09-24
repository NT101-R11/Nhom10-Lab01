#include <iostream>
#include <string>
#include <vector>
#include <cctype>

using namespace std;

char normalize_char(char c) {
    c = toupper(c);
    if (c == 'J') return 'I';
    return c;
}

string normalize_text(const string& text) {
    string result = "";
    for (char c : text) {
        if (isalpha(c)) {
            result += normalize_char(c);
        }
    }
    return result;
}

vector<string> gen_matrix(const string& key) {
    vector<string> matrix(5, string(5, ' '));
    string clean_key = "";

    for (char c : key) {
        if (isalpha(c)) {
            char norm_c = normalize_char(c);
            if (clean_key.find(norm_c) == string::npos) {
                clean_key += norm_c;
            }
        }
    }

    string alphabet = "ABCDEFGHIKLMNOPQRSTUVWXYZ";
    for (char c : alphabet) {
        if (clean_key.find(c) == string::npos) {
            clean_key += c;
        }
    }

    int idx = 0;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            matrix[i][j] = clean_key[idx++];
        }
    }

    return matrix;
}

void print_matrix(const vector<string>& matrix) {
    cout << "\n--- MA TRAN PLAYFAIR 5x5 ---\n";
    for (int i = 0; i < 5; ++i) {
        cout << "  ";
        for (int j = 0; j < 5; ++j) {
            cout << matrix[i][j] << "  ";
        }
        cout << "\n";
    }
    cout << "--------------------------\n\n";
}

void find_pos(const vector<string>& matrix, char c, int& row, int& col) {
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            if (matrix[i][j] == c) {
                row = i;
                col = j;
                return;
            }
        }
    }
}

string prepare_for_encrypt(string text) {
    text = normalize_text(text);
    string prepared = "";

    for (size_t i = 0; i < text.length(); ) {
        char a = text[i];
        if (i + 1 < text.length()) {
            char b = text[i + 1];
            if (a == b) {
                char filler = (a == 'X') ? 'Q' : 'X';
                prepared += a;
                prepared += filler;
                i++;
            } else {
                prepared += a;
                prepared += b;
                i += 2;
            }
        } else {
            char filler = (a == 'X') ? 'Q' : 'X';
            prepared += a;
            prepared += filler;
            i++;
        }
    }
    return prepared;
}

string playfair_process(const string& text, const vector<string>& matrix, bool is_encrypt) {
    string result = "";
    string clean_text = normalize_text(text);

    if (clean_text.length() % 2 != 0) {
        clean_text += 'X';
    }

    for (size_t i = 0; i < clean_text.length(); i += 2) {
        char a = clean_text[i];
        char b = clean_text[i + 1];

        int r1, c1, r2, c2;
        find_pos(matrix, a, r1, c1);
        find_pos(matrix, b, r2, c2);

        if (r1 == r2) {
            int shift = is_encrypt ? 1 : -1;
            result += matrix[r1][(c1 + shift + 5) % 5];
            result += matrix[r2][(c2 + shift + 5) % 5];
        } else if (c1 == c2) {
            int shift = is_encrypt ? 1 : -1;
            result += matrix[(r1 + shift + 5) % 5][c1];
            result += matrix[(r2 + shift + 5) % 5][c2];
        } else {
            result += matrix[r1][c2];
            result += matrix[r2][c1];
        }
    }
    return result;
}

string clean_decrypted_text(const string& text) {
    string result = "";
    size_t n = text.length();

    for (size_t i = 0; i < n; ++i) {
        char c = text[i];

        if (c == 'X' || c == 'Q') {
            bool is_filler = false;

            if (i > 0 && i + 1 < n && text[i - 1] == text[i + 1]) {
                is_filler = true;
            }
            else if (i == n - 1) {
                is_filler = true;
            }

            if (is_filler) {
                continue;
            }
        }
        result += c;
    }
    return result;
}

int main() {
    int choice;
    string key, text;

    cout << "========================================\n";
    cout << "        playfair cipher application     \n";
    cout << "========================================\n";

    cout << "enter key: ";
    getline(cin, key);

    string temp_key = "";
    for(char c : key) if(isalpha(c)) temp_key += c;
    if (temp_key.empty()) {
        cout << "your key contain invalid character(s)\n";
        return 1;
    }

    vector<string> matrix = gen_matrix(key);
    print_matrix(matrix);

    while (true) {
        cout << "choose function:\n";
        cout << "1. encrypt\n";
        cout << "2. decrypt\n";
        cout << "3. exit\n";
        cout << "your option: ";
        cin >> choice;
        cin.ignore();

        if (choice == 3) break;

        if (choice == 1 || choice == 2) {
            cout << "etner text: \n";
            getline(cin, text);

            if (text.empty()) {
                cout << "content cant be empty\n";
                continue;
            }

            if (choice == 1) {
                string prepared_text = prepare_for_encrypt(text);
                string ciphertext = playfair_process(prepared_text, matrix, true);

                cout << "\n--- encryption result ---\n";
                cout << "prepared: " << prepared_text << "\n";
                cout << "ciphertext: " << ciphertext << "\n";
            } else {
                string raw_plaintext = playfair_process(text, matrix, false);
                string cleaned_plaintext = clean_decrypted_text(raw_plaintext);

                cout << "\n--- decryption result ---\n";
                cout << "plaintext (removed X/Q filler): \n" << cleaned_plaintext << "\n";
                cout << "\n(i removed it for fun, might contain the original (not filler) text, please recheck)\n";
            }
            cout << "========================\n\n";
        } else {
            cout << "invalid option\n";
        }
    }

    cout << "aborting...\n";
    return 0;
}
