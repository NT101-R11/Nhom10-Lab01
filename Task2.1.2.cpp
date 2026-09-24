#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

string shiftKeepOthers(const string &input, int shift) {
  int s = ((shift % 26) + 26) % 26;
  string result;
  result.reserve(input.size());

  for (unsigned char c : input) {
    if (c >= 'A' && c <= 'Z') {
      result.push_back('A' + ((c - 'A' + s) % 26));
    } else if (c >= 'a' && c <= 'z') {
      result.push_back('a' + ((c - 'a' + s) % 26));
    } else {
      result.push_back(c);
    }
  }
  return result;
}

string readFromFile(const string &path) {
  ifstream file(path);
  if (!file) {
    throw runtime_error("cannot open: " + path);
  }
  ostringstream buffer;
  buffer << file.rdbuf();
  string content = buffer.str();
  while (!content.empty() && (content.back() == '\n' || content.back() == '\r')) {
    content.pop_back();
  }
  return content;
}

string readFromKeyboard() {
  cout << "===[caesar cipher bruteforce tool]===\n";
  cout << "enter encrypted content:\n> ";
  string content;
  getline(cin, content);
  return content;
}

int main(int argc, char *argv[]) {
  string cipher;

  if (argc > 1) {
    cipher = readFromFile(argv[1]);
    cout << "readed from: " << argv[1] << "\n\n";
  } else {
    cipher = readFromKeyboard();
    cout << "\n";
  }

  for (int key = 1; key <= 25; key++) {
    string candidate = shiftKeepOthers(cipher, -key);

    cout << "\033[2J\033[1;1H";
    cout << "===[caesar cipher bruteforce tool]===\n\n\n";
    cout << "Key " << key << ": \n\n" << candidate << "\n";
    cout << "\n\ndoes this look meaningful to you? (y/N): ";

    string answer;
    if (!getline(cin, answer)) {
      break;
    }

    if (!answer.empty() && (answer[0] == 'y' || answer[0] == 'Y')) {
      cout << "\033[2J\033[1;1H";
      cout << ">>> found key: " << key << "\n";
      cout << ">>> your decrypted content:\n " << candidate << "\n";
      return 0;
    }
  }

  cout << "\nthere was no decrypted content that seem meaningful to you, aborting...\n";
  return 1;
}
