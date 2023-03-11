#include "bOBJ.h"

using namespace std;

string GetWithoutZeroesAtEnd(const string& input);
string GetWithoutFirstInstanceOf(char toRemove, const string& input);

int main() {

	bool success{};

	/*OPEN FILE*/
	ifstream file_toRead;
	const string filename = "debugfile.obj"; //todo make parameter
	success = OpenFileToRead(filename, file_toRead);
	if (!success) { return 1; }

	/*GET FILE LENGTH*/
	int maxLines = count(istreambuf_iterator<char>(file_toRead), istreambuf_iterator<char>(), '\n');
	file_toRead.seekg(0, std::ios::beg); //return to start of file

	/*CONVERT FILE*/
	ConvertFile(file_toRead, maxLines);

	/*WRITE*/
//		//add to line
//		g_writeLine = g_writeLine + g_vertex.xInfo.prefix + g_vertex.xInfo.postfix;
//
//		////write to file
//		//if (g_writeFile.is_open()) { g_writeFile << g_writeLine; }
//		//g_writeFile.close();

	/*END*/
	cout << "Exited Main";
	cin.get();
	return 0;
}

bool OpenFileToRead(const string filename, ifstream& file)
{
	file.open(filename);

	bool success{ false };
	if (file) { success = true; }
	else { cerr << "Error opening file" << endl; }

	return success;
}

void ConvertFile(ifstream& objFile, const int maxLines)
{
	bool done{};
	int operations{};

	const string vertexIndicator{ "v " };
	const string faceIndicator{ "f " };

	string line{};
	while (getline(objFile, line) && !done)	//Iterate over each line
	{
		++operations;

		if (line.compare(0, vertexIndicator.length(), vertexIndicator) == 0) { HandleVertexLine(line); }
		else if (line.compare(0, faceIndicator.length(), faceIndicator) == 0) { HandleFace(); }
		else { cout << "\n handle comment line"; }

		if (operations >= maxLines) { done = true; }
	}
}

void HandleVertexLine(const string& line)
{
	/*EXTRACT VERTEX FROM LINE*/
	Vertex vertex;
	LineStringToVertex(line, vertex);

	/*COMPRESS VERTEX DATA*/
	ConvertVertex(vertex);

	//	// ADD CONVERTEDVERTEX TO FILE
}

void LineStringToVertex(const string& line, Vertex& vertex)
{
	/*READ LINE*/
	istringstream iss(line);
	vector<string> words;
	//"v ", x, y, and z get extracted from the line into a vector
	string word;
	while (iss >> word) { words.push_back(word); }

	/*FILL IN EACH COORDINATE OF VERTEX*/
	VertexStringToCoordinate(vertex.x, words[1]);
	VertexStringToCoordinate(vertex.y, words[2]);
	VertexStringToCoordinate(vertex.z, words[3]);
}

void VertexStringToCoordinate(Coordinate& coordinate, const string& fullString)
{
	coordinate._string = fullString; // _string

	coordinate._string_prefix = CalculateCoordinatePrefix(coordinate._string); // _string_prefix
	coordinate._string_postfix = CalculateCoordinatePostfix(coordinate._string); // _string_postfix
	coordinate.isNegative = CalculateCoordinateNegativity(coordinate._string); // _isNegative todo: make this  a  function
}

string CalculateCoordinatePrefix(const string& coordinateString)
{
	string prefix{};

	char delimiter{ 'e' };
	size_t pos = coordinateString.find(delimiter);
	if (pos != string::npos) { prefix = coordinateString.substr(0, pos); } //get prefix

	return prefix;
}
string CalculateCoordinatePostfix(const string& coordinateString)
{
	string postfix{};

	char delimiter{ 'e' };
	size_t pos = coordinateString.find(delimiter);
	if (pos != string::npos) { postfix = coordinateString.substr(pos); } //get postfix

	return postfix;
}
bool CalculateCoordinateNegativity(const string& coordinateString)
{
	bool result{};

	if (stof(coordinateString) < 0)	{result = true;	}
	else { result = false;	}

	return result;
}

void ConvertVertex( Vertex& vertex)
{
	ConvertCoordinate(vertex.x);
	ConvertCoordinate(vertex.y);
	ConvertCoordinate(vertex.z);
}
void ConvertCoordinate( Coordinate& coordinate)
{
	ConvertCoordinatePrefix(coordinate);
	ConvertCoordinatePostfix(coordinate);
}
void ConvertCoordinatePrefix( Coordinate& coordinate)
{
	//BUFFER
	string convertedPrefix{coordinate._string_prefix};

	//CALCULATE CONVERSION IN BUFFER
	convertedPrefix = GetWithoutZeroesAtEnd(convertedPrefix);
	convertedPrefix = GetWithoutFirstInstanceOf('.', convertedPrefix);
	if(coordinate.isNegative){ convertedPrefix = GetWithoutFirstInstanceOf('-', convertedPrefix); }
	
	//STORE CONVERTED BUFFER OVER ORIGINAL PREFIX
	coordinate._string_prefix = convertedPrefix;
}
void ConvertCoordinatePostfix( Coordinate& coordinate)
{
	//int IDX{};
	//string newPostfix;

	//// Check whether this postfix is already a key in the map
	//try
	//{
	//	newPostfix = g_map.at(g_vertex.xInfo.postfix);
	//}

	//catch (const out_of_range& e)
	//{
	//	g_map.insert(make_pair(g_vertex.xInfo.postfix, char(g_idx))); // add to list
	//	++g_idx;
	//	newPostfix = g_map.at(g_vertex.xInfo.postfix);
	//}

	//g_vertex.xInfo.postfix = newPostfix;
}

string GetWithoutZeroesAtEnd(const string& input)
{
	string result = input;

	while (result.back() == '0') {
		result.pop_back();
	}

	return result;
}
string GetWithoutFirstInstanceOf( char toRemove, const string& input)
{
	string result = input;

	size_t pos = result.find(toRemove);
	if (pos != string::npos)
	{
		result.erase(pos, 1);
	}

	return result;
}


void HandleFace()
{
	cout << "\n handle face line";

}