#include "bOBJ.h"

using namespace std;

int main() {
	OBJ_to_BOBJ();
	BOBJ_to_OBJ();
	return 0;
}

void OBJ_to_BOBJ()
{
	/*START*/
	cout << "-> Start conversion to bOBJ...\n";

	/*OPEN FILE TO READ*/
	bool success{};
	ifstream file_toRead;
	const string filename = "debugfile.obj"; //todo make parameter
	success = OpenFileToRead(filename, file_toRead);
	if (!success) { return; }

	/*GET FILE LENGTH*/
	int maxLines = count(istreambuf_iterator<char>(file_toRead), istreambuf_iterator<char>(), '\n');
	file_toRead.seekg(0, std::ios::beg); //return to start of file

	/*OPEN FILE TO WRITE*/
	ofstream file_toWrite("test.bOBJ", ios::binary);
	file_toWrite.close();
	ofstream file_toWrite_append("test.bOBJ", ios::binary | ios::app);

	/*CONVERT AND WRITE LINES*/
	ConvertAndWriteToBOBJ(file_toRead, maxLines, file_toWrite_append);

	/*WRITE MAP LEGENDA*/
	WriteMap(file_toWrite_append);

	/*CLOSE FILE TO WRITE*/
	file_toWrite_append.close();

	/*END*/
	cout << "-> File successfully converted & saved as bOBJ\n";
	cin.get();
}

bool OpenFileToRead(const string filename, ifstream& file)
{
	file.open(filename);

	bool success{ false };
	if (file) { success = true; }
	else { cerr << "Error opening file" << endl; }

	return success;
}

// BOBJ TO OBJ ///////////////////////////////////////////////////////////////////////////////////////////////
void BOBJ_to_OBJ()
{
	// get map
}



// OBJ TO BOBJ ///////////////////////////////////////////////////////////////////////////////////////////////
void ConvertAndWriteToBOBJ(ifstream& objFile, const int maxLines, ofstream& bObjFile)
{
	bool done{};
	int operations{};

	const string vertexIndicator{ "v" };
	const string faceIndicator{ "f" };
	const string vertexNormalIndicator{ "vn" };
	const string commentIndicator{ "#" };
	const string emptyIndicator{ " " };

	bool commentsFound{ false }, verticesFound{ false }, vertexNormalsFound{ false }, facesFound{ false };

	string line{};
	string introductionline{}; // indicates which type of data this is (vertex/face/...)
	string writeLine{ "placeholder\n" };
	while (getline(objFile, line) && !done)	//Iterate over each line
	{
		++operations;

		//get converted line we must check vn/vp before v, since their indicators might overlap

		if (line.compare(0, vertexNormalIndicator.length(), vertexNormalIndicator) == 0) // in case of vertex normal
		{
			if (!vertexNormalsFound) { introductionline = vertexNormalIndicator + '\n'; vertexNormalsFound = true; }
			writeLine = introductionline + GetConvertedVertexNormalLine(line);
		}

		else if (line.compare(0, vertexIndicator.length(), vertexIndicator) == 0) // in case of vertex
		{
			if (!verticesFound) { introductionline = vertexIndicator + '\n'; verticesFound = true; }
			writeLine = introductionline + GetConvertedVertexLine(line);
		}

		else if (line.compare(0, faceIndicator.length(), faceIndicator) == 0) // in case of face
		{
			if (!facesFound) { introductionline = faceIndicator + '\n'; facesFound = true; }
			writeLine = introductionline + GetConvertedFaceLine(line);
		}

		else if (line.compare(0, commentIndicator.length(), commentIndicator) == 0) // in case of comment
		{
			if (!commentsFound) { introductionline = commentIndicator + '\n'; commentsFound = true; }
			writeLine = introductionline + GetConvertedCommentLine(line);
		}
		else if (line.compare(0, emptyIndicator.length(), emptyIndicator) == 0) // in case of comment
		{
			writeLine = "";
		}
		else //copy anything else without conversion
		{
			writeLine = line;
		}

		//write to file
		bObjFile.write(writeLine.c_str(), writeLine.length());
		writeLine = "placeholder\n";
		introductionline = "";

		//check if need to continue
		if (operations >= maxLines) { done = true; }
	}
}

