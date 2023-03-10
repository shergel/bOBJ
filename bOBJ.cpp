//#include <type_traits>
//#include <string>
//#include <iostream>
//#include <fstream>
//#include <cassert>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

using namespace std;

ifstream readFile;
ofstream writeFile;

bool OpenFileToRead(string filename)
{
	readFile.open(filename);
	if (!readFile)
	{
		cerr << "Error opening file" << endl;
		return false;
	}

	return true;
}
bool SkipToLineStartingWith(string toFind)
{
	bool found{};

	string currentLine{};

}

int main() {

	/*OPEN FILE*/
	string filename = "debugfile.obj";
	if (!OpenFileToRead(filename)) { return 1; }

	//////////////////////////////////////////////////////////////////////////////////////////////// CLEAN
	/*FIND INDEX STARTING WITH 'v' */
	// Loop through each line of the file and find the first line that starts with "v "
	string line;
	string toFind{ "v" };
	bool found{ false };
	int index{ 0 };
	while (getline(readFile, line) && !found) {
		if (line.compare(0, toFind.length(), toFind) == 0) {
			found = true;
			cout << "Found line starting with \"" + toFind + "\" at index " << index << endl;
		}
		index++;
	}
	if (!found)
	{
		cerr << "No line starting with \"" + toFind + "\" found." << endl;
	}

	/*WRITE LINE*/
	std::ofstream newFile("test.mar"); // create an output file stream


	/*SPLIT LINE INTO 3*/
	string vertex[4]{}; //TURN THIS INTO 3? ONCE V HAS BEEN REMOVED

	istringstream iss(line); // create an istringstream object from the line
	string word;
	int idx{ 0 };
	while (iss >> word) { // extract words from the istringstream object using the delimiter " "
		vertex[idx] = word; // add the word to the vector
		++idx;
	}
	//print just to check : 
	for (int i{}; i < 4; ++i)
	{
		std::cout << vertex[i] << "---";
	}

	/*FIND CHARACTER 'e', CHECK IF LAST FEW CHARACTERS ARE IN LIST OF KNOWN SUFFIXES, IF SO, REPLACE IT WITH THAT LETTER*/
	// IF NOT, CREATE NEW LETTER IN LIST AND REPLACE SUFFIX W THAT LETTER
	// BEFORE THE e, REMOVE THE 0s IF ANY
			/*MAP FOR SUFFIXES*/
	std::map<std::string,char> THEMAP;
	int age = int('A');
	int distCapNoCap = int('a') - int('A');


	std::string prefix;
	std::string suffix;
	for (int i{ 1 }; i < 4; ++i)
	{
		std::string str = vertex[i]; //loop over 1 , 2, and 3! DELETE 0!

		size_t pos = str.find('e'); // Find the position of the first occurrence of 'e'
		if (pos != std::string::npos) { // Check if 'e' is found
			prefix = str.substr(0, pos);
			suffix = str.substr(pos); // Extract the suffix starting from 'e'
		}
		std::cout << '\n' << "Suffix: " << suffix << std::endl; // Print the suffix

		/*CHECK FOR ZEROES*/
		cout << prefix << " about to be checked for zeroes and periods ... \n"; //add negativity later
		while (prefix.back() == '0') {
			prefix.pop_back(); // remove the last character
		}
		/*remove .*/
		size_t pos2 = prefix.find('.');
		// Use the std::string::erase method to remove the character
		if (pos2 != std::string::npos) {
			prefix.erase(pos2, 1);
		}
		bool isPrefixNegative{};
		///*CHECK IF PREFIX NEGATIVE*/
		//if (prefix[0] == '-')
		//{
		//	prefix = prefix.substr(1); //remove first character
		//	isPrefixNegative= true;
		//}
		cout << prefix << " <- result. \n";

		
		try { 
			std::cout << "SUFFIX FOUND :" << prefix << THEMAP.at(suffix) << '\n'; 
		}// This will throw a std::out_of_range exception
		catch (const std::out_of_range& e) {
			std::cerr << "Key not found: " << e.what() << '\n';

			THEMAP.insert(std::make_pair(suffix, char(age))); // add key-value pair using insert and make_pair
			std::cout << "the suffix for " << suffix << " has been added to the list : " << prefix << THEMAP[suffix] << '\n';
			++age; //MAKE SURE IF AGE GOES OVER OR UNDER CERTAIN AMT, IT GETS ADDED ON TOP 
		}

		
		if (newFile.is_open()) {
			newFile << prefix << THEMAP[suffix];
		}
	}
	

	newFile.close();
	std::cin.get();
}


