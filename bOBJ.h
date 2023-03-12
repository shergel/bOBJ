#pragma once
//todo : check if any of these are useless
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

using namespace std;
//TODO MAKE FUNCTIONS CONST WHEN NEEDED

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

bool OpenFileToRead(const string filename, ifstream& file);
void ConvertAndWriteFile(ifstream& objFile, const int maxLines, ofstream& bObjFile);
string GetConvertedVertexLine(const string& line);
void HandleFace();
void HandleVertexNormal();
void HandleComment();
void LineStringToVertex(const string& line, Vertex& vertex);
void VertexStringToCoordinate(Coordinate& coordinate, const string& fullString);
string CalculateCoordinatePrefix(const string& coordinateString);
string CalculateCoordinatePostfix(const string& coordinateString);
bool CalculateCoordinateNegativity(const string& coordinateString);
void ConvertVertex(Vertex& vertex);
void ConvertCoordinate(Coordinate& coordinate);
void ConvertCoordinatePrefix(Coordinate& coordinate);
void ConvertCoordinatePostfix(Coordinate& coordinate);
string GetWithoutZeroesAtEnd(const string& input);
string GetWithoutFirstInstanceOf(char toRemove, const string& input);
string GetPostfixAsLetter(const string& input);
void AddNewPostfixToMap(const string& newPostfixEntry);
string GetLastCharCapitalized(const string& input, bool capitalized);
map<string, string> g_commonPostfixes; //map that stores all common vertex postfixes todo: make local?


//todo : find random line? just copy it, just in case