/*
 * Filename:            cdkexample.cc
 * Date:                11/20/2020
 * Author:              Karin Nielsen
 * Version:             1.0
 * Copyright:           2020, All Rights Reserved
 *
 * Description:
 *      Build and display a small text based GUI matrix using curses, 
 *      the CDK, and binary I/O.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdint.h>
#include "cdk.h"

//define preprocessor values for matrix variables
#define MATRIX_ROWS 5
#define MATRIX_COLS 3
#define BOX_WIDTH 20
#define MATRIX_NAME_STRING "Binary File Contents"

//define class for header records
class BinaryFileHeader {
public: 
  uint32_t magicNumber;
  uint32_t versionNumber;
  uint64_t numRecords;
};

const int maxRecordStringLength = 25;

//define class for non-header file records
class BinaryFileRecord {
public: 
  uint8_t strLength;
  char stringBuffer[maxRecordStringLength];
};

using namespace std;

int main() {
  WINDOW	*window;
  CDKSCREEN	*cdkscreen;
  CDKMATRIX     *myMatrix;

  //define variables for matrix labels
  const char 		*rowTitles[MATRIX_ROWS+1] = {"IGNORE", "a", "b", "c", "d", "e"};
  const char 		*columnTitles[MATRIX_COLS+1] = {"IGNORE", "a", "b", "c"};
  int		colWidths[MATRIX_COLS+1] = {BOX_WIDTH, BOX_WIDTH, BOX_WIDTH, BOX_WIDTH};
  int		colTypes[MATRIX_COLS+1] = {vMIXED, vMIXED, vMIXED, vMIXED};

  //define variables for bin file objects and infile streaming
  ifstream binInfile;
  BinaryFileHeader *headRecord = new BinaryFileHeader();
  BinaryFileRecord *myRecord = new BinaryFileRecord();
  
  //define additional variables for manipulating bin infile streaming
  stringstream s;
  string head1, head2, head3;
  string mag = "Magic: 0x";
  string vers = "Version: ";
  string numR = "NumRecords: ";
  string sLen = "strlen: ";
  string clear = "";
  string b;
  int iLen;

  //initialize curses data structure for screen, set terminal size, start colors
  window = initscr();
  cdkscreen = initCDKScreen(window);
  initCDKColor();

  //create matrix
  myMatrix = newCDKMatrix(cdkscreen, CENTER, CENTER, MATRIX_ROWS, MATRIX_COLS, MATRIX_ROWS, MATRIX_COLS,
			  MATRIX_NAME_STRING, (char **) rowTitles, (char **) columnTitles, colWidths,
				     colTypes, 1, 1, ' ', ROW, true, true, false);

  if (myMatrix==NULL) {
    printf("Error creating Matrix\n");
    _exit(1);
  }

  //do the open/read work here
  binInfile.open("/scratch/perkins/cs3377.bin", ios::in | ios::binary);
  binInfile.read((char *) headRecord, sizeof(BinaryFileHeader));

  drawCDKMatrix(myMatrix, true);

  //header reading/manipulation
  //display message at 1,1
  s << hex << uppercase << headRecord->magicNumber; 
  head1 = s.str();
  setCDKMatrixCell(myMatrix, 1, 1, string(mag + string(head1.c_str())).c_str());
  s.str(clear);
  //display message at 1,2
  s << dec << headRecord->versionNumber;
  head2 = s.str();
  setCDKMatrixCell(myMatrix, 1, 2, string(vers + string(head2.c_str())).c_str());
  s.str(clear);
  //display message at 1,3
  s << dec << headRecord->numRecords;
  head3 = s.str();
  setCDKMatrixCell(myMatrix, 1, 3, string(numR + string(head3.c_str())).c_str());
  s.str(clear);

  //non-header reading/manipulation. Display 4 records.
  for (int i=2 ; i<6 ; i++) {
    binInfile.read((char *) myRecord, sizeof(BinaryFileRecord));

    iLen = myRecord->strLength;
    s << sLen << iLen;
    b = s.str();
    char* body = (char*) b.c_str();
    s.str(clear);
    setCDKMatrixCell(myMatrix, i, 1, body);
    setCDKMatrixCell(myMatrix, i, 2, myRecord->stringBuffer);
  }

  drawCDKMatrix(myMatrix, true);    /* required  */

  //keep matrix on screen until user input
  char usrIn;
  cin >> usrIn;

  //cleanup screen, close file
  endCDK();
  binInfile.close();
  return 0;

}