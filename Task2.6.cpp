#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cctype>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <map>

using namespace std;

// letter frequency (source: https://en.wikipedia.org/wiki/Letter_frequency)
// 0 = a, 25 = z
const vector<double> EXPECTED_FREQ = {
    0.082, 0.015, 0.028, 0.043, 0.127, 0.022, 0.020, 0.061, 0.070, 0.002,
    0.008, 0.040, 0.024, 0.067, 0.075, 0.019, 0.001, 0.060, 0.063, 0.091,
    0.028, 0.010, 0.024, 0.002, 0.020, 0.001
};

string normalize(const string& text) {
    string normalized = "";
    for (char c : text) {
        if (isalpha((unsigned char)c))
            normalized += tolower((unsigned char)c);
    }
    return normalized;
}

map<int, int> kasiskiExamination(const string& text) {
    map<int, int> factorCounts;
    int n = text.length();
    for (int len = 3; len <= 5; ++len) {
        for (int i = 0; i <= n - len; ++i) {
            string sub = text.substr(i, len);
            size_t pos = text.find(sub, i + len);
            while (pos != string::npos) {
                int distance = pos - i;
                for (int f = 2; f <= min(distance, 20); ++f) {
                    if (distance % f == 0)
                        factorCounts[f]++;
                }
                pos = text.find(sub, pos + len);
            }
        }
    }
    return factorCounts;
}

double calculateIoC(const string& text) {
    vector<int> freq(26, 0);
    int n = 0;
    for (char c : text) { freq[c - 'a']++; n++; }
    if (n <= 1) return 0.0;
    double ic = 0.0;
    for (int count : freq) ic += count * (count - 1);
    return ic / (n * (n - 1));
}

double chiSquaredForColumn(const string& col, int shift) {
    vector<int> observed(26, 0);
    for (char c : col) {
        int dec = (c - 'a' - shift + 26) % 26;
        observed[dec]++;
    }
    double chi = 0.0;
    int n = col.length();
    for (int i = 0; i < 26; ++i) {
        double expected = n * EXPECTED_FREQ[i];
        if (expected > 0)
            chi += pow(observed[i] - expected, 2) / expected;
    }
    return chi;
}

pair<int, double> findBestShiftForColumn(const string& col) {
    int bestShift = 0;
    double minChi = 1e18;
    for (int shift = 0; shift < 26; ++shift) {
        double chi = chiSquaredForColumn(col, shift);
        if (chi < minChi) {
            minChi = chi;
            bestShift = shift;
        }
    }
    return {bestShift, minChi};
}

string decrypt(const string& originalText, const string& key) {
    string plaintext = "";
    int keyIdx = 0;
    for (char c : originalText) {
        if (isalpha((unsigned char)c)) {
            bool isUpper = isupper((unsigned char)c);
            char base = isUpper ? 'A' : 'a';
            int shift = tolower((unsigned char)key[keyIdx % key.length()]) - 'a';
            plaintext += (char)((c - base - shift + 26) % 26 + base);
            keyIdx++;
        } else {
            plaintext += c;
        }
    }
    return plaintext;
}


