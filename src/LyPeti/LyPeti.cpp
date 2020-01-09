#include "LyPeti.hpp"

/* Constructor, destructor. */
//------------------------------------------------------------------------------
LyPeti::LyPeti(std::string mod, std::map<std::string, std::string> params,
               std::vector<std::string> inputs,
               std::vector<std::string> outputs) {
  _mod = mod;
  _params = params;
  _inputs = inputs;
  _outputs = outputs;

  std::vector<std::string> parserArrayDelimiters;
  parserArrayDelimiters.push_back(this->findParam("sys&DqParser&AD&1", &_params));
  parserArrayDelimiters.push_back(this->findParam("sys&DqParser&AD&2", &_params));
  parserArrayDelimiters.push_back(this->findParam("sys&DqParser&AD&3", &_params));
  parserArrayDelimiters.push_back(this->findParam("sys&DqParser&AD&4", &_params));
  parserArrayDelimiters.push_back(this->findParam("sys&DqParser&AD&5", &_params));
  _parser.setArrayDelimiter(parserArrayDelimiters);
  
  _strDelim = this->findParam("sys&DqParser&AD&3", &_params);
  _logRunNumber = this->findParam("logNumber", &_params);  
  _comment = this->findParam("comment", &_params);
  
  std::string plusOutDir = this->findParam("dir", &_params);  
  std::string outName = this->findParam("fN", &_params);
  std::string defaultPath = _parser.filePath(_outputs.at(0));
  _rootFilePath = defaultPath + plusOutDir + "/" + outName;

}
LyPeti::~LyPeti() {}

/* Get functions. */
std::string LyPeti::getLog() {
  std::string result = _logSymbol + _rowDelimiter;
  result += _log;
  std::string sha = findParam("SHA", &_params);
  result += "Version(GitHub SHA): " + sha + _rowDelimiter;

  return result;
}
//------------------------------------------------------------------------------

