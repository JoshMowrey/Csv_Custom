#include <OpenXLSX.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <unordered_map>
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;
using std::unordered_map;
using namespace OpenXLSX;
unordered_map<string, bool> states;
unordered_map<string, bool> fullstates;
XLDocument OutDoc;
XLWorkbook OutBook;
XLWorksheet OutSheet;
vector<string> findAllofDir(string &Directory, string &FileType);
void xlsxDoStuff(string &FileName);
void BlueStar(string &FileName);
void ScanSource(string &FileName);
void ParseChanges(string &outFile, string &inFile);

int main(void)
{
    string outFile, inFile;
    ParseChanges(outFile, inFile);
    return 0;
}


vector<string> findAllofDir(string& Directory, string& FileType){
    std::filesystem::path cPath(Directory);
    std::filesystem::directory_iterator start(cPath);
    vector<string> vec;
    for (const auto & it :  start){
        string File_Name = it.path().stem().string();
        if (File_Name.find(FileType)){
            ifstream temp;
            temp.open(File_Name);
            if (temp.bad()){
                cout << "\n\nERROR: Bad File\nFile not found\n" << endl;
            }
            else {
                vec.push_back(File_Name);
            }
        }
    }
    return vec;
}


void xlsxDoStuff(string& FileName){
    if (FileName.find("BlueStar")){
        BlueStar(FileName);
    }
    else if (FileName.find("ScanSource")){
        ScanSource(FileName);
    }
}


void BlueStar(string& FileName){
    OutSheet = OutBook.sheet("BlueStar");
    XLDocument thisDoc(FileName);
    XLWorkbook thisWorkbook = thisDoc.workbook();
    XLWorksheet thisSheet = thisWorkbook.worksheet("US");
    uint32_t length = thisSheet.rowCount();
    OutSheet.row(OutSheet.rowCount()) = thisSheet.row(1);
    for (int i = 1; i < length; i++){

        XLCell thisResellerState = thisSheet.cell(i, 7/*G*/);
        XLCell thisShipToState = thisSheet.cell(i, 20 /*T*/);
        XLCell thisEndCustomerState = thisSheet.cell(i, 22 /*V*/);

        if (states[thisResellerState.getString()] == true || states[thisShipToState.getString()] == true || states[thisEndCustomerState.getString()] == true ){
            OutSheet.row(OutSheet.rowCount()) = thisSheet.row(i);
        }
    }
}


void ScanSource(string& FileName){
    OutSheet = OutBook.sheet("ScanSource");
    XLDocument thisDoc(FileName);
    XLWorkbook thisWorkbook = thisDoc.workbook();
    XLWorksheet thisSheet = thisWorkbook.worksheet("Complete POS Report");
    uint32_t length = thisSheet.rowCount();
    OutSheet.row(OutSheet.rowCount()) = thisSheet.row(1);
    for (int i = 1; i < length; i++){
        XLCell thisResellerState = thisSheet.cell(i, 16/*P*/);
        XLCell thisShipToState = thisSheet.cell(i, 21 /*U*/);
        XLCell thisEndCustomerState = thisSheet.cell(i, 28 /*AB*/);

        if (states[thisResellerState.getString()] == true || fullstates[thisShipToState.getString()] == true || states[thisEndCustomerState.getString()] == true ){
            OutSheet.row(OutSheet.rowCount()) = thisSheet.row(i);
        }
    }
}


void ParseChanges(string &outFile, string &inFile)
{
    ifstream Changes;
    Changes.open("Changes.txt");
    if (Changes.good())
    {
        string cLine;

        std::getline(Changes, cLine);
        inFile = cLine.substr(cLine.length() - cLine.find(':'), cLine.find(':'));

        std::getline(Changes, cLine);
        int start = cLine.find(':');
        string cState;
        for (int i = start; i < cLine.length(); i++)
        {
            if (cLine[i] != ',')
            {
                cState += cLine[i];
            }
            else
            {
                states[cState] = true;
                cState = string();
            }
        }

        std::getline(Changes, cLine);
        start = cLine.find(':');
        cState = string();
        for (int i = start; i < cLine.length(); i++)
        {
            if (cLine[i] != ',')
            {
                cState += cLine[i];
            }
            else
            {
                fullstates[cState] = true;
                cState = string();
            }
        }
        std::getline(Changes, cLine);
        outFile = cLine.substr(cLine.length() - cLine.find(':'), cLine.find(':'));

        std::getline(Changes, cLine);
        outFile += cLine.substr(cLine.length() - cLine.find(':'), cLine.find(':'));
    }
}