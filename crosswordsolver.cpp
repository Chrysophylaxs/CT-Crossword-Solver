#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

class unknownWord {
  public:
    string word;
    char direction;
    int xcoord;
    int ycoord;
};

vector< unknownWord > unknownWords;
vector< vector<string> > allFoundWords;
vector< vector< vector<char> > > table;
vector <char> global;
vector <vector <char> > solutions;

// Identifies words that need to be filled in from crossword
vector<unknownWord> findUnknownWords(vector< vector<char> > grid) {
  vector<unknownWord> words;
  unknownWord word;
  string unknown = "";

  for (int i = 0; i < grid.size(); i++) {
    for (int j = 0; j < grid.size(); j++) {
      if (grid.at(i).at(j) != '!') {
        unknown.push_back(grid.at(i).at(j));
      }
      if (unknown.size() == 2) {
        word.xcoord = i;
        word.ycoord = j - 1;
        word.direction = 'h';
      }
      if (grid.at(i).at(j) == '!' || j == grid.size() - 1) {
        if (unknown.size() >= 2) {
          word.word = unknown;
          words.push_back(word);
        }
        unknown = "";
      }
    }
  }

  for (int j = 0; j < grid.size(); j++) {
    for (int i = 0; i < grid.size(); i++) {
      if (grid.at(i).at(j) != '!') {
        unknown.push_back(grid.at(i).at(j));
      }
      if (unknown.size() == 2) {
        word.xcoord = i - 1;
        word.ycoord = j;
        word.direction = 'v';
      }
      if (grid.at(i).at(j) == '!' || i == grid.size() - 1) {
        if (unknown.size() >= 2) {
          word.word = unknown;
          words.push_back(word);
        }
        unknown = "";
      }
    }
  }

  return words;
}

// Compares an unknown word found from crossword with every word in the dictionary and stores all words that could complete this unknown
vector<string> possibleWords(string input) {
  ifstream inFS;
  string word;
  
  bool go;
  vector<string> strings;
   
  inFS.open("word_alpha.txt");
  if (!inFS.is_open()) {
    cout << "Could not open file word_alpha.txt." << endl;
  }

  while (!inFS.eof()) {
    inFS >> word;
    if (input.size() == word.size()) {
      go = true;
      for (int j = 0; j < word.size(); j++) {
        if (input.at(j) == '-') {
          continue;
        }
        else if (input.at(j) != word.at(j)) {
          go = false;
          break;
        } 
      }
      if (go) {
        strings.push_back(word);
      }
    }
  }

  inFS.close();
   
  return strings;
}

// Finds every word that could fit every unknown word found from the crossword using the dictionary
vector< vector<string> > allPossibleWords(vector<unknownWord> unknownWords) {
  vector< vector<string> > allFoundWords;
  
  for (int i = 0; i < unknownWords.size(); i++) {
    vector<string> foundWords = possibleWords(unknownWords.at(i).word);
    allFoundWords.push_back(foundWords);
  }

  return allFoundWords;
}

// Finds every word that could fit every unknown word found from the crossword using the previously found words 
vector< vector<string> > newWords(vector< unknownWord > unknownWords, vector< vector<string> > allFoundWords) {
  vector< vector<string> > newWords;
  bool go;
  for (int i = 0; i < unknownWords.size(); i++) {
    vector<string> strings;
    for (int j = 0; j < allFoundWords.at(i).size(); j++) {
      if (unknownWords.at(i).word.size() == allFoundWords.at(i).at(j).size()) {
        go = true;
        for (int k = 0; k < unknownWords.at(i).word.size(); k++) {
          if (unknownWords.at(i).word.at(k) == '-') {
            continue;
          }
          else if (unknownWords.at(i).word.at(k) != allFoundWords.at(i).at(j).at(k)) {
            go = false;
            break;
          } 
        }
      }
      if (go) {
        strings.push_back(allFoundWords.at(i).at(j));
      }
    }
    newWords.push_back(strings);
  }
  return newWords;
}

// Finds all unique characters at a given index in a string from all strings that complete an unknown word
vector<char> findPossibleChars(vector<string> words, int index) {
  bool go;
  vector <char> chars;
  for (int j = 0; j < words.size(); j++) {
    go = true;
    for (int k = 0; k < chars.size(); k++) {
      if (chars.at(k) == words.at(j).at(index)) {
        go = false;
      }
    }
    if (go) {
      chars.push_back(words.at(j).at(index));
    }
  }
  return chars;
}