/* Analysis params and input files */
std::vector<int> LyPeti::parsNumRuns(std::vector<std::string> files) 
{
  DqParser parser;
  std::vector<int> result;
  std::string name;
  int number = -1;
  int prevNumber = -1;
  for(unsigned int i = 0; i < files.size(); i++) {
    name = _parser.fileName(files.at(i));
    _parser.deleteFormat(&name);
		try {
    	files.at(i).find_last_of("_");
		}
		catch(std::invalid_argument& e) {
			continue;
		}
    std::size_t found = files.at(i).find_last_of("_");
    number = stoi(files.at(i).substr(found + 1));
    if(prevNumber != number)
      result.push_back(number);
    prevNumber = number;
  }
  return result;
}
int LyPeti::numRun(std::string file) 
{
  DqParser parser;
  int result;
  std::string name;
  name = _parser.fileName(file);
  _parser.deleteFormat(&name);
		try {
    	file.find_last_of("_");
		}
		catch(std::invalid_argument& e) {
			return 0;
		}
  std::size_t found = file.find_last_of("_");
  result = stoi(file.substr(found + 1));
  
  return result;
}
bool LyPeti::saveHists(int run, std::map<int, LyPetiAnalysis> *analysis, DqRoot *dataRoot,
                       std::map<std::string, DqTH1D> *TH1s, std::map<std::string, DqTH2D> *TH2s)
{
    for(auto profiles: *TH1s) {
    int chamberNum = -1;
    double value = run;
    std::string chamber = _parser.chamberName(profiles.first);
    bool isChamber = _parser.strToInt(&chamberNum, chamber); 
    if(isChamber) { 
      auto it = analysis->find(chamberNum);
      if(!(it == analysis->end()))
        value = analysis->find(chamberNum)->second.getValue(run);
    }

    std::string histName = _parser.histName(profiles.first);
      std::string path = Form("%s_%s/param_%.2f%srun_%d/chamber_%d", _comment.c_str(), _logRunNumber.c_str(), value, _strDelim.c_str(), run, chamberNum); 
    if(_comment == "webdcs")
      path = Form("param_%.2f%srun_%d/chamber_%d", value, _strDelim.c_str(), run, chamberNum); 
    std::string folder = "";
    bool isFolder = this->findParam((histName + "&path"), &folder, &_params);
    if(isFolder)
      path += "/" + folder;
    else 
      path += "/withoutPath";
    dataRoot->writeDqToCanvasCMS(path, &profiles.second, profiles.second.getFit(), false);
  }
  for(auto profiles: *TH2s) {
    int chamberNum = -1;
    double value = run;
    std::string chamber = _parser.chamberName(profiles.first);
    bool isChamber = _parser.strToInt(&chamberNum, chamber); 
    if(isChamber) { 
      auto it = analysis->find(chamberNum);
      if(!(it == analysis->end()))
        value = analysis->find(chamberNum)->second.getValue(run);
    }
    std::string histName = _parser.histName(profiles.first);
    std::string path = Form("%s_%s/param_%.2f%srun_%d/chamber_%d", _comment.c_str(), _logRunNumber.c_str(), value, _strDelim.c_str(), run, chamberNum); 
    if(_comment == "webdcs")
      path = Form("param_%.2f%srun_%d/chamber_%d", value, _strDelim.c_str(), run, chamberNum); 
    std::string folder = "";
    bool isFolder = this->findParam((histName + "&path"), &folder, &_params);
    if(isFolder)
      path += "/" + folder;
    else 
      path += "/withoutPath";

    dataRoot->writeDqToCanvasCMS(path, &profiles.second,  false);
  }
  
  return true;
}
bool LyPeti::saveGraphs(std::map<int, LyPetiAnalysis> *analysis, DqRoot *dataRoot,
                        std::map<std::string, DqTGE*> *TGEs)
{
  for(auto profiles: *TGEs) {
 	  profiles.second->sortByX(); 
    profiles.second->plot();
    std::string histName = _parser.histName(profiles.first);
    std::string path = _comment + "_" + _logRunNumber + "/analysis/chamber_" + _parser.chamberName(profiles.first); 
    if(_comment == "webdcs")
      path = "analysis/chamber_" + _parser.chamberName(profiles.first); 
    std::string folder = "";
    bool isFolder = this->findParam((histName + "&path"), &folder, &_params);
    if(folder == "print") {
      histName = "draw" + histName + ".root";
      profiles.second->SaveAs(histName.c_str());
      std::string line;
      std::vector<std::string> lines;
      profiles.second->print(&lines);
      //for(unsigned int i = 0; i < lines.size(); i++)
      //  std::cout << _comment + " " + lines.at(i) << std::endl;
      std::string pathFile = "p_"+ profiles.first + ".txt";
      //std::ifstream myfile(pathFile.c_str());
      //if(myfile.is_open()) {
      //  int ss = 0;
      //  while(getline(myfile, line)) {
      // //   std::cout << line;
      //    lines[ss] = _comment  + " " + lines[ss] + " " + line;
      //    ss+=1;
      //  }
      //  myfile.close();
      //}
      //else std::cout << "Unable to open file";
      //std::ofstream myfile2 (pathFile.c_str(), std::ios_base::out | std::ios_base::trunc);
      std::ofstream myfile2 (pathFile.c_str(), std::ios_base::out | std::ios_base::app);
      if(myfile2.is_open()) {
        myfile2 << _comment;
          for(unsigned int i = 0; i < lines.size(); i++)
            myfile2 << " " + lines.at(i);
          myfile2 << std::endl;
          myfile2.close();
      }
      else std::cout << "Unable to open file";
    } 
    if(isFolder)
      path += "/" + folder;
    else 
      path += "/withoutPath";
    dataRoot->writeDqToCanvasCMS(path, profiles.second, false);
    //delete profiles.second;
  }
  return true;
}
bool LyPeti::saveGraphs(std::map<int, LyPetiAnalysis> *analysis, DqRoot *dataRoot,
                        std::map<std::string, DqTGAE*> *TGAEs)
{
  for(auto profiles: *TGAEs) {
 	  profiles.second->sortByX(); 
    profiles.second->plot();
    std::string histName = _parser.histName(profiles.first);
    std::string path = _comment + "_" + _logRunNumber + "/analysis/chamber_" + _parser.chamberName(profiles.first); 
    if(_comment == "webdcs")
      path = "analysis/chamber_" + _parser.chamberName(profiles.first); 
    std::string folder = "";
    bool isFolder = this->findParam((histName + "&path"), &folder, &_params);
    if(folder == "print") {
      histName = "draw" + histName + ".root";
      profiles.second->SaveAs(histName.c_str());
      std::string line;
      std::vector<std::string> lines;
      profiles.second->print(&lines);
      //for(unsigned int i = 0; i < lines.size(); i++)
      //  std::cout << _comment + " " + lines.at(i) << std::endl;
      std::string pathFile = "p_"+ profiles.first + ".txt";
      //std::ifstream myfile(pathFile.c_str());
      //if(myfile.is_open()) {
      //  int ss = 0;
      //  while(getline(myfile, line)) {
      // //   std::cout << line;
      //    lines[ss] = _comment  + " " + lines[ss] + " " + line;
      //    ss+=1;
      //  }
      //  myfile.close();
      //}
      //else std::cout << "Unable to open file";
      //std::ofstream myfile2 (pathFile.c_str(), std::ios_base::out | std::ios_base::trunc);
      std::ofstream myfile2 (pathFile.c_str(), std::ios_base::out | std::ios_base::app);
      if(myfile2.is_open()) {
        myfile2 << _comment;
          for(unsigned int i = 0; i < lines.size(); i++)
            myfile2 << " " + lines.at(i);
          myfile2 << std::endl;
          myfile2.close();
      }
      else std::cout << "Unable to open file";
    } 
    if(isFolder)
      path += "/" + folder;
    else 
      path += "/withoutPath";
    dataRoot->writeDqToCanvasCMS(path, profiles.second, false);
    //delete profiles.second;
  }
  return true;
}
bool LyPeti::saveGraphs(std::map<int, LyPetiAnalysis> *analysis, DqRoot *dataRoot,
                        std::map<std::string, DqMTGE*> *MTGEs)
{
  for(auto profiles: *MTGEs) {
    std::string histName = _parser.histName(profiles.first);
    std::string path = _comment + "_" + _logRunNumber + "/analysis/chamber_" + _parser.chamberName(profiles.first); 
    if(_comment == "webdcs")
      path = "analysis/chamber_" + _parser.chamberName(profiles.first); 
    std::string folder = "";
    bool isFolder = this->findParam((histName + "&path"), &folder, &_params);
    if(isFolder)
      path += "/" + folder;
    else 
      path += "/withoutPath";
    dataRoot->writeDqToCanvasCMS(path, profiles.second, false);
  }
  return true;
}
bool LyPeti::saveGraphs(std::map<int, LyPetiAnalysis> *analysis, DqRoot *dataRoot,
                        std::map<std::string, DqTG2D*> *TG2Ds)
{
  for(auto profiles: *TG2Ds) {
    profiles.second->plot();
    std::string histName = _parser.histName(profiles.first);
    std::string path = _comment + "_" + _logRunNumber + "/analysis/chamber_" + _parser.chamberName(profiles.first); 
    if(_comment == "webdcs")
      path = "analysis/chamber_" + _parser.chamberName(profiles.first); 
    std::string folder = "";
    bool isFolder = this->findParam((histName + "&path"), &folder, &_params);
    if(isFolder)
      path += "/" + folder;
    else 
      path += "/withoutPath";
    dataRoot->writeDqToCanvasCMS(path, profiles.second, true);
  }
  return true;
}
//------------------------------------------------------------------------------
bool LyPeti::initEvent(LyPetiEvent *event)
{
  // PARSING CARD PARAMETORS ---------------------------------------------------
  
  std::vector<std::string> parserArrayDelimiters;
  parserArrayDelimiters.push_back(this->findParam("sys&DqParser&AD&1", &_params));
  parserArrayDelimiters.push_back(this->findParam("sys&DqParser&AD&2", &_params));
  parserArrayDelimiters.push_back(this->findParam("sys&DqParser&AD&3", &_params));
  parserArrayDelimiters.push_back(this->findParam("sys&DqParser&AD&4", &_params));
  parserArrayDelimiters.push_back(this->findParam("sys&DqParser&AD&5", &_params));
  
  std::string sStrips = this->findParam("strips", &_params);
  std::vector<std::string> vsStrips = _parser.parsArray(sStrips);
  std::vector<int> strips = _parser.strToIntArray(vsStrips);
  sStrips.clear(); vsStrips.clear();
  
  std::string sTrigCh = this->findParam("trigCh", &_params);
  std::vector<std::string> vsTrigCh = _parser.parsArray(sTrigCh);
  std::vector<double> trigCh = _parser.strToDoubleArray(vsTrigCh);
  sTrigCh.clear(); vsTrigCh.clear();
  
  std::string sHR = this->findParam("HR", &_params);
  std::vector<std::string> vsHR = _parser.parsArray(sHR);
  std::vector<double> HR = _parser.strToDoubleArray(vsHR);
  sHR.clear(); vsHR.clear();
  
  std::string sLR = this->findParam("LR", &_params);
  std::vector<std::string> vsLR = _parser.parsArray(sLR);
  std::vector<double> LR = _parser.strToDoubleArray(vsLR);
  sLR.clear(); vsLR.clear();
  
  std::string sTDCs = this->findParam("TDCs", &_params);
  std::vector<std::string> vsTDCs = _parser.parsArray(sTDCs);
  std::vector<double> TDCs = _parser.strToDoubleArray(vsTDCs);
  sTDCs.clear(); vsTDCs.clear();
  
  std::string sTimeOffSet = this->findParam("timeOffSet", &_params);
  std::vector<std::string> vsTimeOffSet = _parser.parsArray(sTimeOffSet);
  std::vector<double> timeOffSet = _parser.strToDoubleArray(vsTimeOffSet);
  sTimeOffSet.clear(); vsTimeOffSet.clear();
  
  std::string sTimeOffSetHR = this->findParam("timeOffSetHR", &_params);
  std::vector<std::string> vsTimeOffSetHR = _parser.parsArray(sTimeOffSetHR);
  std::vector<double> timeOffSetHR = _parser.strToDoubleArray(vsTimeOffSetHR);
  sTimeOffSetHR.clear(); vsTimeOffSetHR.clear();
 
  std::string sTimeOffSetLR = this->findParam("timeOffSetLR", &_params);
  std::vector<std::string> vsTimeOffSetLR = _parser.parsArray(sTimeOffSetLR);
  std::vector<double> timeOffSetLR = _parser.strToDoubleArray(vsTimeOffSetLR);
  sTimeOffSetLR.clear(); vsTimeOffSetLR.clear();
  
  std::string sDeadHR = this->findParam("deadHR", &_params);
  std::vector<std::string> vsDeadHR = _parser.parsArray(sDeadHR);
  std::vector<double> deadHR = _parser.strToDoubleArray(vsDeadHR);
  sDeadHR.clear(); vsDeadHR.clear();
  
  std::string sDeadLR = this->findParam("deadLR", &_params);
  std::vector<std::string> vsDeadLR = _parser.parsArray(sDeadLR);
  std::vector<double> deadLR = _parser.strToDoubleArray(vsDeadLR);
  sDeadLR.clear(); vsDeadLR.clear();
  
  std::string sChambers = this->findParam("chambers", &_params);
  std::vector<std::string> vsChambers = _parser.parsArray(sChambers);
  std::vector<double> chambers = _parser.strToDoubleArray(vsChambers);
  sChambers.clear(); vsChambers.clear(); 
  
  // ---------------------------------------------------------------------------
	
  // ANALYSIS INPUT PARAMETORS -------------------------------------------------
	// fill maping 
  bool isMap = false;
  isMap = event->mapping(strips, trigCh, TDCs, 
                HR, timeOffSetHR, 
                LR, timeOffSetLR,
                timeOffSet,
                deadHR, deadLR,
                chambers); // inside check map arrays.
  if(!isMap) {
      _log += " ERROR! Map isn't correct, Size of arrays are not equal!" + _rowDelimiter
					 + "strips=" + std::to_string(strips.size()) + parserArrayDelimiters.at(0) 
					 + "TDCs=" + std::to_string(TDCs.size()) + parserArrayDelimiters.at(0) 
					 + "trigCh=" + std::to_string(trigCh.size()) + parserArrayDelimiters.at(0)
					 + "HR=" + std::to_string(HR.size()) + parserArrayDelimiters.at(0)
					 + "LR=" + std::to_string(LR.size()) + parserArrayDelimiters.at(0)
					 + "timeOffSetHR=" + std::to_string(timeOffSetHR.size()) + parserArrayDelimiters.at(0)
					 + "timeOffSetLR=" + std::to_string(timeOffSetLR.size()) + parserArrayDelimiters.at(0)
					 + "timeOffSet=" + std::to_string(timeOffSet.size()) + parserArrayDelimiters.at(0)
					 + "deadHR=" + std::to_string(deadHR.size()) + parserArrayDelimiters.at(0)
					 + "deadLR=" + std::to_string(deadLR.size()) + parserArrayDelimiters.at(0)
					 + "asciiChambers=" + std::to_string(chambers.size()) + _rowDelimiter;
      std::cout << "ERROR with map!" << std::endl;
      return false;
  }
  
  strips.clear(); trigCh.clear(); TDCs.clear(); chambers.clear(); 
  HR.clear(); timeOffSetHR.clear(); LR.clear(); timeOffSetLR.clear(); timeOffSet.clear(); deadHR.clear(); deadLR.clear();  // clear memory (map params) 
  
  // print output
  std::cout << "#---\n" << std::endl;
	std::cout  << "Getting ingredients for cookies..." << std::endl;
  std::cout << "\n#---" << std::endl;
	std::cout  << "N: " << event->chambers()->size() << " chamber(s) in Processing:";
	for(unsigned int i = 0; i < event->chambers()->size(); i++)
		std::cout << " index=" << event->chambers()->at(i);
	std::cout << std::endl;
  
  return true;
}
std::vector<int> LyPeti::initRuns()
{
  // USING GLOBAL VARIBLS
  // Runs Info
  std::vector<int> runs;
  std::string sRuns = this->findParam("runs", &_params);
  std::vector<std::string> vsRuns = _parser.parsArray(sRuns);
  runs = _parser.strToIntArray(vsRuns);
  sRuns.clear(); vsRuns.clear();
  
  // COMAND LINE ---------------------------------------------------------------
  if(runs.size() > 0) {
    int processingFiles = 0;

    for(unsigned int r = 0; r < runs.size(); r++) {
	    for(unsigned int i = 0; i < _inputs.size(); i++) {
        if(runs.at(r) == this->numRun(_parser.fileName(_inputs.at(i))))
          processingFiles += 1;
      }
    }
    if(processingFiles > 0)
      std::cout <<"N: " << processingFiles << " file(s) in Processing... / ";
    else {
      _log += " ERROR! Processing file(s): " 
	  			 + std::to_string(processingFiles)  + "!" + _rowDelimiter;
      return runs;
    }
  }
  else {  
    runs.clear();
  	runs = this->parsNumRuns(_inputs);
    std::cout <<"N: " << _inputs.size() << " file(s) in Processing... / ";
  }
  std::cout <<"N: " << runs.size() << " run(s) in Processing..." << std::endl;
  
  return runs;
}

std::vector<std::pair<int, int>> LyPeti::initOffSetRuns(std::vector<int> *runs,  LyPetiEvent *event)
{
  std::vector<std::pair<int, int>> results;
  for(unsigned int i = 0; i < runs->size(); i++) {
    bool isChamber = false;
    for(unsigned int i = 0; i < event->chambers()->size(); i++) {
      int chamber = event->chambers()->at(i); 
      std::string sOffsetRun;
      int offsetRun = 0;
      bool isOffsetRun = this->findParam((std::to_string(chamber) + _strDelim + "offsetRun"), &sOffsetRun, &_params);
      if(isOffsetRun) {
        offsetRun = std::atoi(sOffsetRun.c_str());
        if(runs->at(i) == offsetRun) {
          results.push_back(std::make_pair(runs->at(i), chamber));
          isChamber = true;
        }
      }
    }
    if(!isChamber) {
      std::string sOffsetRun;
      bool isOffsetRunDef = this->findParam("offsetRun", &sOffsetRun, &_params);
      if(isOffsetRunDef) {
        int offsetRun = std::atoi(sOffsetRun.c_str());
        if(runs->at(i) == offsetRun)
          results.push_back(std::make_pair(runs->at(i), 1));
        else
          results.push_back(std::make_pair(runs->at(i), 0));
      }
    }
  }
  for(unsigned int i = 0; i < results.size(); i++) {
    if(results.at(i).second > 0)
      results.push_back(std::make_pair(results.at(i).first, 0));
  }
  std::sort(results.begin(), results.end(), [](auto &left, auto &right) {
          return left.first < right.first;
          });
  std::sort(results.begin(), results.end(), [](auto &left, auto &right) {
          return left.second > right.second;
          });
  return results;
}