void analyzeAndCrack(const string& ciphertext) {
    string normalized = normalize(ciphertext);
    if (normalized.empty()) {
        cout << "error: no alphabetic characters found!\n";
        return;
    }

    cout << "total letter count: " << normalized.length() << "\n\n";

    cout << "[1] KASISKI EXAMINATION:\n";
    cout << "finding repeated 3-5 character sequences and analyzing distance factors...\n";
    map<int, int> kasiskiFactors = kasiskiExamination(normalized);

    vector<pair<int, int>> sortedKasiski(kasiskiFactors.begin(), kasiskiFactors.end());
    sort(sortedKasiski.begin(), sortedKasiski.end(),
         [](const pair<int, int>& a, const pair<int, int>& b) {
             return a.second > b.second;
         });

    cout << "  " << left << setw(10) << "length" << setw(15) << "frequency" << "\n";
    cout << "  " << string(25, '-') << "\n";
    for (size_t i = 0; i < min((size_t)8, sortedKasiski.size()); ++i) {
        cout << "  " << left << setw(10) << sortedKasiski[i].first
             << setw(15) << sortedKasiski[i].second << "\n";
    }

    cout << "\n[2] INDEX OF COINCIDENCE (IoC):\n";
    cout << "  (english text IoC should be ~ 0.065, random text ~ 0.038)\n";
    cout << "  " << left << setw(10) << "length" << setw(15) << "avg IoC" << "\n";
    cout << "  " << string(25, '-') << "\n";

    map<int, double> iocMap;
    for (int len = 1; len <= 20; ++len) {
        vector<string> cols(len);
        for (size_t i = 0; i < normalized.length(); ++i)
            cols[i % len] += normalized[i];
        double avgIoC = 0;
        for (int i = 0; i < len; ++i)
            avgIoC += calculateIoC(cols[i]);
        avgIoC /= len;
        iocMap[len] = avgIoC;
        cout << "  " << left << setw(10) << len
             << setw(15) << fixed << setprecision(4) << avgIoC << "\n";
    }

    cout << "\n[3] RANKING KEY LENGTHS (KASISKI + IoC COMBINED):\n";
    cout << "  score = IoC * 100 + kasiski_count\n";

    map<int, double> combinedScores;
    for (auto& p : iocMap) {
        combinedScores[p.first] = p.second * 100.0;
    }
    for (auto& p : sortedKasiski) {
        combinedScores[p.first] += p.second;
    }

    vector<pair<double, int>> ranked;
    for (auto& p : combinedScores) {
        ranked.push_back({p.second, p.first});
    }
    sort(ranked.begin(), ranked.end(),
         [](const pair<double, int>& a, const pair<double, int>& b) {
             return a.first > b.first;
         });

    cout << "  " << left << setw(10) << "rank" << setw(10) << "length"
         << setw(15) << "score" << "\n";
    cout << "  " << string(35, '-') << "\n";
    for (size_t i = 0; i < min((size_t)5, ranked.size()); ++i) {
        cout << "  " << left << setw(10) << (i + 1)
             << setw(10) << ranked[i].second
             << setw(15) << fixed << setprecision(2) << ranked[i].first << "\n";
    }

    cout << "\n[4] TESTING TOP 5 KEY LENGTHS (CHI^2):\n";

    struct Candidate {
        string key;
        int keyLen;
        double totalChi;
        string plaintext;
    };
    vector<Candidate> candidates;

    int maxTries = min(5, (int)ranked.size());
    for (int t = 0; t < maxTries; ++t) {
        int len = ranked[t].second;
        string key = "";
        double totalChi = 0.0;

        for (int i = 0; i < len; ++i) {
            string col = "";
            for (size_t j = i; j < normalized.length(); j += len)
                col += normalized[j];

            auto result = findBestShiftForColumn(col);
            key += (char)(result.first + 'a');
            totalChi += result.second;
        }

        string pt = decrypt(ciphertext, key);
        candidates.push_back({key, len, totalChi, pt});

        cout << "  length " << setw(2) << len
             << " | key: " << setw(12) << left << ("'" + key + "'")
             << " | total Chi^2: " << fixed << setprecision(2) << totalChi
             << "\n";
    }

    sort(candidates.begin(), candidates.end(),
         [](const Candidate& a, const Candidate& b) {
             return a.totalChi < b.totalChi;
         });

    Candidate best = candidates[0];

    cout << "\n===========================================================\n";
    cout << "  RESULT\n";
    cout << "===========================================================\n";
    cout << "  key                 : " << best.key << "\n";
    cout << "  key length          : " << best.keyLen << "\n";
    cout << "  total Chi^2         : " << fixed << setprecision(2) << best.totalChi << "\n";
    cout << "-----------------------------------------------------------\n";
    cout << "  PLAINTEXT:\n\n";
    cout << best.plaintext << "\n";
    cout << "\n===========================================================\n";

    cout << "\n  [REF] top 3 candidates:\n";
    for (int i = 0; i < min(3, (int)candidates.size()); ++i) {
        cout << "    " << (i + 1) << ". key: '" << candidates[i].key
             << "' (length " << candidates[i].keyLen
             << ", Chi^2 = " << fixed << setprecision(2) << candidates[i].totalChi << ")\n";
    }
    cout << "===========================================================\n";
}

int main() {
    cout << "===========================================================\n";
    cout << "  VIGENERE CIPHER CRACKING\n";
    cout << "===========================================================\n\n";

    string filename;
    cout << "enter ciphertext filename (defalut is cipher.txt): ";
    getline(cin, filename);
    if (filename.empty()) filename = "cipher.txt";

    string ciphertext = "";
    ifstream file(filename);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) ciphertext += line + "\n";
        file.close();
        cout << "[SUCCESS] readed from file '" << filename << "'.\n\n";
    } else {
        cout << "file '" << filename << "' not found.\n";
        cout << "enter ciphertext directly (press Enter twice to finish):\n";
        string line;
        while (getline(cin, line)) {
            if (line.empty()) break;
            ciphertext += line + "\n";
        }
        if (ciphertext.empty()) {
            cerr << "[ERROR]: no data provided!\n";
            return 1;
        }
        cout << "\n";
    }

    cout << ">>> startingggggg <<<\n\n";
    analyzeAndCrack(ciphertext);

    return 0;
}
