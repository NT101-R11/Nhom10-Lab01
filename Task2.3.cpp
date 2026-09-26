// hdatuan
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <iomanip>
#include <fstream> 

using namespace std;

struct CharFreq {
    char character;
    int count;
};

struct StringFreq {
    string text;
    int count;
};

bool compareCharFreq(const CharFreq& a, const CharFreq& b) {
    return a.count > b.count;
}

bool compareStringFreq(const StringFreq& a, const StringFreq& b) {
    if (a.count != b.count)
        return a.count > b.count;
    return a.text < b.text;
}

string toUpperText(string text) {
    for (char& c : text) {
        c = toupper((unsigned char)c);
    }
    return text;
}

vector<CharFreq> analyzeCharFrequency(const string& ciphertext, int& totalChars) {
    map<char, int> freqMap;
    totalChars = 0;

    for (char c : ciphertext) {
        if (isalpha((unsigned char)c)) {
            c = toupper((unsigned char)c);
            freqMap[c]++;
            totalChars++;
        }
    }

    vector<CharFreq> freq;
    for (const auto& p : freqMap) {
        freq.push_back({p.first, p.second});
    }
    sort(freq.begin(), freq.end(), compareCharFreq);
    return freq;
}

void printCharFrequency(const vector<CharFreq>& freq, int totalChars) {
    cout << "\n===== BANG THONG KE TAN SUAT TRONG BAN MA =====\n";
    cout << "\n* Thu tu tan suat tieng Anh tham khao:\n";
    cout << "  E T A O I N S H R D L C U M W F G Y P B V K J X Q Z\n";
    cout << "\nKy tu\tSo lan\tTy le (%)\n";

    for (const auto& item : freq) {
        double percentage = (double)item.count / totalChars * 100.0;
        cout << "  " << item.character << '\t' << item.count << '\t'
             << fixed << setprecision(2) << percentage << "%\n";
    }
}

void extractNGramsFromWord(const string& word, int n, map<string, int>& freqMap) {
    if ((int)word.length() < n) return;
    for (int i = 0; i + n <= (int)word.length(); i++) {
        freqMap[word.substr(i, n)]++;
    }
}

vector<StringFreq> analyzeNGram(const string& ciphertext, int n) {
    map<string, int> freqMap;
    string word;

    for (char c : ciphertext) {
        if (isalpha((unsigned char)c)) {
            word += toupper((unsigned char)c);
        } else {
            extractNGramsFromWord(word, n, freqMap);
            word.clear();
        }
    }
    extractNGramsFromWord(word, n, freqMap); 

    vector<StringFreq> freq;
    for (const auto& p : freqMap) {
        freq.push_back({p.first, p.second});
    }
    sort(freq.begin(), freq.end(), compareStringFreq);
    return freq;
}

void printNGram(const vector<StringFreq>& freq, int n, int limit) {
    if (n == 2) {
        cout << "\n===== BIGRAM FREQUENCY =====\n";
        cout << "\n* Bigram tieng Anh pho bien: TH HE IN ER AN RE ON AT EN ND\n";
    } else if (n == 3) {
        cout << "\n===== TRIGRAM FREQUENCY =====\n";
        cout << "\n* Trigram tieng Anh pho bien: THE AND ING HER ERE ENT THA NTH WAS ETH\n";
    }

    cout << "\nChuoi\tSo lan\n";
    int number = min(limit, (int)freq.size());
    for (int i = 0; i < number; i++) {
        cout << freq[i].text << '\t' << freq[i].count << '\n';
    }
}

vector<StringFreq> analyzeWordFrequency(const string& ciphertext) {
    map<string, int> freqMap;
    string word;

    for (char c : ciphertext) {
        if (isalpha((unsigned char)c)) {
            word += toupper((unsigned char)c);
        } else {
            if (!word.empty()) {
                freqMap[word]++;
                word.clear();
            }
        }
    }
    if (!word.empty()) {
        freqMap[word]++;
    }

    vector<StringFreq> freq;
    for (const auto& p : freqMap) {
        freq.push_back({p.first, p.second});
    }
    sort(freq.begin(), freq.end(), compareStringFreq);
    return freq;
}

void printWordFrequency(const vector<StringFreq>& freq, int limit) {
    cout << "\n===== WORD FREQUENCY =====\n";
    cout << "\nTu\tSo lan\n";
    int number = min(limit, (int)freq.size());
    for (int i = 0; i < number; i++) {
        cout << freq[i].text << '\t' << freq[i].count << '\n';
    }
}

