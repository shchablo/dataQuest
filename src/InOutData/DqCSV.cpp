#include "DqCSV.hpp"

/* Constructor, destructor. */ 
/*! \fn DqCSV
* \brief   constructtor
*/
DqCSV::DqCSV()
{
  _path = "";
  _delimiter = ",";
  _rowDelimiter = "\n";
  _mode = std::fstream::in;
  _length = 0;

}
  
/*! \fn DqCSV
* \brief   constructtor
* 1) path - full path to the path.
*/
DqCSV::DqCSV(std::string path)
{
  _path = path;
  _delimiter = ";";
  _rowDelimiter = "\n";
  _mode = std::fstream::in;
  _length = 0;
}

/*! \fn DqCSV
* \brief   constructtor
* 1) path - full path to the path;
* 2) delimiter - list separator for .csv path;
* 3) rowDelimiter - row separator for .csv path.
*/
DqCSV::DqCSV(std::string path, 
             std::string delimiter, std::string rowDelimiter)
{
  _path = path;
  _delimiter = delimiter;
  _rowDelimiter = rowDelimiter;
  _mode = std::fstream::in;
  _length = 0;

}

/*! \fn ~DqCSV
* \brief  destructor
*/
DqCSV::~DqCSV()
{
}
//------------------------------------------------------------------------------
/*! \fn open
* \brief   to open file.
* 1) mode - mode for open file.
*/
bool DqCSV::open(std::ios_base::openmode mode) 
{
  std::string path = _path;

  bool isFixFile = fixFile(path);
  if(!isFixFile)
    return false;

  _file.open(path.c_str(), mode);
  if(!_file.is_open())
    return false;
    
  _file.seekg(0, _file.end);
  _length = _file.tellg();
  _file.seekg(0, _file.beg);

  _mode = mode;
  
  return true;
}

/*! \fn open
* \brief   to open file.
* 1) path - full path to the path;
* 2) mode - mode for open file.
*/
bool DqCSV::open(std::string path, std::ios_base::openmode mode) 
{
  
  bool isFixFile = fixFile(path);
  if(!isFixFile)
    return false;

  _file.open(path.c_str(), mode);
  if(!_file.is_open())
    return false;
  
  _file.seekg(0, _file.end);
  _length = _file.tellg();
  _file.seekg(0, _file.beg);
  
  _mode = mode;
  _path = path;
  
  return true;
}

/*! \fn close
* \brief   to close file.
*/
bool DqCSV::close() 
{
  if(!_file.is_open())
    return false;

  _file.close();
  return true;

}
//------------------------------------------------------------------------------

/* Set functions. */ 
/*! \fn setPath
* 1) path - full path to the file.
*/
bool DqCSV::setPath(std::string path)
{
	if(path.size() == 0)
    return false; 
  
  _path = path;
  return true;
}

/*! \fn setDelimiter
* 1) delimiter - list separator for .csv file.
*/
bool DqCSV::setDelimiter(std::string delimiter)
{
	if(delimiter.size() == 0 || delimiter.size() > 1)
    return false;
    
  _delimiter = delimiter;
  return true;
}

/*! \fn setRowDelimiter
* 1) rowDelimiter - row separator for .csv file.
*/
bool DqCSV::setRowDelimiter(std::string rowDelimiter)
{
	if(rowDelimiter.size() == 0)
    return false;
    
  _rowDelimiter = rowDelimiter;
  return true;
}
//------------------------------------------------------------------------------

/* Get functions. */ 
std::string DqCSV::getPath()
{
  return _path;
}

unsigned int DqCSV::getLength()
{
  return _length;
}

std::string DqCSV::getDelimiter()
{
  return _delimiter;
}

std::string DqCSV::getRowDelimiter()
{
  return _rowDelimiter;
}

std::ios_base::openmode DqCSV::getMode()
{
  return _mode;
}
//------------------------------------------------------------------------------

/* Functions. */ 
// public
bool DqCSV::numCells(unsigned int *numCells) 
{
    if(!_file.good())
      return false;
    
    std::string rowDelimiter = _rowDelimiter;
    std::string delimiter = _delimiter;
    unsigned int length = _length;
    std::string str = "";
    size_t prev = 0; size_t next = 0;
    size_t delta = delimiter.length();
    bool isParse = true;
    *numCells = 0;
    _file.seekg(0, _file.beg);
    while(getline(_file, str)) {
      prev = 0; next = 0;
		  while((next = str.find(delimiter, prev)) != std::string::npos) {
          isParse = true;
          prev = next + delta;
          *numCells = *numCells + 1;
      }
      if(isParse == true) {
        *numCells = *numCells + 1;
        isParse = false;
      }
      if(_file.tellg() == length)
        break;
    }
    _file.seekg(0, _file.beg);
    return true;
}

bool DqCSV::numRows(unsigned int *numRows)
{
    if(!_file.good())
      return false;
    
    std::string rowDelimiter = _rowDelimiter;
    unsigned int length = _length;
    std::string str = "";
    *numRows = 0;
    _file.seekg(0, _file.beg);
    while(getline(_file, str)) {
      *numRows = *numRows + 1;
      if(_file.tellg() == length)
        break;
    str.clear();
    }
    _file.seekg(0, _file.beg);
    return true;
}
	
