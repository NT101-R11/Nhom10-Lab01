#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>

using namespace std;

string ceasar(const std::string &input, int key) {
  int shift = ((key % 26) + 26) % 26;

  string result;
  result.reserve(input.size());

  for (unsigned char c : input) {
    char shifted;
    if (c >= 'A' && c <= 'Z') {
      shifted = 'A' + ((c - 'A' + shift) % 26);
    } else if (c >= 'a' && c <= 'z') {
      shifted = 'a' + ((c - 'a' + shift) % 26);
    } else {
      throw runtime_error("invalid character in plaintext, aborting.");
    }
    result.push_back(shifted);
  }

  return result;
}

int main(int argc, char* argv[]) {
  bool encode = false;
  bool decode = false;
  bool has_key = false;
  int key = 0;
  string input;
  bool has_input = false;

  for (int i = 1; i < argc; i++) {
    string arg = argv[i];
    if (arg == "--encode") {
      encode = true;
    } else if (arg == "--decode") {
      decode = true;
    } else if (arg == "--key") {
      if (i + 1 >= argc) {
        throw runtime_error("--key need to be a valid integer");
      }
      key = atoi(argv[++i]);
      has_key = true;
    } else if (arg == "--text") {
      if (i + 1 >= argc) {
        throw runtime_error("--text cannot be empty, aborting.");
      }
      input = argv[++i];
      has_input = true;
    } else {
      string msg = "invalid arguement: " + arg + ", aborting.";
      throw runtime_error(msg);
    }
  }

  if (encode == decode) {
    throw runtime_error("you must choose either --encode or --decode, aborting");
  }
  if (!has_key) {
    throw runtime_error("--key required (must be a valid integer)");
  }

  if (!has_input) {
    ostringstream buffer;
    buffer << std::cin.rdbuf();
    input = buffer.str();

    if (!input.empty() && input.back() == '\n') {
      input.pop_back();
    }
  }

  int k = encode ? key : -key;
  cout << ceasar(input, k) << '\n';
  return 0;
}