map<char, char> createInitialGuess() {
    map<char, char> decryptMap;
    for (char c = 'A'; c <= 'Z'; c++) {
        decryptMap[c] = '_';
    }
    return decryptMap;
}

void printMapping(const map<char, char>& decryptMap) {
    cout << "\n===== BANG ANH XA HIEN TAI =====\n\n";
    cout << "Cipher : ";
    for (char c = 'A'; c <= 'Z'; c++) cout << c << ' ';
    cout << '\n';

    cout << "Plain  : ";
    for (char c = 'A'; c <= 'Z'; c++) {
        auto it = decryptMap.find(c);
        if (it != decryptMap.end())
            cout << it->second << ' ';
        else
            cout << "_ ";
    }
    cout << '\n';
}

string decryptText(const string& ciphertext, const map<char, char>& decryptMap) {
    string plaintext;
    for (char c : ciphertext) {
        if (isalpha((unsigned char)c)) {
            char upper = toupper((unsigned char)c);
            auto it = decryptMap.find(upper);
            if (it != decryptMap.end() && it->second != '_') {
                plaintext += it->second;
            } else {
                plaintext += '_'; // Ký tự chưa đoán sẽ hiển thị là dấu gạch dưới
            }
        } else {
            plaintext += c;
        }
    }
    return plaintext;
}

void updateMapping(map<char, char>& decryptMap, char cipherChar, char plainChar) {
    cipherChar = toupper((unsigned char)cipherChar);
    plainChar = toupper((unsigned char)plainChar);

    if (cipherChar < 'A' || cipherChar > 'Z' || plainChar < 'A' || plainChar > 'Z') {
        cout << "Ky tu khong hop le!\n";
        return;
    }

    // Đảm bảo tính chất đơn ánh (1-1): Một chữ Plain không được gán cho 2 chữ Cipher khác nhau
    for (const auto& p : decryptMap) {
        if (p.second == plainChar && p.first != cipherChar) {
            cout << "Canh bao: Chuan " << plainChar << " dang duoc anh xa boi cipher " << p.first << ".\n";
            cout << "Monoalphabetic substitution yeu cau anh xa 1-1.\n";
            return;
        }
    }
    decryptMap[cipherChar] = plainChar;
    cout << "Da cap nhat: " << cipherChar << " -> " << plainChar << '\n';
}

int main() {
    ifstream inputFile("input/task2.3.txt");
    if (!inputFile.is_open()) {
        cerr << "Loi: Khong the mo file input/task2.3.txt\n";
        return 1;
    }

    string ciphertext, readline;
    while (getline(inputFile, readline)) {
        ciphertext += readline + "\n";
    }
    inputFile.close();
    ciphertext = toUpperText(ciphertext);

    // 1. Phân tích tần suất
    int totalChars;
    vector<CharFreq> charFreq = analyzeCharFrequency(ciphertext, totalChars);
    printCharFrequency(charFreq, totalChars);

    vector<StringFreq> bigramFreq = analyzeNGram(ciphertext, 2);
    printNGram(bigramFreq, 2, 10);

    vector<StringFreq> trigramFreq = analyzeNGram(ciphertext, 3);
    printNGram(trigramFreq, 3, 10);

    vector<StringFreq> wordFreq = analyzeWordFrequency(ciphertext);
    printWordFrequency(wordFreq, 10);

    // 2. Khởi tạo bản đồ trống
    map<char, char> decryptMap = createInitialGuess();

    // 3. Vòng lặp tương tác chính xác
    char cChar, pChar;
    while (true) {
        printMapping(decryptMap);
        
        cout << "\n--- GOI Y VAN BAN HIEN TAI ---\n";
        cout << decryptText(ciphertext, decryptMap) << "\n";
        cout << "-------------------------------\n";
        
        cout << "Nhap cap ky tu (Cipher Plain). Nhap '0 0' de HOAN THANH va xuat file: ";
        if (!(cin >> cChar >> pChar)) break;
        
        if (cChar == '0' && pChar == '0') break;
        updateMapping(decryptMap, cChar, pChar);
    }

    // 4. Xuất dữ liệu ra file output
    ofstream outputFile("output/task2.3.txt");
    if (outputFile.is_open()) {
        outputFile << decryptText(ciphertext, decryptMap);
        outputFile.close();
        cout << "\nDa xuat ket qua giai ma thanh cong vao file output/task2.3.txt!\n";
    } else {
        cerr << "Loi: Khong ghi duoc file output.\n";
    }

    return 0;
}
