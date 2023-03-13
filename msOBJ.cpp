#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <map>
#include "msOBJ.h"

#ifdef _WIN32
#define RED_TEXT        "\033[91m"
#define GREEN_TEXT      "\033[92m"
#define BLUE_TEXT       "\033[94m"
#define RESET_TEXT      "\033[0m"
#else
#define RED_TEXT        "\x1B[91m"
#define GREEN_TEXT      "\x1B[92m"
#define BLUE_TEXT       "\x1B[94m"
#define RESET_TEXT      "\x1B[0m"
#endif

using namespace std;

int main() {

	cout << BLUE_TEXT << "[ - - - - - - - OBJ <-> MSOBJ conversion tool - - - - - - - ] start\n" << RESET_TEXT;

	ConversionType conversiontype = ConversionType::NONE;

	/*ACCESS FILE TO CONVERT*/
	bool success{};
	string filepath{};
	ifstream file_toRead;
	while (!success)
	{
		cout << "\nPlease enter filepath to an .obj or .msobj : ";
		cin >> filepath;

		success = AccessFilePath(file_toRead, filepath, conversiontype);
	}

	///*CREATE OUTPUT FILENAME*/
	size_t pos = filepath.find('.');
	string outputFilepath = filepath.substr(0, pos);

	/*CONVERT*/
	switch (conversiontype)
	{
	case ConversionType::OBJtoMSOBJ:
		cout << "Converting .obj to .msobj ...\n";
		outputFilepath += ".msobj";
		OBJ_to_MSOBJ(file_toRead, filepath, outputFilepath);
		break;
	case ConversionType::MSOBJtoOBJ:
		cout << "Converting .msobj to .obj ...\n";
		outputFilepath += ".obj";
		MSOBJ_to_OBJ(file_toRead, filepath, outputFilepath);
		break;
	}

	cout
		<< GREEN_TEXT << "\n * * * * * [ SUCCESS ] * * * * * \n" << RESET_TEXT
		<< "-> File successfully converted & saved at : " << GREEN_TEXT << outputFilepath << '\n' << RESET_TEXT
		<< "press any key to exit.\n"
		<< BLUE_TEXT << "[ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ] end\n" << RESET_TEXT;

	cin.get();
	cin.get();

	return 0;
}

