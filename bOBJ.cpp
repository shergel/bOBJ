#include "bOBJ.h"

using namespace std;

int main() {

	bool success{};

	/*OPEN FILE TO READ*/
	ifstream file_toRead;
	const string filename = "debugfile.obj"; //todo make parameter
	success = OpenFileToRead(filename, file_toRead);
	if (!success) { return 1; }

	/*GET FILE LENGTH*/
	int maxLines = count(istreambuf_iterator<char>(file_toRead), istreambuf_iterator<char>(), '\n');
	file_toRead.seekg(0, std::ios::beg); //return to start of file

	/*OPEN FILE TO WRITE*/
	ofstream file_toWrite("test.bOBJ", ios::binary);
	file_toWrite.close();
	ofstream file_toWrite_append("test.bOBJ", ios::binary | ios::app);
	

	/*CONVERT FILE*/
	ConvertAndWriteFile(file_toRead, maxLines, file_toWrite_append);

	/*WRITE*/
	
	//write indicator that we're looking at verts
	//dont forget to write the map as well!

	/*CLOSE FILE TO WRITE*/
	file_toWrite_append.close();
	/*END*/
	cout << "Exited Main";
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
void ConvertAndWriteFile(ifstream& objFile, const int maxLines, ofstream& bObjFile)
{
	bool done{};
	int operations{};

	const string vertexIndicator{ "v " };
	const string faceIndicator{ "f " };
	const string vertexNormalIndicator{ "vn " };
	const string commentIndicator{ "#" };

	string line{};
	string writeLine{"placeholder\n"};
	while (getline(objFile, line) && !done)	//Iterate over each line
	{
		++operations;

		//get converted line
		if (line.compare(0, vertexIndicator.length(), vertexIndicator) == 0) { writeLine = GetConvertedVertexLine(line); }
		else if (line.compare(0, faceIndicator.length(), faceIndicator) == 0) { HandleFace(); }
		else if (line.compare(0, vertexNormalIndicator.length(), vertexNormalIndicator) == 0) { HandleVertexNormal(); }
		else if (line.compare(0, commentIndicator.length(), commentIndicator) == 0) { HandleComment(); }
		else { cout << "handle empty line\n\n"; }

		//write to file
		bObjFile.write(writeLine.c_str(), writeLine.length());
		writeLine = "placeholder\n";

		//check if need to continue
		if (operations >= maxLines) { done = true; }
	}
}

/*VERTEX*/   ////////////////////////////////////////////////////////////////////////////
string GetConvertedVertexLine(const string& line)
{
	string result;

	/*EXTRACT VERTEX FROM LINE*/
	Vertex vertex;
	LineStringToVertex(line, vertex);

	/*COMPRESS VERTEX DATA*/
	ConvertVertex(vertex);

	result = vertex.x._string + vertex.y._string + vertex.z._string + '\n';
	return result;
}

/*store vertex / extract vertex from line*/
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

	if (stof(coordinateString) < 0) { result = true; }
	else { result = false; }

	return result;
}

/*convert vertex*/
void ConvertVertex(Vertex& vertex)
{
	ConvertCoordinate(vertex.x);
	ConvertCoordinate(vertex.y);
	ConvertCoordinate(vertex.z);
}
void ConvertCoordinate(Coordinate& coordinate)
{
	ConvertCoordinatePrefix(coordinate);
	ConvertCoordinatePostfix(coordinate);

	coordinate._string = coordinate._string_prefix + coordinate._string_postfix;
}

/*convert prefix*/
void ConvertCoordinatePrefix(Coordinate& coordinate)
{
	//BUFFER
	string convertedPrefix{ coordinate._string_prefix };

	//CALCULATE CONVERSION IN BUFFER
	convertedPrefix = GetWithoutZeroesAtEnd(convertedPrefix);
	convertedPrefix = GetWithoutFirstInstanceOf('.', convertedPrefix);
	if (coordinate.isNegative) { convertedPrefix = GetWithoutFirstInstanceOf('-', convertedPrefix); }

	//STORE CONVERTED BUFFER OVER ORIGINAL PREFIX
	coordinate._string_prefix = convertedPrefix;
}
string GetWithoutZeroesAtEnd(const string& input)
{
	string result = input;

	while (result.back() == '0') {
		result.pop_back();
	}

	return result;
}
string GetWithoutFirstInstanceOf(char toRemove, const string& input)
{
	string result = input;

	size_t pos = result.find(toRemove);
	if (pos != string::npos)
	{
		result.erase(pos, 1);
	}

	return result;
}

/*convert postfix*/
void ConvertCoordinatePostfix(Coordinate& coordinate)
{
	//BUFFER
	string convertedPostfix{ coordinate._string_postfix };

	//CALCULATE CONVERSION IN BUFFER
	convertedPostfix = GetPostfixAsLetter(convertedPostfix);
	convertedPostfix = GetLastCharCapitalized(convertedPostfix, !coordinate.isNegative); // negative numbers are no-cap, positive are cap

	//STORE CONVERTED BUFFER OVER ORIGINAL POSTFIX
	coordinate._string_postfix = convertedPostfix;
}
string GetPostfixAsLetter(const string& input)
{
	string result{};

	try { result = g_commonPostfixes.at(input); }
	catch (const out_of_range& e)
	{
		AddNewPostfixToMap(input);
		result = g_commonPostfixes.at(input);
	}

	return result;
}
void AddNewPostfixToMap(const string& newPostfixEntry)
{
	string result; //the new value that will be added as this postfixes' value in the map

	/*CALCULATE VALUE*/
	char startChar = 'A';
	char endChar = 'Z';

	int mapSize{ static_cast<int>(g_commonPostfixes.size()) };
	const int alphabetLength{ endChar - startChar };

	const int alphabetLoops = mapSize / alphabetLength;
	const int letterIndex = (mapSize % alphabetLength) - alphabetLoops;

	//Potential 'z'
	for (int i{ 0 }; i < alphabetLoops; ++i)
	{
		result += endChar;
	}
	//Any letter following a (series of) 'z'
	if (letterIndex >= 0)
	{
		char lastLetter = char(startChar + letterIndex);
		result += lastLetter;
	}

	/*NEW ENTRY IN MAP*/
	g_commonPostfixes.insert(make_pair(newPostfixEntry, result));
}
string GetLastCharCapitalized(const string& input, bool capitalized)
{
	string result{ input };
	bool shouldBeCap{ capitalized };
	int lastChar = int(input[input.length() - 1]);

	//is it capitalized now?
	bool isCap{};
	if (int(lastChar) < 'a') { isCap = true; }
	else { isCap = false; }

	//capitalize/decapitalize if needed
	if (isCap && !shouldBeCap) { lastChar += ('a' - 'A'); }
	else if (!isCap && shouldBeCap) { lastChar -= ('a' - 'A'); };

	//result
	result[result.length() - 1] = lastChar;
	return result;
}



/*FACE*/   ////////////////////////////////////////////////////////////////////////////
void HandleFace()
{
	cout << "handle face line\n\n";
}
void HandleComment()
{
	cout << "handle comment line\n\n";
}
void HandleVertexNormal()
{
	cout << "handle vertexnormal line\n\n";
}