// If there is a square which two words in the crossword go through (a crossing), this function finds combines the two lists of characters found for that square
vector<char> combine(vector<char> one, vector<char> two) {
  vector<char> chars;
  bool go;
  for (int i = 0; i < one.size(); i++) {
    for (int j = 0; j < two.size(); j++) {
      if (one.at(i) == two.at(j)) {
        go = true;
        for (int k = 0; k < chars.size(); k++) {
          if (chars.at(k) == one.at(i)) {
            go = false;
          }
        }
        if (go) {
          chars.push_back(one.at(i));
        }
      }
    }
  }
  /*if (chars.size() == 0) {
    for (int i = 0; i < one.size(); i++) {
      cout << one.at(i);
    }
    cout << endl;
    for (int i = 0; i < two.size(); i++) {
      cout << two.at(i);
    }
    cout << "no same chars in both arrays";
  }*/
  return chars;
}

// Fills a table with the size of the crossword with the possible characters for each square
vector< vector< vector<char> > > fillTable(vector <vector<char> > crossword, vector< unknownWord > unknownWords, vector< vector<string> > allFoundWords) {
  vector< vector< vector<char> > > table;
  for (int i = 0; i < crossword.size(); i++) {
    vector< vector<char> > row;
    for (int j = 0; j < crossword.size(); j++) {
      if (isalpha(crossword.at(i).at(j)) || crossword.at(i).at(j) == '!') {
        row.push_back({crossword.at(i).at(j)});
      }
      else if ((j - 1 == -1 || crossword.at(i).at(j - 1) == '!') && (j + 1 == crossword.size() || crossword.at(i).at(j + 1) == '!')) {
        for (int k = unknownWords.size() - 1; k >= 0; k--) {
          if (unknownWords.at(k).direction != 'v' || unknownWords.at(k).ycoord != j) {
            continue;
          }
          else if (unknownWords.at(k).xcoord <= i) {
            int index = i - unknownWords.at(k).xcoord;
            row.push_back(findPossibleChars(allFoundWords.at(k), index));
            break;
          }
        }
      }
      else if ((i - 1 == -1 || crossword.at(i - 1).at(j) == '!') && (i + 1 == crossword.size() || crossword.at(i + 1).at(j) == '!')) {
        for (int k = unknownWords.size() - 1; k >= 0; k--) {
          if (unknownWords.at(k).direction != 'h' || unknownWords.at(k).xcoord != i) {
            continue;
          }
          else if (j >= unknownWords.at(k).ycoord) {
            int index = j - unknownWords.at(k).ycoord;
            row.push_back(findPossibleChars(allFoundWords.at(k), index));
            break;
          }
        }
      }
      else {
        vector<char> vertical;
        vector<char> horizontal;

        for (int k = unknownWords.size() - 1; k >= 0; k--) {
          if (unknownWords.at(k).direction != 'v' || unknownWords.at(k).ycoord != j) {
            continue;
          }
          else if (unknownWords.at(k).xcoord <= i) {
            int index = i - unknownWords.at(k).xcoord;
            vertical = findPossibleChars(allFoundWords.at(k), index);
            break;
          }
        }
        for (int k = unknownWords.size() - 1; k >= 0; k--) {
          if (unknownWords.at(k).direction != 'h' || unknownWords.at(k).xcoord != i) {
            continue;
          }
          else if (unknownWords.at(k).ycoord <= j) {
            int index = j - unknownWords.at(k).ycoord;
            horizontal = findPossibleChars(allFoundWords.at(k), index);
            break;
          }
        }

        row.push_back(combine(vertical, horizontal));
      }
    }
    table.push_back(row);
  }
  return table;
}

// Converts the table back to a crossword with each square with one possible character filled in
vector< vector<char> > convert(vector< vector< vector<char> > > grid) {
  vector< vector<char> > output;
  for (int i = 0; i < grid.size(); i++) {
    vector<char> row;
    for (int j = 0; j < grid.at(i).size(); j++) {
      if (grid.at(i).at(j).size() == 1) {
        row.push_back(grid.at(i).at(j).at(0));
      }
      else {
        row.push_back('-');
      }
    }
    output.push_back(row);
  }
  return output;
}

// Outputs the crossword to the screen
void displayGrid(vector< vector<char> > grid) {
  for (int i = 0; i < grid.at(0).size() + 2; i++) {
    cout << "███";
  }
  cout << endl;
  for (int i = 0; i < grid.size(); i++) {
    cout << "███";
    for (int j = 0; j < grid.at(i).size(); j++) {
      if (grid.at(i).at(j) == '!') {
        cout << "███";
      } 
      else if (grid.at(i).at(j) != '-') {
        cout << " " << grid.at(i).at(j) << " ";
      }
      else {
        cout << "   ";
      }
    }
    cout << "███";
    cout << endl;
  }
  for (int i = 0; i < grid.at(0).size() + 2; i++) {
    cout << "███";
  }
  cout << endl;
}

