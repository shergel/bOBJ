//#include <type_traits>
//#include <string>
//#include <iostream>
//#include <fstream>
//#include <cassert>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

ifstream g_readFile;
ofstream g_writeFile("test.mar");

string g_readLine{};
vector<string> g_readWords{};
struct WordInfo
{
	string prefix, postfix;
	bool negative{};
};
WordInfo g_readWordInfo;

std::map<std::string, char> g_map;
int g_idx{ int('A') }; //make sure this cannot go over a certain nr AND make sure it can be nocap when negative


string g_writeLine{};


bool OpenFileToRead(string filename)
{
	//OPEN FILE
	g_readFile.open(filename);

	//check for success
	bool success{ false };
	if (g_readFile) { success = true; }
	else { cerr << "Error opening file" << endl; }

	//return
	return success;
}

//todo BELOW : when we run out of bounds with the lines, it crashes, ALSO, when v is exceeded, it doesn't know. use the vertex count?
bool GoToNextLineStartingWith(string toFind)
{
	bool success{ false };

	while (!success && getline(g_readFile, g_readLine))
	{
		//Check if start of line == toFind
		if (g_readLine.compare(0, toFind.length(), toFind) == 0) { success = true; }
	}

	return success;
}
void StoreEachWordSeparatelyInVector()
{
	istringstream iss(g_readLine);
	string word;

	// EXTRACT "WORDS" FROM THE ISS USING DEFAULT DELIMITER ' '
	while (iss >> word) {
		g_readWords.push_back(word);
	}
}
WordInfo SplitWordsInPrefixPostfix(string word, char delimiter)
{
	WordInfo local{};

	size_t pos = word.find(delimiter);

	if (pos != string::npos)
	{
		local.prefix = word.substr(0, pos);
		local.postfix = word.substr(pos);
	}

	return local;
}
void RemoveZeroesAtEndOfPrefix()
{
	while (g_readWordInfo.prefix.back() == '0') {
		g_readWordInfo.prefix.pop_back();
	}
}
void RemoveFromWord(char toRemove)
{
	size_t pos = g_readWordInfo.prefix.find(toRemove);
	if (pos != string::npos)
	{
		g_readWordInfo.prefix.erase(pos, 1);
	}
}
void HandleNegativeSign()
{
	if (stof(g_readWordInfo.prefix) < 0)
	{
		g_readWordInfo.prefix = g_readWordInfo.prefix.substr(1); //remove first character (the minus)
		g_readWordInfo.negative = true;
	}
}
void HandlePostfix()
{
	int IDX{};
	string newPostfix;

	// Check whether this postfix is already a key in the map
	try
	{
		newPostfix = g_map.at(g_readWordInfo.postfix);
	}

	catch (const out_of_range& e)
	{
		g_map.insert(make_pair(g_readWordInfo.postfix, char(g_idx))); // add to list
		++g_idx;
		newPostfix = g_map.at(g_readWordInfo.postfix);
	}

	g_readWordInfo.postfix = newPostfix;
}

void CleanupVertices()
{
	GoToNextLineStartingWith("v");

	StoreEachWordSeparatelyInVector();

	g_readWordInfo = SplitWordsInPrefixPostfix(g_readWords[1], 'e');


	//PREFIX
	RemoveZeroesAtEndOfPrefix();
	RemoveFromWord('.');

	//BOTH
	HandleNegativeSign();

	//POSTFIX
	HandlePostfix();

	//createline
	g_writeLine = g_writeLine + g_readWordInfo.prefix + g_readWordInfo.postfix;

	//write to file
	if (g_writeFile.is_open()) {g_writeFile << g_writeLine;}
	g_writeFile.close();

	cout << "successfully converted vertices in file ...";
	std::cin.get();
}

int main() {

	/*OPEN FILE*/
	string filename = "debugfile.obj";
	if (!OpenFileToRead(filename)) { return 1; }

	CleanupVertices();

	return 0;
}