bool AccessFilePath(ifstream& file, const string& filepath, ConversionType& conversiontype)
{
	bool success{};

	// Check if file opens
	file.close();
	bool fileFound = OpenFileToRead(filepath, file);

	// Check if correct file type
	string extension = filepath.substr(filepath.find_last_of(".") + 1);
	transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return std::toupper(c); });
	bool correctType{};
	if (extension == "OBJ")
	{
		correctType = true;
		conversiontype = ConversionType::OBJtoMSOBJ;
	}
	else if (extension == "MSOBJ")
	{
		correctType = true;
		conversiontype = ConversionType::MSOBJtoOBJ;
	}
	if (fileFound && !correctType) { cerr << RED_TEXT  "Error : Incorrect file type\n" << RESET_TEXT; }


	// Check if all good
	if (fileFound && correctType) { success = true; }

	return success;
}
void OBJ_to_MSOBJ(ifstream& input, const string& inputFilepath, const string& outputPath)
{
	/*GET FILE LENGTH*/
	int maxLines = static_cast<int>(count(istreambuf_iterator<char>(input), istreambuf_iterator<char>(), '\n'));
	input.seekg(0, std::ios::beg); //return to start of file

	/*OPEN FILE TO WRITE*/
	ofstream file_toWrite(outputPath, ios::binary);
	file_toWrite.close();
	ofstream file_toWrite_append(outputPath, ios::binary | ios::app);

	/*CONVERT LINES*/
	string conversion = GetConvertedOBJtoMSOBJString(input, maxLines);

	/*WRITE MAP LEGENDA*/
	WriteMap(file_toWrite_append);

	/*WRITE CONVERSION TO FILE*/
	file_toWrite_append.write(conversion.c_str(), conversion.length());

	/*CLOSE FILE TO WRITE*/
	file_toWrite_append.close();
	input.close();
}
void MSOBJ_to_OBJ(ifstream& input, const string& inputFilepath, const string& outputPath)
{
	/*GET FILE LENGTH*/
	int maxLines = static_cast<int>(count(istreambuf_iterator<char>(input), istreambuf_iterator<char>(), '\n'));
	input.seekg(0, std::ios::beg); //return to start of file

	// enum checking which type of conversion we need, DATA is its default state
	Linetype thistype = Linetype::OTHER;
	Linetype lasttype = Linetype::OTHER;
	Linetype conversiontype = Linetype::OTHER;

	/*ITERATE OVER FILE*/
	bool done{};
	int operations{};
	string line;
	string conversion{};

	while (getline(input, line) && !done)	//Iterate over each line
	{
		++operations;

		thistype = GetLineType(line);
		if (lasttype == Linetype::OTHER && thistype != Linetype::OTHER) { conversiontype = thistype; } // new section detected

		if (conversiontype == Linetype::MAP) { HandleMapLine(line, thistype, conversiontype); }
		else { conversion += GetMSOBJtoOBJLine(line, thistype, conversiontype); }

		lasttype = thistype;
		if (operations >= maxLines) { done = true; }
	}

	/*OPEN FILE TO WRITE*/
	ofstream file_toWrite;
	file_toWrite.open(outputPath);
	file_toWrite.write(conversion.c_str(), conversion.length());
	file_toWrite.close();
}
bool OpenFileToRead(const string filename, ifstream& file)
{
	file.open(filename);

	bool success{ false };
	if (file) { success = true; }
	else { cerr << RED_TEXT  "Error : File not found" << RESET_TEXT << endl; }

	return success;
}

#pragma region MSOBJ to OBJ
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
string GetMSOBJtoOBJLine(const string& line, Linetype thistype, Linetype conversiontype)
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
	g_postfixes_MSOBJtoOBJ.insert(std::make_pair(key, value));
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
		word = GetMSOBJVertexCoordinateToOBJVertexCoordinate(word);
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
	string result{ line };

	string conversion = GetWithoutFirstInstanceOf('*', result);
	result = g_commentIndicator + " " + conversion + '\n';

	return result;
}
string GetMSOBJVertexCoordinateToOBJVertexCoordinate(const string& coordinate)
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
	postfix = g_postfixes_MSOBJtoOBJ[postfix];

	result = prefix + postfix;
	return result;
}
#pragma endregion
#pragma region OBJ to MSOBJ
string GetConvertedOBJtoMSOBJString(ifstream& objFile, const int maxLines)
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
	coordinate.isNegative = CalculateCoordinateNegativity(coordinate._string); // _isNegative 
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

	try { result = g_postfixes_OBJtoMSOBJ.at(input); }
	catch (const out_of_range&)
	{
		AddNewPostfixToMap(input);
		result = g_postfixes_OBJtoMSOBJ.at(input);
	}

	return result;
}
void AddNewPostfixToMap(const string& newPostfixEntry)
{
	string result; //the new value that will be added as this postfixes' value in the map

	/*CALCULATE VALUE*/
	char startChar = 'A';
	char endChar = 'Z';

	int mapSize{ static_cast<int>(g_postfixes_OBJtoMSOBJ.size()) };
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
	g_postfixes_OBJtoMSOBJ.insert(make_pair(newPostfixEntry, result));
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
void WriteMap(ofstream& msObjFile)
{
	string line{};

	line += g_mapIndicator + '\n';

	for (auto it = g_postfixes_OBJtoMSOBJ.begin(); it != g_postfixes_OBJtoMSOBJ.end(); ++it) {

		line += "*" + it->second + " " + it->first + '\n';
	}

	//line += g_mapIndicator + +"end" + '\n';
	msObjFile.write(line.c_str(), line.length());
}
#pragma endregion

