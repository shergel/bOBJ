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
void ConvertFile(ifstream& objFile, const int maxLines);
void HandleVertexLine(const string& line);
void HandleFace();
void LineStringToVertex(const string& line, Vertex& vertex);
void VertexStringToCoordinate(Coordinate& coordinate, const string& fullString);
string CalculateCoordinatePrefix(const string& coordinateString);
string CalculateCoordinatePostfix(const string& coordinateString);
bool CalculateCoordinateNegativity(const string& coordinateString);
void ConvertVertex(Vertex& vertex);
void ConvertCoordinate(Coordinate& coordinate);
void ConvertCoordinatePrefix(Coordinate& coordinate);
void ConvertCoordinatePostfix(Coordinate& coordinate);


////global variables
//ofstream g_writeFile("test.mar");

//std::map<std::string, char> g_map;
//int g_idx{ int('A') }; //make sure this cannot go over a certain nr AND make sure it can be nocap when negative
//
//string g_writeLine{};