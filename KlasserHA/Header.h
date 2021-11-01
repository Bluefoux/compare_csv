#pragma once
#ifndef DATABASECLASSHPP
#define DATABASECLASSHPP

#include <comdef.h>
#include <gcroot.h>
#include <string>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <algorithm>
#include <msclr\marshal_cppstd.h>

#using <mscorlib.dll>
using namespace std;
using namespace System;
using namespace System::Data;
using namespace System::Data::SqlClient;
using namespace System::Runtime::InteropServices;

class DatabaseClass {
private:
	gcroot<DataTable^> table;
public:
	DatabaseClass() : table(nullptr) {}

	cli::array<String^>^ CreateAndPopulateTable(ifstream& inFile1, const char* sep)
	{
		int i = 0;
		int x = 0;
		int pos = 0;
		string line, word, temp;
		DataRow^ myDataRow;
		cli::array<String^>^ arr = gcnew cli::array<String^>(40);

		//create table
		table = gcnew DataTable("SampleTable");
		stringstream s(line);
		getline(inFile1, line);
		string tmp = line;
		//add all the columns to the table
		do {
			istringstream iline(tmp);
			word = tmp.substr(0, pos);
			word.erase(std::remove(word.begin(), word.end(), ','), word.end());
			String^ word2 = gcnew String(word.c_str());
			DataColumn^ column1 = gcnew DataColumn(word2, Type::GetType("System.String"));
			if (word != "") {
				try {
					table->Columns->Add(column1);
					tmp = tmp.substr(pos + 1, tmp.length());
					arr[x] = word2;
					x++;
				}
				catch (...) {
					break;
				}
				i++;
			}
		} while ((pos = tmp.find(sep)) != string::npos + 1);

		//add all the rows to the table
		while (getline(inFile1, line)) {
			if (line != "") {
				string tmp = line;
				myDataRow = table->NewRow();
				int k = 0;
				for (int k = 0; k < i; k++) {
					pos = tmp.find(sep);
					word = tmp.substr(0, pos);
					String^ word2 = gcnew String(word.c_str());
					myDataRow[k] = word2;
					tmp = tmp.substr(pos + 1, tmp.length());
				}
				table->Rows->Add(myDataRow);
			}
		}
		return arr;
	}

	cli::array<DataRow^>^ Select(char* value, string colname) {
		cli::array<DataRow^>^ rows = {};
		string filterExpression = colname + "='" + value + "'";
		String^ filterExpression2 = gcnew String(filterExpression.c_str());
		if (value != "" && colname != "") {
			rows = table->Select(filterExpression2);
		}

		return rows;
	}

	int GetValuesForColumn(char* dataColumn, char** values, int valuesLength)
	{
		String^ columnStr = Marshal::PtrToStringAnsi((IntPtr)dataColumn);
		cli::array<DataRow^>^ rows = table->Select();
		int len = rows->Length;
		len = (len > valuesLength) ? valuesLength : len;
		for (int i = 0; i < len; i++)
		{
			// Marshal each column value from a managed string
			// to a char *.
			values[i] = (char*)Marshal::StringToHGlobalAnsi((String^)rows[i][columnStr]).ToPointer();
		}
		return len;
	}
		
	void tocsv(string testcolumn, char** values, DatabaseClass* tableDiff, string csvnamnstr, cli::array<String^>^ testarray) {
		int nrOfColumns = table->Columns->Count;
		int nrOfRows = table->Rows->Count;
		cli::array<DataRow^>^ rows = table->Select();
		String^ rowText = "";
		String^ rowSep = ";";
		string strtofile = "";
		ofstream utResult;

		utResult.open(csvnamnstr + "_Resultat.csv");
		for (int i = 0; i < testarray->Length-1; i++) {
			if (testarray[i] != nullptr) {
				msclr::interop::marshal_context context;
				utResult << ";" + context.marshal_as<string>(testarray[i]);
			}
			else {
				utResult << ";Resultat\n";
				break;
			}
		}

		for (int i = 0; i < nrOfRows; i++)
		{
			cli::array<DataRow^>^ rows2 = tableDiff->Select(values[i], testcolumn);

			for (int k = 0; k < nrOfColumns; k++)
			{
				rowText = rowText + rowSep + rows[i][k];
			}

			if (rows2->Length == 0) {
				rowText = rowText + ";SAKNAS\n";
			}
			else
			{
				rowText = rowText + ";-\n";
			}
			msclr::interop::marshal_context contxt;
			utResult << contxt.marshal_as<string>(rowText);
			rowText = "";
			rows2 = nullptr;
			delete[] rows2;
		}

		rows = nullptr;
		delete rows;
		delete rowText;
		delete rowSep;
		utResult.close();
	}

	void to_csv2(string testcolumn, char** values, DatabaseClass* tableDiff, string csvnamnstr, cli::array<String^>^ testarray, int kolumn) {
		int nrOfColumns = table->Columns->Count;
		int nrOfRows = table->Rows->Count;
		cli::array<DataRow^>^ rows = table->Select();
		String^ rowText = "";
		String^ rowSep = ";";
		string strtofile = "";
		ofstream utResult;

		utResult.open(csvnamnstr + "_Resultat.csv");
		for (int i = 0; i < testarray->Length - 1; i++) {
			if (testarray[i] != nullptr) {
				msclr::interop::marshal_context context;
				utResult << ";" + context.marshal_as<string>(testarray[i]);
			}
			else {
				utResult << ";Resultat\n";
				break;
			}
		}

		for (int i = 0; i < nrOfRows; i++)
		{
			cli::array<DataRow^>^ rows2 = tableDiff->Select(values[i], testcolumn);

			for (int k = 0; k < nrOfColumns; k++)
			{
				rowText += rowSep + rows[i][k];
			}

			if (rows2->Length == 0) {
				rowText += rowSep + "Saknas" + "\n";
			}
			else
			{
				rowText += rowSep + rows2[0][kolumn-1] + "\n";
			}
			msclr::interop::marshal_context contxt;
			utResult << contxt.marshal_as<string>(rowText);
			rowText = "";
			rows2 = nullptr;
			delete[] rows2;
		}
		rows = nullptr;
		delete rows;
		delete rowText;
		delete rowSep;
		utResult.close();
	}
};

#endif //DATABASECLASSHPP