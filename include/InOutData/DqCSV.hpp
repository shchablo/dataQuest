//------------------------------------------------------------------------------
/* INFORMATION: 
Important parametars: */
//------------------------------------------------------------------------------

#ifndef DQCSV_h
#define DQCSV_h

// C++ includes
#include <vector>
#include <map>
#include <fstream>
#include <iostream>

class DqCSV
{
public:

  /* Constructor, destructor. */ 
  DqCSV();
  DqCSV(std::string path);
  DqCSV(std::string path, 
        std::string delimiter, std::string rowDelimiter);
  
  virtual ~DqCSV();
  //----------------------------------------------------------------------------
  bool open(std::ios_base::openmode mode);
  bool open(std::string path, std::ios_base::openmode mode);

  bool close();
  //----------------------------------------------------------------------------

  /* Set functions. */ 
	bool setPath(std::string path);
  
  bool setDelimiter(std::string delimiter);

  bool setRowDelimiter(std::string rowDelimiter);
  //----------------------------------------------------------------------------
	
  /* Get functions. */ 
	std::string getPath();
	
  unsigned int getLength();
  
  std::string getDelimiter();
  
  std::string getRowDelimiter();

  std::ios_base::openmode getMode();
  //----------------------------------------------------------------------------
  
  /* Functions. */ 
	bool numCells(unsigned int *numCells);	
  bool numRows(unsigned int *numRows);
	bool numColumn(unsigned int *numColumn);
	
	bool readCell(unsigned int indexRow, unsigned int indexColumn, 
                std::string *value);
  bool readRow(unsigned int indexRow, std::vector<std::string> *row);
	bool readColumn(unsigned int indexColumn, std::vector<std::string> *column);
	
  bool addRow(std::vector<std::string> *row);
  //----------------------------------------------------------------------------

protected:

  std::ios_base::openmode _mode;

  std::fstream _file;
  
  unsigned int _length; // length of file

  std::string _delimiter;
  
  std::string _rowDelimiter;

  std::string _path;

private:


  bool fixFile(std::string path);
};

#endif
//------------------------------------------------------------------------------
