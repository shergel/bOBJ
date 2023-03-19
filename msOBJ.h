 #pragma once
using namespace std;
#pragma region Header

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

enum ConversionType
{
	NONE,
	OBJtoMSOBJ,
	MSOBJtoOBJ
};

void OBJ_to_MSOBJ(ifstream& input, const string& inputFilepath, const string& outputPath);
void MSOBJ_to_OBJ(ifstream& input, const string& inputFilepath, const string& outputPath);

bool AccessFilePath(ifstream& file, const string& filepath, ConversionType& conversiontype);
bool OpenFileToRead(const string filename, ifstream& file);
string GetConvertedOBJtoMSOBJString(ifstream& objFile, const int maxLines);
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
void WriteMap(ofstream& msObjFile);


const string g_noPostfixIndicator{ "NONE" };
map<string, string> g_postfixes_OBJtoMSOBJ//map that stores all common vertex postfixes
{
{ g_noPostfixIndicator, "A"}
};
map<string, string> g_postfixes_MSOBJtoOBJ{};

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
string GetMSOBJtoOBJLine(const string& line, Linetype thistype, Linetype conversiontype);
void HandleMapLine(const string& line, Linetype thistype, Linetype conversiontype);
string GetDecompressedVertex(const string& line);
string GetDecompressedVertexNormal(const string& line);
string GetDecompressedFace(const string& line);
string GetDecompressedComment(const string& line);
string GetMSOBJVertexCoordinateToOBJVertexCoordinate(const string& coordinate);
