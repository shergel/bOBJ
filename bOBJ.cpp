using namespace std;

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

//INDICATORS
const string g_vertexIndicator{ "v" };
const string g_faceIndicator{ "f" };
const string g_vertexNormalIndicator{ "vn" };
const string g_commentIndicator{ "#" };
const string g_mapIndicator{ "map" };

struct Coordinate
{
	string _string = {};
	string _string_prefix = {};
	string _string_postfix = {};
	bool isNegative = {};
};
struct Vertex
{
	Coordinate x{}, y{}, z{};
};

void OBJ_to_BOBJ();
void BOBJ_to_OBJ();


bool OpenFileToRead(const string filename, ifstream& file);
string GetConvertedOBJtoBOBJString(ifstream& objFile, const int maxLines);
string GetConvertedVertexLine(const string& line);
string GetConvertedFaceLine(const string& line);
string GetConvertedVertexNormalLine(const string& line);
string GetConvertedCommentLine(const string& line);
void LineStringToVertex(const string& line, Vertex& vertex);
void VertexStringToCoordinate(Coordinate& coordinate, const string& fullString);
string CalculateCoordinatePrefix(const string& coordinateString);
string CalculateCoordinatePostfix(const string& coordinateString);
bool CalculateCoordinateNegativity(const string& coordinateString);
void ConvertVertex(Vertex& vertex);
void ConvertCoordinate(Coordinate& coordinate);
void ConvertCoordinatePrefix(Coordinate& coordinate);
void ConvertCoordinatePostfix(Coordinate& coordinate);
string GetWithoutCharAtEnd(const string& input, char toDelete);
string GetWithoutFirstInstanceOf(char toRemove, const string& input);
string GetPostfixAsLetter(const string& input);
void AddNewPostfixToMap(const string& newPostfixEntry);
string GetLastCharCapitalized(const string& input, bool capitalized);
void WriteMap(ofstream& bObjFile);


const string g_noPostfixIndicator{ "NONE" };
map<string, string> g_postfixes_OBJtoBOBJ//map that stores all common vertex postfixes
{
{ g_noPostfixIndicator, "A"}
};
map<string, string> g_postfixes_BOBJtoOBJ{};


////////////////////////////////////////////
//todo make these parameters
string g_filenameOBJ{ "low poly stanford bunny.obj" };
string g_filenameBOBJ{ "test.bOBJ" };
string g_filenameOBJ2{ "test.obj" };
///////////////////////////////////////////

enum Linetype
{
	OTHER = 0,
	MAP,
	VERTEX,
	VERTEXNORMAL,
	FACE,
	COMMENT,
};

Linetype GetLineType(const string& line);
string GetBOBJtoOBJLine(const string& line, Linetype thistype, Linetype conversiontype);
void HandleMapLine(const string& line, Linetype thistype, Linetype conversiontype);