bool DqCSV::numColumn(unsigned int *numColumn)
{
    if(!_file.good())
      return false;
    
    std::string rowDelimiter = _rowDelimiter;
    std::string delimiter = _delimiter;
    std::string str = "";
    size_t prev = 0; size_t next = 0;
    size_t delta = delimiter.length();
    bool isParse = true;
    
    *numColumn = 0;
    _file.seekg(0, _file.beg);
     getline(_file, str);
      prev = 0; next = 0;
		  while((next = str.find(delimiter, prev)) != std::string::npos) {
          isParse = true;
          prev = next + delta;
          *numColumn = *numColumn + 1;
      }
      if(isParse == true) {
        *numColumn = *numColumn + 1;
        isParse = false;
      }
    _file.seekg(0, _file.beg);
    return true;
}

bool DqCSV::readCell(unsigned int indexRow, unsigned int indexColumn, 
                     std::string *value)
{
    if(!_file.good())
      return false;

    std::string rowDelimiter = _rowDelimiter;
    std::string delimiter = _delimiter;
    unsigned int length = _length;
    std::string str = "";
    size_t prev = 0; size_t next = 0;
    size_t delta = delimiter.length();
    
    _file.seekg(0, _file.beg);
    for(unsigned int l = 0; l <= indexRow; l++) {
      getline(_file, str);
      if(l == indexRow) {
        for(unsigned int c = 0; c <= indexColumn; c++) {
          if(next == std::string::npos) {
            _file.seekg(0, _file.beg);
            return false;
          }
          next = str.find(delimiter, prev);
          if(c == indexColumn)
            *value = str.substr(prev, next-prev);
          prev = next + delta;
        }
      }
      if(_file.tellg() == length) {
        if(l == indexRow)
          break;
        else {
          _file.seekg(0, _file.beg);
          return false;
        }
      }
    }
    _file.seekg(0, _file.beg);
    return true;
}

bool DqCSV::readRow(unsigned int indexRow, std::vector<std::string> *row)
{
    if(!_file.good())
      return false;
    
    std::string rowDelimiter = _rowDelimiter;
    std::string delimiter = _delimiter;
    std::string endStr;
    std::string tmpStr;
    unsigned int length = _length;
    std::string str = "";
    size_t prev = 0; size_t next = 0; size_t lastPrev = 0;
    size_t delta = delimiter.length();
    
    
    _file.seekg(0, _file.beg);
    for(unsigned int l = 0; l <= indexRow; l++) {
      getline(_file, str);
      tmpStr = str + rowDelimiter.at(rowDelimiter.size()-1);  
      if(l == indexRow) {
        while(next != std::string::npos) {
          lastPrev = prev;
          next = str.find(delimiter, prev);
          row->push_back(str.substr(prev, next-prev));
          prev = next + delta;
        }
        endStr = tmpStr.substr(lastPrev, tmpStr.size() - lastPrev);
        if(endStr != rowDelimiter) {
          row->pop_back();
          row->push_back(tmpStr.substr(lastPrev, tmpStr.size() - lastPrev - rowDelimiter.size()));
        }
        else {
          tmpStr = row->at(row->size()-1);
          row->pop_back();
          row->push_back("");

        }

      }
      if(_file.tellg() == length) {
        if(l == indexRow)
          break;
        else {
          _file.seekg(0, _file.beg);
          return false;
        }
      }
    }
    _file.seekg(0, _file.beg);
    return true;
}

bool DqCSV::readColumn(unsigned indexColumn, std::vector<std::string> *column) 
{
    if(!_file.good())
      return false;
    
    std::string rowDelimiter = _rowDelimiter;
    std::string delimiter = _delimiter;
    unsigned int length = _length;
    std::string str = "";
    size_t prev = 0; size_t next = 0;
    size_t delta = delimiter.length();
    
    _file.seekg(0, _file.beg);
    while(getline(_file, str)) {
      prev = 0; next = 0;
      for(unsigned int c = 0; c <= indexColumn; c++) {
        if(next == std::string::npos) {
          _file.seekg(0, _file.beg);
          return false;
        }
        next = str.find(delimiter, prev);
        if(c == indexColumn)
          column->push_back(str.substr(prev, next-prev));
        prev = next + delta;
      }
      if(_file.tellg() == length)
          break;
    }
    _file.seekg(0, _file.beg);
    return true;
}

bool DqCSV::addRow(std::vector<std::string> *row)
{
    if(!_file.good())
      return false;
    
    std::string rowDelimiter = _rowDelimiter;
    std::string delimiter = _delimiter;
    
    _file.seekg(0, _file.end);
    for(unsigned int i = 0; i < row->size(); i++) {
      if(i == (row->size() - 1))
        _file << row->at(i) << rowDelimiter;
      else
        _file << row->at(i) << delimiter;
    }
    _file.seekg(0, _file.beg);
    return true;
}

// private
bool DqCSV::fixFile(std::string path)
{
  // fix file
  char * buffer = new char [_rowDelimiter.size()];
  _file.open(path.c_str(), std::ios::in | std::ios::out);
  if(!_file.is_open())
    return false;
  _file.seekg(-1*_rowDelimiter.size(), _file.end);
  _file.read(buffer, _rowDelimiter.size());
  if(buffer != _rowDelimiter) {
    _file.seekg(0, _file.end);
   _file << _rowDelimiter.c_str();
  }
  _file.seekg(0, _file.beg);
  _file.close();
  delete buffer;
  //--- 
  return true;

}
//------------------------------------------------------------------------------