bool LyPeti::initAnalys(std::map<int, LyPetiAnalysis> *analysis, 
                        LyPetiEvent *event,  std::vector<int> runs)
{

  std::string strDelim = this->findParam("sys&DqParser&AD&3", &_params);
  // ANALYSIS CLASS SETTING ----------------------------------------------------	
  for(unsigned int i = 0; i < event->chambers()->size(); i++) {
    int chamber = event->chambers()->at(i); 
    auto itA = analysis->find(chamber);
    if(itA == analysis->end())
      analysis->insert(std::make_pair(chamber, LyPetiAnalysis{}));

    analysis->find(chamber)->second.setParser(&_parser);
    analysis->find(chamber)->second.setDeadCHsHR(event->getDeadCHsHR());
    analysis->find(chamber)->second.setDeadCHsLR(event->getDeadCHsLR());
    
    // take values for runs 
    std::vector<double> values; 
    std::string sValues; 
    bool isValues = this->findParam((std::to_string(chamber) + strDelim + "values"), &sValues, &_params);
    if(isValues) {
      std::vector<std::string> vsValues = _parser.parsArray(sValues);
      values = _parser.strToDoubleArray(vsValues);
      sValues.clear(); vsValues.clear();
    }
    else {
      bool isValuesDef = this->findParam("values", &sValues, &_params);
      if(isValuesDef) {
    	  _log += " INFO: Chamber:  " + std::to_string(chamber) + " is using values !" + _rowDelimiter;
        std::vector<std::string> vsValues = _parser.parsArray(sValues);
        values = _parser.strToDoubleArray(vsValues);
        sValues.clear(); vsValues.clear();
      }
    }
	 	
		// take filters params
    std::map<std::string, double> algoParams; 
 		for(auto& itP: _params) {
			std::string chamberName = _parser.chamberName(itP.first); // first param before delim
			bool isAlgo = _parser.findParamFromStr("algo", itP.first);
			bool isFilter = _parser.findParamFromStr("filter", itP.first);
			if(chamberName == std::to_string(chamber) && (isFilter || isAlgo)) {
        double value = 0;
				bool isValue = _parser.strToDouble(&value, itP.second);
				if(isValue)
      	 	algoParams.insert(std::make_pair(_parser.histName(itP.first), value));
			}
			if((isAlgo || isFilter) && (chamberName == "filter" || chamberName == "algo") ) {
    		_log += " INFO: Param for chamber: " + std::to_string(chamber)  + " is " + itP.first + "." + _rowDelimiter;
  			double value = 0;
				bool isValue = _parser.strToDouble(&value, itP.second);
				if(isValue)
      	 	algoParams.insert(std::make_pair(_parser.histName(itP.first), value));
			}
		}
    analysis->find(chamber)->second.setRuns(runs, values);
    analysis->find(chamber)->second.setParams(algoParams);
    values.clear(); algoParams.clear();
    bool isAnalysis = analysis->find(chamber)->second.configure();
    
    if(!isAnalysis) {
    	_log += " ERROR: Can't configure analysis for chamber: " + std::to_string(chamber) + "!" + _rowDelimiter;
			return false;
		}
  }
  return true;
}