//file.seekg(0, ios::beg); //GOES TO FIRST CHARACTER OF FILE

void CompressVertices()
{
	bool isTheNextLineAVertex{ false }; //SET TO TRUE AFTER CHECK IS DONE



	//Float3f toRead;
	//std::ifstream input;
	//input.open("test.bin", std::ios::binary);

	//if (input.is_open()) // ask class : how tf do I check if the file even opened? 
	//{
	//	input.read((char*)&toRead, sizeof(toRead));
	//	assert(toRead.x == toWrite.x);
	//	assert(toRead.y == toWrite.y);
	//	assert(toRead.z == toWrite.z);
	//	input.close();
	//}

	string converted{};

	do
	{


		//check if next line is a vertex, update isNextLineAVertex
	} while (isTheNextLineAVertex);

	std::cout << converted;


	//	//WRITE
//
//	std::string pod = std::is_pod<Float3f> ::value == 1 ? "yes" : "no";
//	std::cout << "Is the Float3 struct a pod : " << pod << std::endl;
//
//	Float3f toWrite;
//	toWrite.x = 6.4f;
//	toWrite.y = 9.8f;
//	toWrite.z = 9.5f;
//
//	std::ofstream temp;
//	temp.open("test.bin", std::ios::binary);
//	if (temp.is_open())
//	{
//		temp.write((const char*)&toWrite, sizeof(toWrite));
//		temp.close();
//	}









// split up the 3 into a vector for the line
// first one, find an e?
// what comes after an e?
// check if it's in THE LIST
// no? add new letter to the list
// rename it to that letter
// yes? rename it to that letter
// move onto next nr
// repeat




/* Write a binary writer */
//#include <type_traits>
//#include <string>
//#include <iostream>
//#include <fstream>
//#include <cassert>
//
//struct Float3f
//{
//	float x, y, z;
//};

//
//int main()
//{
//
//	//WRITE
//
//	std::string pod = std::is_pod<Float3f> ::value == 1 ? "yes" : "no";
//	std::cout << "Is the Float3 struct a pod : " << pod << std::endl;
//
//	Float3f toWrite;
//	toWrite.x = 6.4f;
//	toWrite.y = 9.8f;
//	toWrite.z = 9.5f;
//
//	std::ofstream temp;
//	temp.open("test.bin", std::ios::binary);
//	if (temp.is_open())
//	{
//		temp.write((const char*)&toWrite, sizeof(toWrite));
//		temp.close();
//	}
//
//
//	//READ
//
//	Float3f toRead;
//	std::ifstream input;
//	input.open("test.bin", std::ios::binary);
//
//	if (input.is_open()) // ask class : how tf do I check if the file even opened? 
//	{
//		input.read((char*)&toRead, sizeof(toRead));
//		assert(toRead.x == toWrite.x);
//		assert(toRead.y == toWrite.y);
//		assert(toRead.z == toWrite.z);
//		input.close();
//	}
//
//
//	std::cout
//		<< "original float3 : [" << toWrite.x << ", " << toWrite.y
//		<< ", " << toWrite.z << "]" << std::endl;
//	std::cout
//		<< "read float3 : [" << toRead.x << ", " << toRead.y
//		<< ", " << toRead.z << "]" << std::endl;
//
//
//
//
//
//	std::cin.get();
//}
//
////size of file
////std::filesystem::path myFilePath{ "test.bin" };
////std::filesystem.filesize...
//
//// do NOT write a pointer to your string to a file
}