// Takes in a crossword, outputs a table with all the possible characters in each spot
vector< vector< vector<char> > > completeCrossword (vector <vector<char> > crossword, int type) {
  vector< vector< vector<char> > > previous;

  do {
    unknownWords = findUnknownWords(crossword);
    if (type == 0) {
      allFoundWords = allPossibleWords(unknownWords);
    }
    else {
      allFoundWords = newWords(unknownWords, allFoundWords);
    }
    previous = table;
    table = fillTable(crossword, unknownWords, allFoundWords);
    crossword = convert(table);
    type++;
  } while (previous != table);

  return table;
}

vector <vector<int> > analyze(vector< vector< vector<char> > > table) {
  vector <vector<int> > list;
  for (int i = 0; i < table.size(); i++) {
    for (int j = 0; j < table.at(i).size(); j++) {
      if (table.at(i).at(j).size() == 0) return {{-1, -1}};
      if (table.at(i).at(j).size() > 1) list.push_back({i, j});
    }
  }
  return list;
}

vector< vector<char> > getchars(vector< vector< vector<char> > > table) {
  vector <vector<char> > cha;
  for (int i = 0; i < table.size(); i++) {
    for (int j = 0; j < table.at(i).size(); j++) {
      if (table.at(i).at(j).size() > 1) cha.push_back(table.at(i).at(j));
    }
  }
  return cha;
}

void Solutionchecker(vector< vector<char> > chars, int row) {
  if (row >= chars.size()) {
    solutions.push_back(global);
    return;
  }

  for (int i = 0; i < chars.at(row).size(); i++) {
    global.push_back(chars.at(row).at(i));
    Solutionchecker(chars, row + 1);
    global.pop_back();
  }
}

bool check(vector <vector<char> > crossword) {
  vector< unknownWord > unknownWordsTest = findUnknownWords(crossword);

  for (int i = 0; i < unknownWordsTest.size(); i++) {
    bool go = false;
    for (int j = 0; j < allFoundWords.at(i).size(); j++) {
      if (unknownWordsTest.at(i).word == allFoundWords.at(i).at(j)) {
        go = true;
      }
    }
    if (!go) {
      return false;
    }
  }
  return true;
}

// Calls all the functions
int main() {
  vector <vector<char> > crossword = {
    {'-', 'e', '-', 'l', '!', '-', '-', 'a', '-', 's', '-', 'e', '-'},

    {'a', '!', '-', '!', '!', '!', 'u', '!', 'e', '!', 'r', '!', 'e'},

    {'-', 'w', '-', 'g', '!', 's', '-', 'i', '-', 'r', '-', '-', '-'},

    {'c', '!', 's', '!', '-', '!', 'w', '!', 'm', '!', 'e', '!', 't'},

    {'-', '-', '-', 't', '-', 'r', '-', 'c', '-', 'l', '!', '!', '!'},

    {'e', '!', 'a', '!', 's', '!', 't', '!', 't', '!', 'a', '!', 'l'},

    {'-', 'u', '-', 'b', '-', 't', '!', 'd', '-', 'm', '-', 'l', '-'},

    {'s', '!', 'y', '!', 'l', '!', 'p', '!', 't', '!', '-', '!', 'i'},

    {'!', '!', '!', '-', '-', 'l', '-', 'm', '-', 't', '-', '-', '-'},

    {'a', '!', 'f', '!', 't', '!', 'r', '!', 's', '!', 's', '!', 'u'},

    {'-', 'r', '-', 'd', '-', '-', '-', 'l', '!', 'l', '-', 'a', '-'},

    {'e', '!', 'u', '!', 'o', '!', 'd', '!', '!', '!', 't', '!', 'e'},

    {'-', 'i', '-', 'e', '-', 'c', '-', 'r', '!', 'h', '-', 'r', '-'}
  };
  vector< vector< vector<char> > > table;
  int total = 1;

  cout << endl << "The crossword to be solved:" << endl;
  displayGrid(crossword);

  table = completeCrossword(crossword, 0);
  cout << endl << "The following table completes the crossword with every letter that is needed for every word to be in word_alpha.txt: " << endl;
  crossword = convert(table);
  displayGrid(crossword);

  vector <vector<int> > list = analyze(table);
  vector <vector<int> > compare = {{-1, -1}};

  if (list == compare) {
    cout << "The crossword does not have any solutions" << endl;
    return 0;
  }

  Solutionchecker(getchars(table), 0);

  cout << endl;

  for (int i = 0; i < solutions.size(); i++) {
    for (int j = 0; j < solutions.at(i).size(); j++) {
      crossword.at(list.at(j).at(0)).at(list.at(j).at(1)) = solutions.at(i).at(j);
    }
    if(check(crossword)) {
      cout << "Solution Number: " << total << endl;
      displayGrid(crossword);
      cout << endl;
      total++;
    }
  }

  cout << "Total number of solutions found: " << --total;

  return 0;
}