bool LyPeti::offset(std::string mod, std::vector<std::string> *mods, 
                  int run, LyPetiEvent *event, std::map<int, LyPetiAnalysis> *analysis,
                  DqRoot *dataRoot,
                  std::map<std::string, DqTGE*> *TGEs, 
                  std::map<std::string, DqTH1D> *TH1s)
{
  std::string nameTGEs = ""; std::string titleTGEs = ""; auto itTGEs = TGEs->find("");
  std::string nameTH1s = ""; std::string titleTH1s = ""; auto itTH1s = TH1s->find("");
  std::map<int, double> offsetsHR; std::map<int, double> offsetsLR;
  if(mod ==  "OFFSET" + _strDelim + "CONFIG" || mod ==  "OFFSET" + _strDelim + "ANALYSIS") {
    bool isA = true; if(mod ==  "OFFSET" + _strDelim + "CONFIG") isA = false; 
    // FIRST ANALYSIS BEGIN ------------------------------------------------
    std::string strValue; double buffer = 6; 
    bool isBuffer = this->findParam("offsetFit&buffer", &strValue, &_params);
		if(isBuffer) _parser.strToDouble(&buffer, strValue);
    for(unsigned int ic = 0; ic < event->chambers()->size(); ic++) {
      int chamber = event->chambers()->at(ic);
      analysis->find(chamber)->second.setStrips(event->strips(chamber));
      //---
      for(unsigned int jt = 0; jt < event->trigs(chamber)->size(); jt++) {
        //std::cout << event->trigs(chamber)->at(jt)->head->boardID << std::endl;
        //if(event->trigs(chamber)->at(jt)->head->numOfCh > buffer && event->trigs(chamber)->at(jt)->head->boardID == 15) analysis->find(chamber)->second.skipEvent(true);
      }
      //--- 
      unsigned int numberOfTrigers = event->trigs(chamber)->size(); // #
      bool isTrigers = analysis->find(chamber)->second.fillTrigers(numberOfTrigers);
    	if(isTrigers) {
        for(unsigned int js = 0; js < event->strips(chamber)->size(); js++) { // Fill raw data with maping. 
          for(unsigned int jh = 0; jh < event->strips(chamber)->at(js).HR.size(); jh++) {
            bool isFilterHR = analysis->find(chamber)->second.filters("data_HR", js, jh); 
            if(isFilterHR)  analysis->find(chamber)->second.fillEvents("data_HR", js, jh); 
          }
          
          for(unsigned int jl = 0; jl < event->strips(chamber)->at(js).LR.size(); jl++) {
            bool isFilterLR = analysis->find(chamber)->second.filters("data_LR", js, jl); 
            if(isFilterLR) analysis->find(chamber)->second.fillEvents("data_LR", js, jl); 
     	  	}
    	  }
      }
      // ANALYSIS PART
      std::string filters = analysis->find(chamber)->second.filtersStr(); 
      for(unsigned int im = 0; im < mods->size(); im++) {
        if(!analysis->find(chamber)->second.skipEvent()) {  
          analysis->find(chamber)->second.algos(mods->at(im));
          std::string modParams = analysis->find(chamber)->second.modParamsStr(mods->at(im)); 
          std::vector<std::pair<double, double>> data = analysis->find(chamber)->second.data(mods->at(im).c_str());
          for(unsigned int jd = 0; jd < data.size(); jd++) { 
            nameTH1s = Form("%d%s%d%s%sTimeProfileOffset", chamber, _strDelim.c_str(), (int)data.at(jd).first, _strDelim.c_str(), mods->at(im).c_str()); 
            itTH1s = dataRoot->addDqTH1D(nameTH1s, nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params); 
            if(!isA) itTH1s->second.setting(data.at(jd).second); 
            if(isA) { itTH1s = TH1s->find(nameTH1s); if(!(itTH1s == TH1s->end())) 
              TH1s->find(nameTH1s.c_str())->second.Fill(data.at(jd).second); }
          }
        }
      }
    }
  }
  if(mod ==  "OFFSET" + _strDelim + "CONFIG END") {
    std::string name;
    for(unsigned int ic = 0; ic < event->chambers()->size(); ic++) {
      int chamber = event->chambers()->at(ic);
      std::string filters = analysis->find(chamber)->second.filtersStr(); 
      for(auto& it: *TH1s) {
        for(unsigned int im = 0; im < mods->size(); im++) {
          std::string modParams = analysis->find(chamber)->second.modParamsStr(mods->at(im)); 
          if(_parser.last(it.second.GetTitle()) == _parser.last((mods->at(im).c_str()))) {
            name = Form("%s%s%s%s%s", it.second.GetName(), _strDelim.c_str(), modParams.c_str(), _strDelim.c_str(), filters.c_str()); 
            it.second.SetTitle(name.c_str()); 
          }
        }
        it.second.setting();
      }
    }
  }
  if(mod ==  "OFFSET" + _strDelim + "ANALYSIS END") {
    
    std::string strValue;
    double nRMS = 3; double entries = 100;
    bool isRMS = this->findParam("offsetFit&nRMS", &strValue, &_params);
		if(isRMS) _parser.strToDouble(&nRMS, strValue);
    bool isEntries = this->findParam("offsetFit&entries", &strValue, &_params);
		if(isEntries) _parser.strToDouble(&entries, strValue);
    
    for(unsigned int ic = 0; ic < event->chambers()->size(); ic++) {
      int chamber = event->chambers()->at(ic); 
      std::string filters = analysis->find(chamber)->second.filtersStr(); 
      for(unsigned int im = 0; im < mods->size(); im++) {
        analysis->find(chamber)->second.algos(mods->at(im));
        std::string modParams = analysis->find(chamber)->second.modParamsStr(mods->at(im)); 
        double offSet = 0; double errOffSet = 0;
        double value = analysis->find(chamber)->second.getValue(run); 
        int strips = event->numberOfStrips(chamber);
	    	nameTGEs = Form("%d%s%s_%d_%soffSet", chamber, _strDelim.c_str(), mods->at(im).c_str(), (int)value, _strDelim.c_str());   
        itTGEs = dataRoot->addDqTGE(nameTGEs, nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
        double refTime = 0; double errRefTime = 0; int prevEntries = 0; int refStrip = -1;
        for(int c = 0; c < strips; c++) {
          nameTH1s = Form("%d%s%d%s%sTimeProfileOffset", chamber, _strDelim.c_str(), c, _strDelim.c_str(), mods->at(im).c_str()); 
          itTH1s = TH1s->find(nameTH1s.c_str()); if(!(itTH1s == TH1s->end())) {
            if(TH1s->find(nameTH1s.c_str())->second.GetEntries() > prevEntries) {
              prevEntries = TH1s->find(nameTH1s.c_str())->second.GetEntries();
              refStrip = c;
            }
          }
        }
        if(refStrip != -1) {
          nameTH1s = Form("%d%s%d%s%sTimeProfileOffset", chamber, _strDelim.c_str(), refStrip, _strDelim.c_str(), mods->at(im).c_str()); 
          double mean = TH1s->find(nameTH1s.c_str())->second.GetMean(); double rms = TH1s->find(nameTH1s.c_str())->second.GetRMS();
          TF1 *f1 = new TF1("f1", "gaus", mean-nRMS*rms, mean+nRMS*rms);
          TH1s->find(nameTH1s.c_str())->second.Fit("f1", "R");
          refTime = f1->GetParameter(1); errRefTime = f1->GetParError(1);
          delete f1;
        }
        double averSigma = 0; int an = 0;
        
        for(int c = 0; c < strips; c++) {
          nameTH1s = Form("%d%s%d%s%sTimeProfileOffset", chamber, _strDelim.c_str(), c, _strDelim.c_str(), mods->at(im).c_str()); 
          auto itStrip = TH1s->find(nameTH1s.c_str());
          if(!(itStrip == TH1s->end())) {
            if(TH1s->find(nameTH1s.c_str())->second.GetEntries() > entries) {
              double mean = TH1s->find(nameTH1s.c_str())->second.GetMean(); double rms = TH1s->find(nameTH1s.c_str())->second.GetRMS();
              TF1 *f1 = new TF1("f1", "gaus", mean-nRMS*rms, mean+nRMS*rms);
              TH1s->find(nameTH1s.c_str())->second.Fit("f1", "R");
              offSet = f1->GetParameter(1)-refTime; 
              if(std::abs(offSet) > 10) offSet = 0;
              errOffSet = f1->GetParError(1)+errRefTime; 
              TGEs->find(nameTGEs.c_str())->second->setting(c + chamber, 0, offSet, errOffSet);
              if( _parser.last(mods->at(im)) == "CB") {
                averSigma += f1->GetParameter(2);
                an +=1;
              }
              if(mods->at(im) == "HR") { 
                offsetsHR.insert(std::make_pair(c+chamber, offSet));
              }
              if(mods->at(im) == "LR") 
                offsetsLR.insert(std::make_pair(c+chamber, offSet));
              delete f1;
            }
          }
        }
        double RMS = 0;
        for(int c = 0; c < strips; c++) {
          nameTH1s = Form("%d%s%d%s%sTimeProfileOffset", chamber, _strDelim.c_str(), c, _strDelim.c_str(), mods->at(im).c_str()); 
          auto itStrip = TH1s->find(nameTH1s.c_str());
          if(!(itStrip == TH1s->end())) {
            if(TH1s->find(nameTH1s.c_str())->second.GetEntries() > entries) {
              double mean = TH1s->find(nameTH1s.c_str())->second.GetMean(); double rms = TH1s->find(nameTH1s.c_str())->second.GetRMS();
              TF1 *f1 = new TF1("f1", "gaus", mean-nRMS*rms, mean+nRMS*rms);
              TH1s->find(nameTH1s.c_str())->second.Fit("f1", "R");
              offSet = f1->GetParameter(1)-refTime; 
              if(std::abs(offSet) > 10) offSet = 0;
              errOffSet = f1->GetParError(1)+errRefTime; 
              TGEs->find(nameTGEs.c_str())->second->setting(c + chamber, 0, offSet, errOffSet);
              if( _parser.last(mods->at(im)) == "CB") {
                RMS += (f1->GetParameter(2) - averSigma/an)*(f1->GetParameter(2) - averSigma/an);
              }
            }
          }
        }
        std::cout << "average sigma cluster: " << std::setprecision(5) << averSigma/an << " " << std::sqrt(RMS)/an  << std::endl; 
      }
    }
    event->setTimeOffSetHR(&offsetsHR); 
    event->setTimeOffSetLR(&offsetsLR);

    std::map<int, double> printHR; std::map<int, double> printLR;
    printHR = event->getTimeOffSetHR(); 
    printLR = event->getTimeOffSetLR();
    double print = 0;
    bool isPrint = this->findParam("offsetFit&print", &strValue, &_params);
		if(isPrint)  _parser.strToDouble(&print, strValue);
    if(print == 1) { 
      std::string delim = this->findParam("sys&DqParser&AD&1", &_params);
      for(unsigned int ic = 0; ic < event->chambers()->size(); ic++) {
        int chamber = event->chambers()->at(ic); 
        int strips = event->numberOfStrips(chamber);
        std::cout << "\n" << "chamber: " << chamber << "; HR\n";
        for(int i = 0; i < strips; i++) {
          auto itStrip = printHR.find(i+chamber);
            if(!(itStrip == printHR.end())) {
              std::cout << std::setprecision(5) << printHR.find(i+chamber)->second << delim;
            }
            else
              std::cout << 0 << delim;
        }
        std::cout << "\n" << "chamber: " << chamber << "; LR\n";
        for(int i = 0; i < strips; i++) {
          auto itStrip = printLR.find(i+chamber);
            if(!(itStrip == printLR.end())) {
              std::cout << std::setprecision(5) <<  printLR.find(i+chamber)->second << delim;
            }
            else
              std::cout << 0 << delim;
        }
      }
    }
  }
  else
    return false;
  return true;
}
bool LyPeti::core(std::string mod, std::vector<std::string> *mods, 
                  int run, LyPetiEvent *event, 
                  std::map<int, RoPetiEvent*> *roEvents,
                  std::map<int, LyPetiAnalysis> *analysis,
                  DqRoot *dataRoot, 	
                  std::map<std::string, DqTGE*> *TGEs, 
                  std::map<std::string, DqTGAE*> *TGAEs, 
                  std::map<std::string, DqTG2D*> *TG2Ds, 
                  std::map<std::string, DqTH1D> *TH1s, 
                  std::map<std::string, DqTH2D> *TH2s)
{
  std::string nameTGEs = ""; std::string titleTGEs = ""; auto itTGEs = TGEs->find("");
  std::string nameTGAEs = ""; std::string titleTGAEs = ""; auto itTGAEs = TGAEs->find("");
  std::string nameTG2Ds = ""; std::string titleTG2Ds = ""; auto itTG2Ds = TG2Ds->find("");
  std::string nameTH1s = ""; std::string titleTH1s = ""; auto itTH1s = TH1s->find("");
  std::string nameTH2s = ""; std::string titleTH2s = ""; auto itTH2s = TH2s->find("");
  if(mod == "CONFIG" || mod == "ANALYSIS") {
    bool isA = true; if(mod == "CONFIG") isA = false; 
    //  ANALYSIS BEGIN  --------------------------------------------------------
    for(unsigned int ic = 0; ic < event->chambers()->size(); ic++) {
      int chamber = event->chambers()->at(ic); // #
      analysis->find(chamber)->second.setStrips(event->strips(chamber));
        
      // TRIGS
      std::vector<double> trigers;
      for(unsigned int jt = 0; jt < event->trigs(chamber)->size(); jt++) {
        
        trigers.push_back(event->trigs(chamber)->at(jt)->time);
       
        int boardID = event->trigs(chamber)->at(jt)->head->boardID;
        if(isA) {
          int gtc = event->trigs(chamber)->at(jt)->head->gtc; 
          int ch = event->trigs(chamber)->at(jt)->ch; 
          double time = event->trigs(chamber)->at(jt)->time; 
          double fine = event->trigs(chamber)->at(jt)->fine; 
          double coarse = event->trigs(chamber)->at(jt)->coarse; 
          roEvents->find(chamber)->second->addTrig(boardID, gtc, ch, time, fine, coarse);
        }
        
        int numOfCh = event->trigs(chamber)->at(jt)->head->numOfCh;
        nameTH1s = Form("%d%snum%d%sbuffer", chamber, _strDelim.c_str(), boardID, _strDelim.c_str());
        itTH1s = dataRoot->addDqTH1D(nameTH1s, nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params); 
        if(!isA) itTH1s->second.setting(numOfCh); 
        if(isA) { itTH1s = TH1s->find(nameTH1s); if(!(itTH1s == TH1s->end())) 
          TH1s->find(nameTH1s.c_str())->second.Fill(numOfCh); }
        //---
        int BCID = event->trigs(chamber)->at(jt)->BCID; 
        nameTH1s = Form("%d%snum%d%strigBCID", chamber, _strDelim.c_str(), boardID,_strDelim.c_str());
        itTH1s = dataRoot->addDqTH1D(nameTH1s, nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
        if(!isA) itTH1s->second.setting(BCID);
        if(isA) { itTH1s = TH1s->find(nameTH1s); if(!(itTH1s == TH1s->end())) 
          TH1s->find(nameTH1s.c_str())->second.Fill(BCID); }
        //---
      }
      unsigned int numberOfTrigers = event->trigs(chamber)->size(); // #
      nameTH1s = Form("%d%sNumberOfTrigers", chamber, _strDelim.c_str()); 
      itTH1s = dataRoot->addDqTH1D(nameTH1s, nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
      if(!isA) itTH1s->second.setting(numberOfTrigers);
      if(isA) { itTH1s = TH1s->find(nameTH1s); if(!(itTH1s == TH1s->end()))
        TH1s->find(nameTH1s.c_str())->second.Fill(numberOfTrigers); }
      //--- 
      
      bool isTrigers = analysis->find(chamber)->second.fillTrigers(trigers);
      for(unsigned int js = 0; js < event->strips(chamber)->size(); js++) {
        // HR
        double numberOfHitsHR = event->strips(chamber)->at(js).HR.size(); 
        nameTH1s = Form("%d%sNumberOfHitsHR", chamber, _strDelim.c_str()); 
        itTH1s = dataRoot->addDqTH1D(nameTH1s, nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
        if(!isA) itTH1s->second.setting(numberOfHitsHR);
        if(isA) { itTH1s = TH1s->find(nameTH1s); if(!(itTH1s == TH1s->end())) 
          TH1s->find(nameTH1s.c_str())->second.Fill(numberOfHitsHR); }
        //---
        for(unsigned int jh = 0; jh < event->strips(chamber)->at(js).HR.size(); jh++) { // Filling data for HR
          
          if(isA) {
            int boardID = event->strips(chamber)->at(js).HR.at(jh)->head->boardID; 
            int gtc = event->strips(chamber)->at(js).HR.at(jh)->head->gtc; 
            int strip = event->strips(chamber)->at(js).number;
            int ch = event->strips(chamber)->at(js).HR.at(jh)->ch; 
            double time = event->strips(chamber)->at(js).HR.at(jh)->time; 
            double fine = event->strips(chamber)->at(js).HR.at(jh)->fine; 
            double coarse = event->strips(chamber)->at(js).HR.at(jh)->coarse; 
            int isHR = 1; int isLR = 0; 
            roEvents->find(chamber)->second->addHit(boardID, gtc, ch, strip, time, fine, coarse, isHR, isLR);
          }
    	    
          if(isTrigers) {
            bool isFilterHR = analysis->find(chamber)->second.filters("data_HR", js, jh); 
            if(isFilterHR) 
              analysis->find(chamber)->second.fillEvents("data_HR", js, jh); 
            bool isFilterNoiseHR = analysis->find(chamber)->second.filters("noise_HR", js, jh); 
            if(isFilterNoiseHR) 
              analysis->find(chamber)->second.fillEvents("noise_HR", js, jh); 
            //analysis->find(chamber)->second.filters("deadTime_HR", js, jh); // skip if deadTime 
          }
        }
      	// LR
        double numberOfHitsLR = event->strips(chamber)->at(js).LR.size(); 
        nameTH1s = Form("%d%sNumberOfHitsLR", chamber, _strDelim.c_str()); 
        itTH1s = dataRoot->addDqTH1D(nameTH1s, nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
        if(!isA) itTH1s->second.setting(numberOfHitsLR);
        if(isA) { itTH1s = TH1s->find(nameTH1s); if(!(itTH1s == TH1s->end())) 
          TH1s->find(nameTH1s.c_str())->second.Fill(numberOfHitsLR); }
        //---
        for(unsigned int jl = 0; jl < event->strips(chamber)->at(js).LR.size(); jl++) { // Filling data for LR
          
          if(isA) {
            int boardID = event->strips(chamber)->at(js).LR.at(jl)->head->boardID; 
            int gtc = event->strips(chamber)->at(js).LR.at(jl)->head->gtc; 
            int strip = event->strips(chamber)->at(js).number;
            int ch = event->strips(chamber)->at(js).LR.at(jl)->ch; 
            double time = event->strips(chamber)->at(js).LR.at(jl)->time; 
            double fine = event->strips(chamber)->at(js).LR.at(jl)->fine; 
            double coarse = event->strips(chamber)->at(js).LR.at(jl)->coarse; 
            int isHR = 0; int isLR = 1; 
            roEvents->find(chamber)->second->addHit(boardID, gtc, ch, strip, time, fine, coarse, isHR, isLR);
          }
    	    
          if(isTrigers) {
            bool isFilterLR = analysis->find(chamber)->second.filters("data_LR", js, jl); 
            if(isFilterLR)
              analysis->find(chamber)->second.fillEvents("data_LR", js, jl); 
            bool isFilterNoiseLR = analysis->find(chamber)->second.filters("noise_LR", js, jl); 
            if(isFilterNoiseLR) 
              analysis->find(chamber)->second.fillEvents("noise_LR", js, jl); 
            //analysis->find(chamber)->second.filters("deadTime_LR", js, jl); // skip if deadTime 
          }
        }
    	}

      std::string filters = analysis->find(chamber)->second.filtersStr(); 


      for(unsigned int im = 0; im < mods->size(); im++) {
        analysis->find(chamber)->second.algos(mods->at(im));
        if(!analysis->find(chamber)->second.skipEvent()) {  // ANALYSIS PART
          std::string modParams = analysis->find(chamber)->second.modParamsStr(mods->at(im)); 
        
					std::vector<std::pair<double, double>> data = analysis->find(chamber)->second.data(mods->at(im).c_str());
          for(unsigned int jd = 0; jd < data.size(); jd++) { 
            
            nameTH1s = Form("%d%s%sstripProfile", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
            titleTH1s = Form("%d%s%s", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
            itTH1s = dataRoot->addDqTH1D(nameTH1s, titleTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
            if(!isA) itTH1s->second.setting(data.at(jd).first);
            if(isA) { itTH1s = TH1s->find(nameTH1s); if(!(itTH1s == TH1s->end())) 
              TH1s->find(nameTH1s.c_str())->second.Fill(data.at(jd).first); }
            //---
            nameTH1s = Form("%d%s%stimeProfile", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
            titleTH1s = Form("%d%s%s", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
            itTH1s = dataRoot->addDqTH1D(nameTH1s, titleTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
            if(!isA) itTH1s->second.setting(data.at(jd).second);
            if(isA) { itTH1s = TH1s->find(nameTH1s); if(!(itTH1s == TH1s->end())) 
              TH1s->find(nameTH1s.c_str())->second.Fill(data.at(jd).second); }
            //--- 
            nameTH2s = Form("%d%s%sstripTimeProfile", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
            titleTH2s = Form("%d%s%s", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
            itTH2s = dataRoot->addDqTH2D(nameTH2s, titleTH2s, TH2s); itTH2s->second.config(nameTH2s, &_params);
            if(!isA) itTH2s->second.setting(data.at(jd).second, data.at(jd).first);
            if(isA) { itTH2s = TH2s->find(nameTH2s); if(!(itTH2s == TH2s->end())) 
              TH2s->find(nameTH2s.c_str())->second.Fill(data.at(jd).second, data.at(jd).first); }
            //--- 
            nameTH1s = Form("%d%s%d%s%sTimeProfile", chamber, _strDelim.c_str(), (int)data.at(jd).first, _strDelim.c_str(), mods->at(im).c_str()); 
            titleTH1s = Form("%d%s%s", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
            itTH1s = dataRoot->addDqTH1D(nameTH1s, titleTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
            if(!isA) itTH1s->second.setting(data.at(jd).second);
            if(isA) { itTH1s = TH1s->find(nameTH1s); if(!(itTH1s == TH1s->end())) 
              TH1s->find(nameTH1s.c_str())->second.Fill(data.at(jd).second); }
            //---
          }
          std::vector<std::pair<double, std::pair<double, double>>> pairData = analysis->find(chamber)->second.pairData(mods->at(im).c_str());
          for(unsigned int jd = 0; jd < pairData.size(); jd++) {
            nameTH2s = Form("%d%s%d%s%sTimeProfileCouple", chamber, _strDelim.c_str(), (int)pairData.at(jd).first, _strDelim.c_str(), mods->at(im).c_str()); 
            titleTH2s = Form("%d%s%s", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
            itTH2s = dataRoot->addDqTH2D(nameTH2s, titleTH2s, TH2s); itTH2s->second.config(nameTH2s, &_params);
            if(!isA) itTH2s->second.setting(pairData.at(jd).second.first, pairData.at(jd).second.second);
            if(isA) { itTH2s = TH2s->find(nameTH2s); if(!(itTH2s == TH2s->end())) 
              TH2s->find(nameTH2s.c_str())->second.Fill(pairData.at(jd).second.first, pairData.at(jd).second.second); }
          }
          //--- 
          double minTime = 0; bool isMinTime = analysis->find(chamber)->second.minTime(&minTime, mods->at(im).c_str());
          if(isMinTime) {
            nameTH1s = Form("%d%s%sminTime", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
            titleTH1s = Form("%d%s%s", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
            itTH1s = dataRoot->addDqTH1D(nameTH1s, titleTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
            if(!isA) itTH1s->second.setting(minTime);
            if(isA) { itTH1s = TH1s->find(nameTH1s); if(!(itTH1s == TH1s->end())) 
              TH1s->find(nameTH1s.c_str())->second.Fill(minTime); }
          }
          //---
          double maxTime = 0; bool isMaxTime = analysis->find(chamber)->second.maxTime(&maxTime, mods->at(im).c_str());
          if(isMaxTime) {
            nameTH1s = Form("%d%s%smaxTime", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
            titleTH1s = Form("%d%s%s", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
            itTH1s = dataRoot->addDqTH1D(nameTH1s, titleTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
            if(!isA) itTH1s->second.setting(maxTime);
            if(isA) { itTH1s = TH1s->find(nameTH1s); if(!(itTH1s == TH1s->end())) 
              TH1s->find(nameTH1s.c_str())->second.Fill(maxTime); }
          }
          //---
          int mult = 0; bool isMult = analysis->find(chamber)->second.multiplicity(&mult, mods->at(im).c_str());
          if(isMult) {
            nameTH1s = Form("%d%s%smultiplicity", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
            titleTH1s = Form("%d%s%s", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
            itTH1s = dataRoot->addDqTH1D(nameTH1s, titleTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
            if(!isA) itTH1s->second.setting(mult);
            if(isA) { itTH1s = TH1s->find(nameTH1s); if(!(itTH1s == TH1s->end())) 
              TH1s->find(nameTH1s.c_str())->second.Fill(mult); }
          }
          //---
          std::vector<int> clusterSize; bool isClusterSize = analysis->find(chamber)->second.clusterSize(&clusterSize, mods->at(im).c_str());
          if(isClusterSize) {
            nameTH1s = Form("%d%s%sclusterSize", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
            titleTH1s = Form("%d%s%s", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
            itTH1s = dataRoot->addDqTH1D(nameTH1s, titleTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
            for( unsigned int i = 0; i < clusterSize.size(); i++) {
              if(!isA) itTH1s->second.setting(clusterSize.at(i));
              if(isA) { itTH1s = TH1s->find(nameTH1s); if(!(itTH1s == TH1s->end())) 
                TH1s->find(nameTH1s.c_str())->second.Fill(clusterSize.at(i)); }
            }
          }
          if(isMult) {
            nameTH2s = Form("%d%s%sclusterSizeMutltiplicity", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
            titleTH2s = Form("%d%s%s", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
            itTH2s = dataRoot->addDqTH2D(nameTH2s, titleTH2s, TH2s); itTH2s->second.config(nameTH2s, &_params);
            for( unsigned int i = 0; i < clusterSize.size(); i++) {
              if(!isA) itTH2s->second.setting(mult, clusterSize.at(i));
              if(isA) { itTH2s = TH2s->find(nameTH2s); if(!(itTH2s == TH2s->end())) 
                TH2s->find(nameTH2s.c_str())->second.Fill(mult, clusterSize.at(i)); }
            }
          }
          clusterSize.clear();
    		  if(isA) { // fill eff plot
            bool isEff = analysis->find(chamber)->second.efficiency(mods->at(im).c_str()); // count events
            if(isEff) {
    		  	  nameTGEs = Form("%d%s%sefficiency", chamber, _strDelim.c_str(), mods->at(im).c_str());   
          	  itTGEs = dataRoot->addDqTGE(nameTGEs, nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
    	      }
          }
        }
      
        // CLUSTERING STUDY
				std::map<int, lyCB::data> cluster = analysis->find(chamber)->second.data();
    		double LR_M = 0; double HR_M = 0;
        for(auto& it: cluster) {
          if(cluster.find(it.first)->second.HR.size() > 0) { 
            HR_M +=1;
            nameTH1s = Form("%d%s%sHRcs", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
            titleTH1s = Form("%d%s%s", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
            itTH1s = dataRoot->addDqTH1D(nameTH1s, titleTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
            if(!isA) itTH1s->second.setting(cluster.find(it.first)->second.HR.size());
            if(isA) { itTH1s = TH1s->find(nameTH1s); if(!(itTH1s == TH1s->end())) 
              TH1s->find(nameTH1s.c_str())->second.Fill(cluster.find(it.first)->second.HR.size()); }
				  	
	 			  	for(unsigned int i = 0; i < cluster.find(it.first)->second.HR.size() ; i++) {
      	  		for(unsigned int k = 0; k < cluster.find(it.first)->second.HR.size(); k++) {
          			if(cluster.find(it.first)->second.HR.at(i).first !=  cluster.find(it.first)->second.HR.at(k).first) {
            			nameTH1s = Form("%d%s%sHRtimeTHR", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
            			titleTH1s = Form("%d%s%s", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
            			itTH1s = dataRoot->addDqTH1D(nameTH1s, titleTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
            			if(!isA) itTH1s->second.setting(std::abs(cluster.find(it.first)->second.HR.at(i).second - cluster.find(it.first)->second.HR.at(k).second));
            			if(isA) { itTH1s = TH1s->find(nameTH1s); if(!(itTH1s == TH1s->end())) 
              			TH1s->find(nameTH1s.c_str())->second.Fill(std::abs(cluster.find(it.first)->second.HR.at(i).second - cluster.find(it.first)->second.HR.at(k).second)); }
				  			}
				  		}
				  	}            
          }
          if(cluster.find(it.first)->second.LR.size() > 0) { 
            LR_M +=1;
            nameTH1s = Form("%d%s%sLRcs", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
            titleTH1s = Form("%d%s%s", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
            itTH1s = dataRoot->addDqTH1D(nameTH1s, titleTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
            if(!isA) itTH1s->second.setting( cluster.find(it.first)->second.LR.size());
            if(isA) { itTH1s = TH1s->find(nameTH1s); if(!(itTH1s == TH1s->end()))  
              TH1s->find(nameTH1s.c_str())->second.Fill(cluster.find(it.first)->second.LR.size()); }
	 			  	
				  	for(unsigned int i = 0; i < cluster.find(it.first)->second.LR.size() ; i++) {
      	  		for(unsigned int k = 0; k < cluster.find(it.first)->second.LR.size(); k++) {
          			if(cluster.find(it.first)->second.LR.at(i).first !=  cluster.find(it.first)->second.LR.at(k).first) {
            			nameTH1s = Form("%d%s%sLRtimeTHR", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
            			titleTH1s = Form("%d%s%s", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
            			itTH1s = dataRoot->addDqTH1D(nameTH1s, titleTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
            			if(!isA) itTH1s->second.setting(std::abs(cluster.find(it.first)->second.LR.at(i).second - cluster.find(it.first)->second.LR.at(k).second));
            			if(isA) { itTH1s = TH1s->find(nameTH1s); if(!(itTH1s == TH1s->end())) 
              			TH1s->find(nameTH1s.c_str())->second.Fill(std::abs(cluster.find(it.first)->second.LR.at(i).second - cluster.find(it.first)->second.LR.at(k).second)); }
				  			}
				  		}
				  	}            
          }
            for(unsigned int j = 0; j < cluster.find(it.first)->second.HR.size(); j++) {
				  	for(unsigned int jj = 0; jj < cluster.find(it.first)->second.LR.size(); jj++) {
				  		if(cluster.find(it.first)->second.LR.size() > j) {
				  			double strip_HR = cluster.find(it.first)->second.HR.at(j).first;
				  			double strip_LR = cluster.find(it.first)->second.LR.at(j).first;
      	  			nameTH2s = Form("%d%s%lu%s%sHRLRcs", chamber, _strDelim.c_str(), cluster.find(it.first)->second.HR.size(), _strDelim.c_str(), mods->at(im).c_str()); 
      	  			titleTH2s = Form("%d%s%lu%s%s", chamber, _strDelim.c_str(), cluster.find(it.first)->second.HR.size(), _strDelim.c_str(), mods->at(im).c_str()); 
      	  			itTH2s = dataRoot->addDqTH2D(nameTH2s, titleTH2s, TH2s); itTH2s->second.config(nameTH2s, &_params);
      	  			if(!isA) itTH2s->second.setting(strip_HR, strip_LR);
      	  			if(isA) { itTH2s = TH2s->find(nameTH2s); if(!(itTH2s == TH2s->end())) 
      	    			TH2s->find(nameTH2s.c_str())->second.Fill(strip_HR, strip_LR); }
				  		}
				  	}
				  }
				}
        if(HR_M > 0) {
          nameTH1s = Form("%d%s%sHRm", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
          titleTH1s = Form("%d%s%s", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
          itTH1s = dataRoot->addDqTH1D(nameTH1s, titleTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
          if(!isA) itTH1s->second.setting(HR_M);
          if(isA) { itTH1s = TH1s->find(nameTH1s); if(!(itTH1s == TH1s->end())) 
            TH1s->find(nameTH1s.c_str())->second.Fill(HR_M); }
        }
        if(LR_M > 0) {
          nameTH1s = Form("%d%s%sLRm", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
          titleTH1s = Form("%d%s%s", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
          itTH1s = dataRoot->addDqTH1D(nameTH1s, titleTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
          if(!isA) itTH1s->second.setting(LR_M);
          if(isA) { itTH1s = TH1s->find(nameTH1s); if(!(itTH1s == TH1s->end()))  
            TH1s->find(nameTH1s.c_str())->second.Fill(LR_M); }
        }
      }
      int multLR = 0; analysis->find(chamber)->second.multiplicity(&multLR, "LR");
      int multHR = 0; analysis->find(chamber)->second.multiplicity(&multHR, "HR");
      nameTH2s = Form("%d%s%smultCorrelation", chamber, _strDelim.c_str(), "HRLR"); 
      titleTH2s = Form("%d%s%s", chamber, _strDelim.c_str(), "HRLR"); 
      itTH2s = dataRoot->addDqTH2D(nameTH2s, titleTH2s, TH2s); itTH2s->second.config(nameTH2s, &_params);
      if(!isA) itTH2s->second.setting(multHR, multLR);
      if(isA) { itTH2s = TH2s->find(nameTH2s); if(!(itTH2s == TH2s->end())) 
      TH2s->find(nameTH2s.c_str())->second.Fill(multHR, multLR); }
    

    }

  }
  if(mod == "CONFIG END") {
    std::string name;
    for(unsigned int ic = 0; ic < event->chambers()->size(); ic++) {
      int chamber = event->chambers()->at(ic);
      std::string filters = analysis->find(chamber)->second.filtersStr(); 
      for(auto& it: *TH1s) {
        for(unsigned int im = 0; im < mods->size(); im++) {
          std::string modParams = analysis->find(chamber)->second.modParamsStr(mods->at(im)); 
          if(_parser.last(it.second.GetTitle()) == _parser.last((mods->at(im).c_str()))) {
            name = Form("%s%s%s%s%s", it.second.GetName(), _strDelim.c_str(), modParams.c_str(), _strDelim.c_str(), filters.c_str()); 
            it.second.SetTitle(name.c_str()); 
          }
        }
        it.second.setting();
      }
      for(auto& it: *TH2s) {
        for(unsigned int im = 0; im < mods->size(); im++) {
          std::string modParams = analysis->find(chamber)->second.modParamsStr(mods->at(im)); 
          if(_parser.last(it.second.GetTitle()) == _parser.last((mods->at(im).c_str()))) {
            name = Form("%s%s%s%s%s", it.second.GetName(), _strDelim.c_str(), modParams.c_str(), _strDelim.c_str(), filters.c_str()); 
            it.second.SetTitle(name.c_str()); 
          }
        }
        it.second.setting();
      }
    }
  }
  if(mod == "ANALYSIS END") {

    for(unsigned int ic = 0; ic < event->chambers()->size(); ic++) {
      int chamber = event->chambers()->at(ic);

      std::string filters = analysis->find(chamber)->second.filtersStr(); 
      for(unsigned int im = 0; im < mods->size(); im++) {
        analysis->find(chamber)->second.algos(mods->at(im));
        std::string modParams = analysis->find(chamber)->second.modParamsStr(mods->at(im)); 

        // fit 2DProfile strip by strip 
				std::string strValue; double entries; double nRMS;
    		bool isRMS = this->findParam("offsetFit&nRMS", &strValue, &_params);
				if(isRMS) _parser.strToDouble(&nRMS, strValue);
    		bool isEntries = this->findParam("offsetFit&entries", &strValue, &_params);
				if(isEntries) _parser.strToDouble(&entries, strValue);
        nameTH2s = Form("%d%s%sstripTimeProfile", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
      	auto it = TH2s->find(nameTH2s);
      	if(!(it == TH2s->end())) {
					TH1D * projh2X = it->second.ProjectionX();
        	int nbinsx = projh2X->GetXaxis()->GetNbins();
        	for(int bin = 0; bin < nbinsx; bin++) {
						TH1D * proj = it->second.ProjectionX("h", bin, bin, "");
          	if(proj->GetEntries() > entries) {
							double mean = proj->GetMean(); double rms = proj->GetRMS();
          		TF1 *f1 = new TF1("f1", "gaus", mean-nRMS*rms, mean+nRMS*rms);
              proj->Fit("f1", "R");
            	double sigma = f1->GetParameter(2); double errSigma = f1->GetParError(2);
							nameTGEs = Form("%d%s%ssigmas2D", chamber, _strDelim.c_str(), mods->at(im).c_str());   
        			itTGEs = dataRoot->addDqTGE(nameTGEs, nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
        			TGEs->find(nameTGEs.c_str())->second->setting(float(bin)/2, 0, sigma, errSigma);
							delete f1;
						}
        	}
					delete projh2X;
      	}

        // CLUSTER STUDY
    		nameTH1s = Form("%d%s%sHRcs", chamber, _strDelim.c_str(), mods->at(im).c_str());
        auto itCluster = TH1s->find(nameTH1s);
        if(!(itCluster == TH1s->end())) {
          double mult = itCluster->second.GetMean(); double eMult = itCluster->second.GetRMS()/std::sqrt(itCluster->second.GetEntries());
    		  double value = analysis->find(chamber)->second.getValue(run); double eValue = 0;
    		  nameTGEs = Form("%d%s%sHRcs", chamber, _strDelim.c_str(), mods->at(im).c_str());   
          itTGEs = dataRoot->addDqTGE(nameTGEs, nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
          TGEs->find(nameTGEs.c_str())->second->setting(value, eValue, mult, eMult);
        }
    		nameTH1s = Form("%d%s%sLRcs", chamber, _strDelim.c_str(), mods->at(im).c_str());
        itCluster = TH1s->find(nameTH1s);
        if(!(itCluster == TH1s->end())) {
          double mult = itCluster->second.GetMean(); double eMult = itCluster->second.GetRMS()/std::sqrt(itCluster->second.GetEntries());
          
    		  double value = analysis->find(chamber)->second.getValue(run); double eValue = 0;
    		  nameTGEs = Form("%d%s%sLRcs", chamber, _strDelim.c_str(), mods->at(im).c_str());   
          itTGEs = dataRoot->addDqTGE(nameTGEs, nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
          TGEs->find(nameTGEs.c_str())->second->setting(value, eValue, mult, eMult);
        }
    		nameTH1s = Form("%d%s%sHRm", chamber, _strDelim.c_str(), mods->at(im).c_str());
        itCluster = TH1s->find(nameTH1s);
        if(!(itCluster == TH1s->end())) {
          double mult = itCluster->second.GetMean(); double eMult = itCluster->second.GetRMS()/std::sqrt(itCluster->second.GetEntries());
    		  double value = analysis->find(chamber)->second.getValue(run); double eValue = 0;
    		  nameTGEs = Form("%d%s%sHRm", chamber, _strDelim.c_str(), mods->at(im).c_str());   
          itTGEs = dataRoot->addDqTGE(nameTGEs, nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
          TGEs->find(nameTGEs.c_str())->second->setting(value, eValue, mult, eMult);
        }
    		nameTH1s = Form("%d%s%sLRm", chamber, _strDelim.c_str(), mods->at(im).c_str());
        itCluster = TH1s->find(nameTH1s);
        if(!(itCluster == TH1s->end())) {
          double mult = itCluster->second.GetMean(); double eMult = itCluster->second.GetRMS()/std::sqrt(itCluster->second.GetEntries());
    		  double value = analysis->find(chamber)->second.getValue(run); double eValue = 0;
    		  nameTGEs = Form("%d%s%sLRm", chamber, _strDelim.c_str(), mods->at(im).c_str());   
          itTGEs = dataRoot->addDqTGE(nameTGEs, nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
          TGEs->find(nameTGEs.c_str())->second->setting(value, eValue, mult, eMult);
        }
        // ---
      }
    }
    for(unsigned int ic = 0; ic < event->chambers()->size(); ic++) {
      int chamber = event->chambers()->at(ic); 
      
      nameTH2s = Form("%d%s%smultCorrelation", chamber, _strDelim.c_str(), "HRLR"); 
      auto it = TH2s->find(nameTH2s);
      if(!(it == TH2s->end())) {
        double correlation = it->second.GetCorrelationFactor();
        double value = analysis->find(chamber)->second.getValue(run); double eValue = 0;
        nameTGEs = Form("%d%s%smultCorr", chamber, _strDelim.c_str(), "HRLR");   
        itTGEs = dataRoot->addDqTGE(nameTGEs, nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
        TGEs->find(nameTGEs.c_str())->second->setting(value, eValue, correlation, 0);
      }

      std::string filters = analysis->find(chamber)->second.filtersStr(); 
    	double numTrigers = 0; double eNumTrigers = 0; bool isNumTrigers = analysis->find(chamber)->second.getNumTrigers(&numTrigers); 
    	if(isNumTrigers) {
    	  double value = analysis->find(chamber)->second.getValue(run); double eValue = 0;
    		nameTGEs = Form("%d%snumberOfEventsWithTriger", chamber,  _strDelim.c_str());   
        itTGEs = dataRoot->addDqTGE(nameTGEs, nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
        TGEs->find(nameTGEs.c_str())->second->setting(value, eValue, numTrigers, eNumTrigers);
    	}
      double numSkipedTrigers = 0; bool isNumSkipedTrigers = analysis->find(chamber)->second.getNumSkipedTrigers(&numSkipedTrigers); 
    	if(isNumSkipedTrigers) {
    	  double value = analysis->find(chamber)->second.getValue(run); double eValue = 0;
    		nameTGEs = Form("%d%snumberOfSkipedEventsWithTriger", chamber, _strDelim.c_str());   
        itTGEs = dataRoot->addDqTGE(nameTGEs, nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
        TGEs->find(nameTGEs.c_str())->second->setting(value, eValue, numSkipedTrigers/numTrigers, 0);
    	}
    	int numEvents = analysis->find(chamber)->second.getNumberEvents();
    	double value = analysis->find(chamber)->second.getValue(run); double eValue = 0;
    	nameTGEs = Form("%d%snumberOfEvents", chamber, _strDelim.c_str());   
      itTGEs = dataRoot->addDqTGE(nameTGEs, nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
      TGEs->find(nameTGEs.c_str())->second->setting(value, eValue, numEvents, 0);
      for(unsigned int im = 0; im < mods->size(); im++) {
        
        std::string modParams = analysis->find(chamber)->second.modParamsStr(mods->at(im)); 
    		double eff = 0; double eEff = 0; bool isEff = analysis->find(chamber)->second.efficiency(&eff, &eEff, mods->at(im).c_str()); 
    		double eEff_s = 0.006; double eEff_a = 0.0009;
        eEff = std::sqrt(std::pow(eEff,2) + std::pow(eEff_s, 2) + std::pow(eEff_a, 2));
        double eEffh = eEff; if(eEffh + eff > 1) eEffh = 1-eff; 
        double eEffl = eEff; if(eff - eEffl < 0) eEffl = eff; 
        if(isEff) {
    			double value = analysis->find(chamber)->second.getValue(run); double eValue = 0.001;
    			nameTGAEs = Form("%d%s%sefficiency", chamber, _strDelim.c_str(), mods->at(im).c_str());   
          itTGAEs = dataRoot->addDqTGAE(nameTGAEs, nameTGAEs, TGAEs); itTGAEs->second->config(nameTGAEs, &_params);
          TGAEs->find(nameTGAEs.c_str())->second->setting(value, eValue, eValue, eff, eEffl, eEffh);
    		}

        nameTH1s = Form("%d%s%sstripProfile", chamber, _strDelim.c_str(), mods->at(im).c_str()); 
        int nStrips = TH1s->find(nameTH1s.c_str())->second.GetXaxis()->GetNbins();
        for(int is = 0; is < nStrips; is++) {
          double noise = 0; double eNoise = 0; bool isNoise = analysis->find(chamber)->second.noise(&noise, &eNoise, is, TH1s->find(nameTH1s.c_str())->second.GetBinContent(is), mods->at(im).c_str());
    		  eNoise = 0;
    		  if(isNoise) {
    			  double value = analysis->find(chamber)->second.getValue(run); double eValue = 0;
    			  nameTGEs = Form("%d%s%d%s%srateStrip", chamber, _strDelim.c_str(), is, _strDelim.c_str(), mods->at(im).c_str());   
            itTGEs = dataRoot->addDqTGE(nameTGEs, nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
            TGEs->find(nameTGEs.c_str())->second->setting(value, eValue, noise, eNoise);
    			  nameTG2Ds = Form("%d%s%srateStrips", chamber, _strDelim.c_str(), mods->at(im).c_str());   
            itTG2Ds = dataRoot->addDqTG2D(nameTG2Ds, nameTG2Ds, TG2Ds); itTG2Ds->second->config(nameTG2Ds, &_params);
            TG2Ds->find(nameTG2Ds.c_str())->second->setting(is, value, noise);
    		  }
        }
        double noise = 0; double eNoise = 0; bool isNoise = analysis->find(chamber)->second.noise(&noise, &eNoise, -1, -1, mods->at(im).c_str());
    		eNoise = 0;
    		if(isNoise) {
    			double value = analysis->find(chamber)->second.getValue(run); double eValue = 0;
    			nameTGEs = Form("%d%s%srate", chamber, _strDelim.c_str(), mods->at(im).c_str());   
          itTGEs = dataRoot->addDqTGE(nameTGEs, nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
          TGEs->find(nameTGEs.c_str())->second->setting(value, eValue, noise, eNoise);
    		}

    		nameTH1s = Form("%d%s%smultiplicity", chamber, _strDelim.c_str(), mods->at(im).c_str());
        auto itMult = TH1s->find(nameTH1s);
        if(!(itMult == TH1s->end())) {
          double mult = itMult->second.GetMean(); double eMult = itMult->second.GetRMS()/std::sqrt(itMult->second.GetEntries());
    		  double value = analysis->find(chamber)->second.getValue(run); double eValue = 0;
    		  nameTGEs = Form("%d%s%smultiplicity", chamber, _strDelim.c_str(), mods->at(im).c_str());   
          itTGEs = dataRoot->addDqTGE(nameTGEs, nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
          TGEs->find(nameTGEs.c_str())->second->setting(value, eValue, mult, eMult);
        }
    		nameTH1s = Form("%d%s%sfilterX", chamber, _strDelim.c_str(), mods->at(im).c_str());
        auto itFilterX = TH1s->find(nameTH1s);
        if(!(itFilterX == TH1s->end())) {
          double filterX = itFilterX->second.GetMean(); double eFilterX = itFilterX->second.GetRMS()/std::sqrt(itFilterX->second.GetEntries());
    		  double value = analysis->find(chamber)->second.getValue(run); double eValue = 0;
    		  nameTGEs = Form("%d%s%sfilterX", chamber, _strDelim.c_str(), mods->at(im).c_str());   
          itTGEs = dataRoot->addDqTGE(nameTGEs, nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
          TGEs->find(nameTGEs.c_str())->second->setting(value, eValue, filterX, eFilterX);
        }
    		nameTH1s = Form("%d%s%sclusterSize", chamber, _strDelim.c_str(), mods->at(im).c_str());
        auto itClusterSize = TH1s->find(nameTH1s);
        if(!(itClusterSize == TH1s->end())) {
          double clusterSize = itClusterSize->second.GetMean(); double eClusterSize = itClusterSize->second.GetRMS()/std::sqrt(itClusterSize->second.GetEntries());
    		  double value = analysis->find(chamber)->second.getValue(run); double eValue = 0;
    		  nameTGEs = Form("%d%s%sclusterSize", chamber, _strDelim.c_str(), mods->at(im).c_str());   
          itTGEs = dataRoot->addDqTGE(nameTGEs, nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
          TGEs->find(nameTGEs.c_str())->second->setting(value, eValue, clusterSize, eClusterSize);
        }
    		nameTH1s = Form("%d%sNumberOfHits%s", chamber, _strDelim.c_str(), mods->at(im).c_str());
        auto itMHR = TH1s->find(nameTH1s);
        if(!(itMHR == TH1s->end())) {
          double mult = itMHR->second.GetEntries(); double eMult = 0;
    		  double value = analysis->find(chamber)->second.getValue(run); double eValue = 0;
    		  nameTGEs = Form("%d%s%sNumberOfHits", chamber, _strDelim.c_str(), mods->at(im).c_str());   
          itTGEs = dataRoot->addDqTGE(nameTGEs, nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
          TGEs->find(nameTGEs.c_str())->second->setting(value, eValue, mult, eMult);
        }
      }
    }
  }
  else
    return false;
  return true;
}

bool LyPeti::petiroc(std::pair<int, int> run, std::vector<std::string> *mods,  
                     LyPetiEvent *event, std::map<int, LyPetiAnalysis> *analysis, 
                     DqRoot *dataRoot, 
                     std::map<std::string, DqTGE*> *TGEs, 
                     std::map<std::string, DqTGAE*> *TGAEs, 
                     std::map<std::string, DqTG2D*> *TG2Ds, 
                     std::map<std::string, DqMTGE*> *MTGEs, 
                     std::map<std::string, DqTH1D> *TH1s, 
                     std::map<std::string, DqTH2D> *TH2s)
{
    LyPetiRead stream; bool isOpen = false; bool isClose = false;
  
    std::map<int, RoPetiEvent*> roEvents;

    TTree tree(Form("tree_%d", run.first), Form("tree_%d", run.first));
    for(unsigned int ic = 0; ic < event->chambers()->size(); ic++) {
      int chamber = event->chambers()->at(ic); 
      std::string pathRAW = Form("%s_%s/tree", _comment.c_str(), _logRunNumber.c_str()); 
      roEvents.insert(std::make_pair(chamber, new RoPetiEvent{}));
      tree.Branch(Form("raw_chamber_%d", chamber), "RoPetiEvent", &roEvents.find(chamber)->second);
    }

    std::vector<std::string> loops;
    bool isOffsetSkip = false;
    if(run.second == 0) {
      loops.push_back("CONFIG"); loops.push_back("ANALYSIS");
      for(unsigned int ic = 0; ic < event->chambers()->size(); ic++) {
        int chamber = event->chambers()->at(ic); 
  	    if(analysis->find(chamber)->second.getValue(run.first) == -1)
          isOffsetSkip = true;
      }
    }
    else if(run.second > 0) {
      loops.push_back("OFFSET" + _strDelim + "CONFIG"); 
      loops.push_back("OFFSET" + _strDelim +  "ANALYSIS");
    }

    for(unsigned int l = 0; l < loops.size(); l++) {
      if(isOffsetSkip) continue;
      std::cout << loops.at(l) << " loop..." << std::endl;
      bool isFindRun = false;
		  for(unsigned int i = 0; i < _inputs.size(); i++) {
		  
        if(run.first == this->numRun(_parser.fileName(_inputs.at(i)))) isFindRun = true;
        if(run.first != this->numRun(_parser.fileName(_inputs.at(i)))) {
          if(i < _inputs.size() - 1) continue;
          else if(!isFindRun) {
            _log += " WARNING! RUN: " 
	    				 + std::to_string(run.first) + " was skipped! Can't find files!" 
	    				 + _rowDelimiter;
            return false;
          }
          else continue;
        }
        std::cout << "    File[" << i << "]: " 
		  												 << _parser.fileName(_inputs.at(i)) << std::endl;

  	    isOpen = stream.open(_inputs.at(i));
  	    if (!isOpen) {
  	      _log += " WARNING! file: " 
		  				 + _parser.fileName(_inputs.at(i))  + " was skipped! Can't open!" 
		  				 + _rowDelimiter;
  	      continue;
  	    }

        // raw data
  	    int codeRead = 1;
		  	int size = stream.size();
  	    int pos = 0, prev = 0;
  	    std::vector<zdaq::buffer*> buffers; // raw data (buffers)
        
        DqProcessBar processBar;
  	    processBar.setEndIndex(size);
  	    processBar.setCurrentStep(0);
  	    while(codeRead > 0) {
  	      codeRead = stream.read(&pos, &buffers); // read file
        
  	      // fill event class with raw data from buffers
  	      event->clear(); event->fill(&buffers); 


  	      // clear memory form old buffers
  	      for (int j = 0; j < (int)buffers.size(); j++)
  	        delete buffers.at(j);
  	      buffers.clear();
  	    
  	      bool isEvent = event->check();
  	      if(!isEvent) {
  	        _log += " WARNING! Event in pos: " + std::to_string(prev) + " was skipped! Can't open!" 
		  		       + _rowDelimiter;
  	        continue;
  	      }
	        // =====================================================================
            offset(loops.at(l), mods, run.first, event, analysis, dataRoot, TGEs, TH1s);
            core(loops.at(l), mods, run.first, event, &roEvents, analysis, dataRoot, TGEs, TGAEs, TG2Ds, TH1s, TH2s);
          // =====================================================================
          for(unsigned int ic = 0; ic < event->chambers()->size(); ic++) {
            if(loops.at(l) == "ANALYSIS") {
              roEvents.find(event->chambers()->at(ic))->second->getHeader()->Set(event->header(ic)->number, run.first);
              tree.Fill(); roEvents.find(event->chambers()->at(ic))->second->Clear("C");
            }
            analysis->find(event->chambers()->at(ic))->second.clear(loops.at(l) + " EVENT");
          }
  	      
  	    	if(codeRead > 0) processBar.print(pos, 0.02);
		  		else processBar.print(size, 0.02);
  	      prev = pos;
		    }
  	    std::cout << std::endl;
  	    if (isOpen) {
  	      isClose = stream.close();
  	      if (!isClose) {
  	        _log += " ERROR! file: " + _parser.fileName(_inputs.at(i))  
		  															 + " Can't close!" + _rowDelimiter;
  	      	return false;
		  		}
  	    }
	    }
      if(loops.at(l) == "CONFIG" || loops.at(l) == ("OFFSET" + _strDelim + "CONFIG"))
        for(unsigned int ic = 0; ic < event->chambers()->size(); ic++)
          analysis->find(event->chambers()->at(ic))->second.clear(loops.at(l) + " RUN");
	    // =======================================================================
      offset(loops.at(l) + " END", mods, run.first, event, analysis, dataRoot, TGEs, TH1s);
      core(loops.at(l) + " END", mods, run.first, event, &roEvents, analysis, dataRoot, TGEs, TGAEs, TG2Ds, TH1s, TH2s);
	    // =======================================================================
      if(loops.at(l) == "ANALYSIS") { std::string pathRAW = Form("%s_%s/data", _comment.c_str(), _logRunNumber.c_str()); dataRoot->writeObject(pathRAW, &tree);  }
      if(loops.at(l) == "ANALYSIS" || loops.at(l) == ("OFFSET" + _strDelim +  "ANALYSIS"))
        for(unsigned int ic = 0; ic < event->chambers()->size(); ic++)
          analysis->find(event->chambers()->at(ic))->second.clear(loops.at(l) + " RUN");
    }
    return true;
}

bool LyPeti::analysis() 
{ 
  DqRoot dataRoot; dataRoot.setOutputFile(_rootFilePath, "default");
  LyPetiEvent event;
  this->initEvent(&event); 
  
  std::vector<std::string> mods;
  std::string sMods = this->findParam("mods", &_params);
  mods = _parser.parsArray(sMods);
  
  std::vector<int> runs = this->initRuns();
  std::vector<std::pair<int, int>> RUNS = this->initOffSetRuns(&runs, &event);
  
  std::map<int, LyPetiAnalysis> analysis; 
  this->initAnalys(&analysis, &event, runs);
  
  std::cout  << "\nCookies are ready, invite you to enjoy the taste!" << std::endl;
  std::cout << "#-------------------------------------------------------------------------------" << std::endl;
  
  std::map<std::string, DqTGE*> TGEs;
  std::map<std::string, DqTGAE*> TGAEs;
  std::map<std::string, DqTG2D*> TG2Ds;
  std::map<std::string, DqMTGE*> MTGEs;
  for(unsigned int r = 0; r < RUNS.size(); r++) {
    std::cout << std::endl << "Run[" << r << "]: " << RUNS.at(r).first << std::endl;
    
    std::map<std::string, DqTH1D> TH1s;
    std::map<std::string, DqTH2D> TH2s;

    // =========================================================================
    this->petiroc(RUNS.at(r), &mods, &event, &analysis, &dataRoot, &TGEs, &TGAEs, &TG2Ds, &MTGEs, &TH1s, &TH2s);
    // =========================================================================
    
    // ANALYSIS ALL RUNS
    // =========================================================================
    
    // =========================================================================
    
    this->saveHists(RUNS.at(r).first, &analysis, &dataRoot, &TH1s, &TH2s);
    TH1s.clear(); TH2s.clear();
  }
      
  std::cout << "End of Processing!" << std::endl;
  std::cout << "#-------------------------------------------------------------------------------\n" << std::endl;
	std::cout  << "Comparing cookies and Cleaning the kitchen..." << std::endl;

  
  this->saveGraphs(&analysis, &dataRoot, &TGEs); TGEs.clear();
  this->saveGraphs(&analysis, &dataRoot, &TGAEs); TGAEs.clear();
  this->saveGraphs(&analysis, &dataRoot, &TG2Ds); TG2Ds.clear();
  this->saveGraphs(&analysis, &dataRoot, &MTGEs); MTGEs.clear();
  
	std::cout << "You're welcome!" << std::endl;
  std::cout << "#---\n" << std::endl;
  std::cout << "#COMMENT: " << _comment << "_" << _logRunNumber <<  std::endl;
  std::cout << "#OUTPUT FILE: " << _rootFilePath <<  std::endl;
  
  return true;
}

bool LyPeti::run() 
{
  bool isRun = false;
  if (_mod == "analysis") isRun = analysis();
  return isRun;
}
//------------------------------------------------------------------------------