string GetDecompressedVertex(const string& line);
string GetDecompressedVertexNormal(const string& line);
string GetDecompressedFace(const string& line);
string GetDecompressedComment(const string& line);
string GetBOBJVertexCoordinateToOBJVertexCoordinate(const string& coordinate);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// header/cpp split

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
	const string filename = g_filenameOBJ; //todo make parameter
	success = OpenFileToRead(filename, file_toRead);
	if (!success) { return; }

	/*GET FILE LENGTH*/
	int maxLines = static_cast<int>(count(istreambuf_iterator<char>(file_toRead), istreambuf_iterator<char>(), '\n'));
	file_toRead.seekg(0, std::ios::beg); //return to start of file

	/*OPEN FILE TO WRITE*/
	ofstream file_toWrite(g_filenameBOBJ, ios::binary);
	file_toWrite.close();
	ofstream file_toWrite_append(g_filenameBOBJ, ios::binary | ios::app);

	/*CONVERT LINES*/
	string conversion = GetConvertedOBJtoBOBJString(file_toRead, maxLines);

	/*WRITE MAP LEGENDA*/
	WriteMap(file_toWrite_append);

	/*WRITE CONVERSION TO FILE*/
	file_toWrite_append.write(conversion.c_str(), conversion.length());

	/*CLOSE FILE TO WRITE*/
	file_toWrite_append.close();
	file_toRead.close();

	/*END*/
	cout << "-> File successfully converted & saved as bOBJ\n";
}
void BOBJ_to_OBJ()
{
	cout << "\n->-> Start conversion to OBJ...\n";

	/*OPEN FILE TO READ*/
	bool success{};
	ifstream file_toRead;
	const string filename = g_filenameBOBJ; //todo make parameter
	success = OpenFileToRead(filename, file_toRead);
	if (!success) { return; }

	/*GET FILE LENGTH*/
	int maxLines = static_cast<int>(count(istreambuf_iterator<char>(file_toRead), istreambuf_iterator<char>(), '\n'));
	file_toRead.seekg(0, std::ios::beg); //return to start of file

	// enum checking which type of conversion we need, DATA is its default state
	Linetype thistype = Linetype::OTHER;
	Linetype lasttype = Linetype::OTHER;
	Linetype conversiontype = Linetype::OTHER;

	/*ITERATE OVER FILE*/
	bool done{};
	int operations{};
	string line;
	string conversion{};

	while (getline(file_toRead, line) && !done)	//Iterate over each line
	{
		++operations;

		thistype = GetLineType(line);
		if (lasttype == Linetype::OTHER && thistype != Linetype::OTHER) { conversiontype = thistype; } // new section detected

		if (conversiontype == Linetype::MAP) { HandleMapLine(line, thistype, conversiontype); }
		else { conversion += GetBOBJtoOBJLine(line, thistype, conversiontype); }

		lasttype = thistype;
		if (operations >= maxLines) { done = true; }
	}

	/*OPEN FILE TO WRITE*/
	ofstream file_toWrite;
	file_toWrite.open(g_filenameOBJ2);
	file_toWrite.write(conversion.c_str(), conversion.length());
	file_toWrite.close();

	cout << "->-> File successfully converted to OBJ.\n";
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

#pragma region BOBJ to OBJ
// BOBJ TO OBJ ///////////////////////////////////////////////////////////////////////////////////////////////
Linetype GetLineType(const string& line)
{
	Linetype linetype{ Linetype::OTHER };

	if (line.compare(0, g_mapIndicator.length(), g_mapIndicator) == 0) { linetype = Linetype::MAP; }
	else if (line.compare(0, g_vertexNormalIndicator.length(), g_vertexNormalIndicator) == 0) { linetype = Linetype::VERTEXNORMAL; }
	else if (line.compare(0, g_vertexIndicator.length(), g_vertexIndicator) == 0) { linetype = Linetype::VERTEX; }
	else if (line.compare(0, g_faceIndicator.length(), g_faceIndicator) == 0) { linetype = Linetype::FACE; }
	else if (line.compare(0, g_commentIndicator.length(), g_commentIndicator) == 0) { linetype = Linetype::COMMENT; }

	return linetype;
}
string GetBOBJtoOBJLine(const string& line, Linetype thistype, Linetype conversiontype)
{
	string result{};

	if (thistype != Linetype::OTHER) { return result; }
	else if (conversiontype == Linetype::OTHER) { return result; }

	switch (conversiontype)
	{
	case Linetype::VERTEX:
		result = GetDecompressedVertex(line);
		break;
	case Linetype::VERTEXNORMAL:
		result = GetDecompressedVertexNormal(line);
		break;
	case Linetype::FACE:
		result = GetDecompressedFace(line);
		break;
	case Linetype::COMMENT:
		result = GetDecompressedComment(line);
		break;
	}

	return result;
}

void HandleMapLine(const string& line, Linetype thistype, Linetype conversiontype)
{
	if (thistype != Linetype::OTHER) { return; }
	else if (conversiontype != Linetype::MAP) { return; }

	/*SEPARATE KEY & VALUE FROM DATA*/
	istringstream iss(line);
	vector<string> words;
	string word;
	while (iss >> word) { words.push_back(word); }

	/*STORE KEY*/
	string key = words[0];
	key = key.substr(1);

	/*STORE VALUE*/
	string value = words[1];
	if (key == "A") { value = ""; } // the key is A, it indicates there was no postfix to begin with

	/*STORE IN MAP*/
	g_postfixes_BOBJtoOBJ.insert(std::make_pair(key, value));
}
string GetDecompressedVertex(const string& line)
{
	string result{ line };

	/*PLACE SPACES WHERE NEEDED*/
	bool isLetter{ false }, wasLetter{ false };
	string accumulated{};
	string lineWithSpaces{};
	for (int i{ 0 }; i < result.size(); ++i) {

		/*check for letter vs number*/
		if (isalpha(result[i])) { isLetter = true; }
		else { isLetter = false; }

		if (wasLetter && !isLetter) { lineWithSpaces += " "; };
		lineWithSpaces += result[i];

		wasLetter = isLetter;
	}

	/*SPLIT COORDINATES INTO VECTOR*/
	istringstream iss(lineWithSpaces);
	vector<string> words;
	string word;
	while (iss >> word) { words.push_back(word); }

	/*CONVERT EACH COORDINATE TO OBJ STYLE*/
	for (string& word : words)
	{
		word = GetBOBJVertexCoordinateToOBJVertexCoordinate(word);
	}

	result = g_vertexIndicator + " " + words[0] + " " + words[1] + " " + words[2] + '\n';

	return result;
}
string GetDecompressedVertexNormal(const string& line)
{
	string result{};

	/*SPLIT LINE INTO VECTOR*/
	istringstream iss(line);
	vector<string> words;
	string word;
	while (iss >> word) { words.push_back(word); }

	bool isNegative{};
	const int desiredCharCount{ 8 }; // in the OBJ, vn values were always 8 characters long
	for (string& word : words)
	{
		// check if dot, if so -> must become negative
		if (word[1] == '.') { isNegative = true; }
		else { isNegative = false; }

		// add dot to numbers lacking it
		if (!isNegative) { word.insert(1, "."); }

		// elongate word to the desired length (by adding 0s)
		while (word.length() < desiredCharCount)
		{
			word += "0";
		}

		// add '-' to negative numbers
		if (isNegative) { word.insert(0, "-"); }
	}

	/*RETURN NEW LINE*/
	result += g_vertexNormalIndicator + " " + words[0] + " " + words[1] + " " + words[2] + '\n';
	return result;
}
string GetDecompressedFace(const string& line)
{
	string result{};

	result += g_faceIndicator + " " + line + '\n';

	return result;
}
string GetDecompressedComment(const string& line)
{
	string result{line};

	string conversion = GetWithoutFirstInstanceOf('*', result);
	result = g_commentIndicator + " " + conversion + '\n';

	return result;
}
string GetBOBJVertexCoordinateToOBJVertexCoordinate(const string& coordinate)
{
	string result{ coordinate };

	/*SPLIT into prefix & postfix*/
	string prefix{}, postfix{};
	for (char c : result)
	{
		if (isdigit(c)) { prefix += c; }
		else if (isalpha(c)) { postfix += c; }
	}

	/*CONVERT*/
	//check if it has to be negative
	bool isNegative{};
	if (isupper(postfix.back())) { isNegative = false; }
	else { isNegative = true; }

	//add '.'
	prefix.insert(1, ".");

	//lengthen prefix to desired length
	const int desiredLength{ 9 };
	while (prefix.length() != desiredLength)
	{
		prefix += '0';
	}

	//apply changes which come with negativity
	if (isNegative)
	{
		prefix.insert(0, "-");

		/*replace last character of the postfix with its capitalized version*/
		char lastLetter = postfix.back();
		postfix.pop_back();
		postfix += toupper(lastLetter);
	}

	//convert postfix
	postfix = g_postfixes_BOBJtoOBJ[postfix];

	result = prefix + postfix;
	return result;
}
#pragma endregion


#pragma region OBJ to BOBJ
// OBJ TO BOBJ ///////////////////////////////////////////////////////////////////////////////////////////////
string GetConvertedOBJtoBOBJString(ifstream& objFile, const int maxLines)
{
	string result;
	bool done{};
	int operations{};

	bool commentsFound{ false }, verticesFound{ false }, vertexNormalsFound{ false }, facesFound{ false };

	string line{};
	string introductionline{}; // indicates which type of data this is (vertex/face/...)
	string writeLine{ "placeholder\n" };
	while (getline(objFile, line) && !done)	//Iterate over each line
	{
		++operations;

		//get converted line we must check vn/vp before v, since their indicators might overlap
		if (line.compare(0, g_vertexNormalIndicator.length(), g_vertexNormalIndicator) == 0) { // in case of vertex normal
			if (!vertexNormalsFound) { introductionline = g_vertexNormalIndicator + '\n'; vertexNormalsFound = true; }
			writeLine = introductionline + GetConvertedVertexNormalLine(line);
		}

		else if (line.compare(0, g_vertexIndicator.length(), g_vertexIndicator) == 0) { // in case of vertex
			if (!verticesFound) { introductionline = g_vertexIndicator + '\n'; verticesFound = true; }
			writeLine = introductionline + GetConvertedVertexLine(line);
		}

		else if (line.compare(0, g_faceIndicator.length(), g_faceIndicator) == 0) { // in case of face
			if (!facesFound) { introductionline = g_faceIndicator + '\n'; facesFound = true; }
			writeLine = introductionline + GetConvertedFaceLine(line);
		}

		else if (line.compare(0, g_commentIndicator.length(), g_commentIndicator) == 0) { // in case of comment
			if (!commentsFound) { introductionline = g_commentIndicator + '\n'; commentsFound = true; }
			writeLine = introductionline + GetConvertedCommentLine(line);
		}
		else { writeLine = ""; }//copy anything else without conversion

		//write to file
		result += writeLine;
		writeLine = "placeholder\n";
		introductionline = "";

		//check if need to continue
		if (operations >= maxLines) { done = true; }
	}

	return result;
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

	try { result = g_postfixes_OBJtoBOBJ.at(input); }
	catch (const out_of_range&)
	{
		AddNewPostfixToMap(input);
		result = g_postfixes_OBJtoBOBJ.at(input);
	}

	return result;
}
void AddNewPostfixToMap(const string& newPostfixEntry)
{
	string result; //the new value that will be added as this postfixes' value in the map

	/*CALCULATE VALUE*/
	char startChar = 'A';
	char endChar = 'Z';

	int mapSize{ static_cast<int>(g_postfixes_OBJtoBOBJ.size()) };
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
	g_postfixes_OBJtoBOBJ.insert(make_pair(newPostfixEntry, result));
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
	result = "*" + result.substr(2) + "\n";

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

	line += g_mapIndicator + '\n';

	for (auto it = g_postfixes_OBJtoBOBJ.begin(); it != g_postfixes_OBJtoBOBJ.end(); ++it) {

		line += "*" + it->second + " " + it->first + '\n';
	}

	//line += g_mapIndicator + +"end" + '\n';

	bObjFile.write(line.c_str(), line.length());
}
#pragma endregion

