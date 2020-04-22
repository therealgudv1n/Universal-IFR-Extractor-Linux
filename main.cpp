// Header files
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unistd.h>
#include "EFI.h"
#include "UEFI.h"

using namespace std;

// Error codes
enum {IFR_EXTRACTED, FILE_NOT_FOUND, UNKNOWN_PROTOCOL};

// Global definitions
enum type {EFI, UEFI, UNKNOWN};


// Global variables
string fileLocation, outputFile;
int errorCode;
string buffer;
type protocol;
vector<string> strings;


/*
Name: fileExists
Purpose: Checks if a file exists
*/
bool fileExists(const string &file);

/*
Name: readFile
Purpose: Reads specified file
*/
void readFile(const string &file, string &buffer);

/*
Name: getType
Purpose: Determines if file uses EFI or UEFI IFR protocol
*/
type getType(const string &buffer);



int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		if (fileExists(argv[1]))
		{
			readFile(argv[1], buffer);
			protocol = getType(buffer);
			//cout << protocol << '\n';
			
			if (protocol == UNKNOWN)
			{
				cout << "Protocol Unknown.\n";
				return 0;
			}
			else
			{
				strings.clear();
				if (protocol == EFI)
				{
					cout << "Protocol EFI detected.\n";
					// Initialize EFI variables
					vector<EFI_IFR_STRING_PACK> stringPackages;
					vector<EFI_IFR_FORM_SET_PACK> formSets;
					// Get EFI string packages
					getEFIStringPackages(stringPackages, buffer);
					// Get EFI strings
					getEFIStrings(stringPackages, strings, buffer);
					// Get EFI form sets
					getEFIFormSets(formSets, buffer, stringPackages, strings);
					// Generate EFI IFR dump
					generateEFIIFRDump("ifrdump.txt", stringPackages, formSets, buffer, strings);


					cout << "File ifrdump.txt generated successfully.\n";
				}
				else if (protocol == UEFI)
				{
					cout << "Protocol UEFI detected.\n";
					// Initialize UEFI variables
					vector<UEFI_IFR_STRING_PACK> stringPackages;
					vector<UEFI_IFR_FORM_SET_PACK> formSets;
					// Get UEFI string packages
					getUEFIStringPackages(stringPackages, buffer);
					// Get UEFI strings
					getUEFIStrings(stringPackages, strings, buffer);
					// Get UEFI form sets
					getUEFIFormSets(formSets, buffer, stringPackages, strings);
					// Generate UEFI IFR dump
					generateUEFIIFRDump("ifrdump.txt", stringPackages, formSets, buffer, strings);

					cout << "File ifrdump.txt generated successfully.\n";


				}
			}

		}
		else
		{
			cout << "File not exist.\n";
			return 0;
		}
	}
	else
	{
		cout << "No file specified.\n";
	}

	return 0;
}

// Supporting function implementation



bool fileExists(const string &file) {

	// Open file
	ifstream fin(file);

	// Return if first characetr doesn't equal EOF
	return fin.peek() != EOF;
}

void readFile(const string &file, string &buffer) {

	// Initialize variables
	ifstream fin(file, ios::binary);

	// Clear buffer
	buffer.clear();

	// Read in file
	while(fin.peek() != EOF)
		buffer.push_back(fin.get());

	// Close file
	fin.close();
}

type getType(const string &buffer) {

	// Go through buffer
	for(unsigned int i = 0; i < buffer.size() - 9; i++)
	
		// Check if an EFI string pakage was found
		if((buffer[i] != '\x00' || buffer[i + 1] != '\x00' || buffer[i + 2] != '\x00' || buffer[i + 3] != '\x00') && buffer[i + 4] == '\x02' && buffer[i + 5] == '\x00' && (buffer[i + 6] != '\x00' || buffer[i + 7] != '\x00' || buffer[i + 8] != '\x00' || buffer[i + 9] != '\x00') && i + static_cast<unsigned char>(buffer[i + 6]) + (static_cast<unsigned char>(buffer[i + 7]) << 8) + (static_cast<unsigned char>(buffer[i + 8]) << 16) + (static_cast<unsigned char>(buffer[i + 9]) << 24) < buffer.size() && buffer[i + static_cast<unsigned char>(buffer[i + 6]) + (static_cast<unsigned char>(buffer[i + 7]) << 8) + (static_cast<unsigned char>(buffer[i + 8]) << 16) + (static_cast<unsigned char>(buffer[i + 9]) << 24) + 4] >= 'a' && buffer[i + static_cast<unsigned char>(buffer[i + 6]) + (static_cast<unsigned char>(buffer[i + 7]) << 8) + (static_cast<unsigned char>(buffer[i + 8]) << 16) + (static_cast<unsigned char>(buffer[i + 9]) << 24) + 4] <= 'z' && i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) + (static_cast<unsigned char>(buffer[i + 3]) << 24) < buffer.size() && buffer[i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) + (static_cast<unsigned char>(buffer[i + 3]) << 24) + 4] == '\x02' && buffer[i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) + (static_cast<unsigned char>(buffer[i + 3]) << 24) + 5] == '\x00')
		
			// Return EFI
			return EFI;
	
		// Otherwise check if a UEFI string pakage was found
		else if((buffer[i] != '\x00' || buffer[i + 1] != '\x00' || buffer[i + 2] != '\x00') && buffer[i + 3] == '\x04' && buffer[i + 4] == '\x34' && buffer[i + 44] == '\x01' && buffer[i + 45] == '\x00' && buffer[i + 48] == '\x2D' && i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) < buffer.size() && buffer[i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) - 1] == '\x00' && buffer[i + static_cast<unsigned char>(buffer[i]) + (static_cast<unsigned char>(buffer[i + 1]) << 8) + (static_cast<unsigned char>(buffer[i + 2]) << 16) - 2] == '\x00')
	
			// Return UEFI
			return UEFI;
	
	// Return unknown
	return UNKNOWN;
}