/*VERTEX*/   ////////////////////////////////////////////////////////////
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
	else { prefix = coordinateString; }

	return prefix;
}
string CalculateCoordinatePostfix(const string& coordinateString)
{
	string postfix{};

	char delimiter{ 'e' };
	size_t pos = coordinateString.find(delimiter);
	if (pos != string::npos) { postfix = coordinateString.substr(pos); } //get postfix
	else { postfix = g_noPostfixIndicator; }

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
	convertedPrefix = GetWithoutCharAtEnd(convertedPrefix, '0');
	convertedPrefix = GetWithoutFirstInstanceOf('.', convertedPrefix);
	if (coordinate.isNegative) { convertedPrefix = GetWithoutFirstInstanceOf('-', convertedPrefix); }

	//STORE CONVERTED BUFFER OVER ORIGINAL PREFIX
	coordinate._string_prefix = convertedPrefix;
}
string GetWithoutCharAtEnd(const string& input, char toDelete)
{
	string result = input;

	while (result.back() == toDelete) {
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

	try { result = g_prefixes_OBJtoBOBJ.at(input); }
	catch (const out_of_range& e)
	{
		AddNewPostfixToMap(input);
		result = g_prefixes_OBJtoBOBJ.at(input);
	}

	return result;
}
void AddNewPostfixToMap(const string& newPostfixEntry)
{
	string result; //the new value that will be added as this postfixes' value in the map

	/*CALCULATE VALUE*/
	char startChar = 'A';
	char endChar = 'Z';

	int mapSize{ static_cast<int>(g_prefixes_OBJtoBOBJ.size()) };
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
	g_prefixes_OBJtoBOBJ.insert(make_pair(newPostfixEntry, result));
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

/*FACE*/   ////////////////////////////////////////////////////////////
string GetConvertedFaceLine(const string& line)
{
	string result{ line };

	/*CLEAN*/
	result = result.substr(2) + "\n";

	return result;
}
/*COMMENT*/   ////////////////////////////////////////////////////////////
string GetConvertedCommentLine(const string& line)
{
	string result{ line };

	/*CLEAN*/
	result = result.substr(2) + "\n";

	return result;
}
/*VERTEX NORMAL*/   ////////////////////////////////////////////////////////////
string GetConvertedVertexNormalLine(const string& line)
{
	string result{ line };

	/*REMOVE ACRONYM*/
	result = result.substr(2);

	/*SPLIT UP INTO ELEMENTS*/
	istringstream iss(result);
	vector<string> vertexNormal;
	string coordinate;
	while (iss >> coordinate) { vertexNormal.push_back(coordinate); }

	//CLEAN EACH OF 0s AT END
	// IF POSITIVE, REMOVE '.'
	//ADD TO END RESULT
	result = {};
	for (string coordinate : vertexNormal)
	{
		//remove 0s at end
		coordinate = GetWithoutCharAtEnd(coordinate, '0');

		//remove '-' for negative numbers, remove '.' for positive numbers
		char toRemove{};
		if (coordinate[0] == '-') { toRemove = '-'; }
		else { toRemove = '.'; }
		coordinate = GetWithoutFirstInstanceOf(toRemove, coordinate);

		result += coordinate + " ";
	}

	result += '\n';

	return result;
}
void WriteMap(ofstream& bObjFile)
{
	string line{};

	string mapIndicator{ "map" };
	line += mapIndicator + '\n';

	for (auto it = g_prefixes_OBJtoBOBJ.begin(); it != g_prefixes_OBJtoBOBJ.end(); ++it) {
		
		line += it->second + " " + it->first + '\n';
	}

	bObjFile.write(line.c_str(), line.length());
}