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
    dataRoot->writeTH1toCanvasCMS(path, &profiles.second, profiles.second.getFit(), false);
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

    dataRoot->writeTH2toCanvasCMS(path, &profiles.second,  false);
  }
  
  return true;
}
bool LyPeti::saveGraphs(std::map<int, LyPetiAnalysis> *analysis, DqRoot *dataRoot,
                        std::map<std::string, DqTGE*> *TGEs)
{

  for(auto profiles: *TGEs) {
 	  //profiles.second->sortByX(); 
    profiles.second->plot();
    std::string histName = _parser.histName(profiles.first);
    std::string path = _comment + "_" + _logRunNumber + "/analysis/chamber_" + _parser.chamberName(profiles.first); 
    if(_comment == "webdcs")
      path = "analysis/chamber_" + _parser.chamberName(profiles.first); 
    std::string folder = "";
    bool isFolder = this->findParam((histName + "&path"), &folder, &_params);
    if(folder == "print") {
      std::string line;
      std::vector<std::string> lines;
      profiles.second->print(&lines);
      //for(unsigned int i = 0; i < lines.size(); i++)
      //  std::cout << _comment + " " + lines.at(i) << std::endl;
      std::string pathFile = "p_"+ profiles.first + ".txt";
       std::ifstream myfile(pathFile.c_str());
      if(myfile.is_open()) {
        int ss = 0;
        while(getline(myfile, line)) {
       //   std::cout << line;
          lines[ss] = _comment  + " " + lines[ss] + " " + line;
          ss+=1;
        }
        myfile.close();
      }
      else std::cout << "Unable to open file";
      std::ofstream myfile2 (pathFile.c_str(), std::ios_base::out | std::ios_base::trunc);
      if(myfile2.is_open()) {
          for(unsigned int i = 0; i < lines.size(); i++)
            myfile2 << lines.at(i) << std::endl;
          myfile2.close();
      }
      else std::cout << "Unable to open file";
    } 
    if(isFolder)
      path += "/" + folder;
    else 
      path += "/withoutPath";
    dataRoot->writeDqTGEtoCanvasCMS(path, profiles.second, false);
    //delete profiles.second;
  }
  return true;
}
bool LyPeti::saveMGraphs(std::map<int, LyPetiAnalysis> *analysis, DqRoot *dataRoot,
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
    dataRoot->writeDqMTGEtoCanvasCMS(path, profiles.second, false);
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
  
  std::string sInverse = this->findParam("inverse", &_params);
  std::vector<std::string> vsInverse = _parser.parsArray(sInverse);
  std::vector<double> inverse = _parser.strToDoubleArray(vsInverse);
  sInverse.clear(); vsInverse.clear();
  
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
                inverse,
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
					 + "inverse=" + std::to_string(inverse.size()) + parserArrayDelimiters.at(0)
					 + "asciiChambers=" + std::to_string(chambers.size()) + _rowDelimiter;
      return false;
  }
  
  strips.clear(); trigCh.clear(); TDCs.clear(); chambers.clear(); 
  HR.clear(); timeOffSetHR.clear(); LR.clear(); timeOffSetLR.clear(); timeOffSet.clear(); inverse.clear(); // clear memory (map params) 
  
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

bool LyPeti::stripStudy(std::string mod, std::vector<std::string> *mods, 
                  int run, LyPetiEvent *event, std::map<int, LyPetiAnalysis> *analysis,
                  DqRoot *dataRoot,
                  std::map<std::string, DqTGE*> *TGEs, 
                  std::map<std::string, DqTH1D> *TH1s, 
                  std::map<std::string, DqTH2D> *TH2s)
{
  std::string nameTGEs = "";    
  auto itTGEs = TGEs->find("");
  std::string nameTH1s = "";
  auto itTH1s = TH1s->find("");
  std::string nameTH2s = "";
  auto itTH2s = TH2s->find("");
  if(mod == "CONFIG") {
    std::map<int, lyCB::data> chamber = analysis->find(0)->second.data();
    for(auto& it: chamber) {
      for(unsigned int j = 0; j < chamber.find(it.first)->second.HR.size(); j++) {
	      //if(chamber.find(it.first)->second.HR.size() == 1 || chamber.find(it.first)->second.HR.size() == 3) {
          double strip_HR = analysis->find(0)->second.centralStripCB(&chamber.find(it.first)->second.HR);
          double time_HR = analysis->find(0)->second.middleTimeCB(&chamber.find(it.first)->second.HR);

          nameTH1s =  Form("200_stripsHR"); 
          itTH1s = dataRoot->addDqTH1D(nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
			    itTH1s->second.setting(strip_HR);
	        nameTH1s =  Form("200_%1.f_TimeHR", strip_HR);  
          itTH1s = dataRoot->addDqTH1D(nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
			    itTH1s->second.setting(time_HR);
        //}
      }
      for(unsigned int j = 0; j < chamber.find(it.first)->second.LR.size(); j++) {
	      //if(chamber.find(it.first)->second.LR.size() == 1 || chamber.find(it.first)->second.LR.size() == 3) {
          double strip_LR = analysis->find(0)->second.centralStripCB(&chamber.find(it.first)->second.LR);
          double time_LR = analysis->find(0)->second.middleTimeCB(&chamber.find(it.first)->second.LR);
          
          nameTH1s =  Form("200_stripsLR"); 
          itTH1s = dataRoot->addDqTH1D(nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
			    itTH1s->second.setting(strip_LR);
	        nameTH1s =  Form("200_%1.f_TimeLR", strip_LR);  
          itTH1s = dataRoot->addDqTH1D(nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
			    itTH1s->second.setting(time_LR);
        //}
      }
    }
  }
  if(mod == "CONFIG END") {
    // проверка
    for(auto& it: *TH1s) it.second.setting();
    for(auto& it: *TH2s) it.second.setting();
  }
  if(mod == "ANALYSIS") {
    std::map<int, lyCB::data> chamber = analysis->find(0)->second.data();
    for(auto& it: chamber) {
      for(unsigned int j = 0; j < chamber.find(it.first)->second.HR.size(); j++) {
	      //if(chamber.find(it.first)->second.HR.size() == 1 || chamber.find(it.first)->second.HR.size() == 3) {
          double strip_HR = analysis->find(0)->second.centralStripCB(&chamber.find(it.first)->second.HR);
          double time_HR = analysis->find(0)->second.middleTimeCB(&chamber.find(it.first)->second.HR);
          nameTH1s =  Form("200_stripsHR"); 
          auto it =  TH1s->find(nameTH1s.c_str());
          if(it != TH1s->end()) TH1s->find(nameTH1s.c_str())->second.Fill(strip_HR);
	        nameTH1s =  Form("200_%1.f_TimeHR", strip_HR);  
          it =  TH1s->find(nameTH1s.c_str());
          if(it != TH1s->end()) TH1s->find(nameTH1s.c_str())->second.Fill(time_HR);
        //}
      }
      for(unsigned int j = 0; j < chamber.find(it.first)->second.LR.size(); j++) {
	      //if(chamber.find(it.first)->second.LR.size() == 1 || chamber.find(it.first)->second.LR.size() == 3) {
          double strip_LR = analysis->find(0)->second.centralStripCB(&chamber.find(it.first)->second.LR);
          double time_LR = analysis->find(0)->second.middleTimeCB(&chamber.find(it.first)->second.LR);
          nameTH1s =  Form("200_stripsLR"); 
           auto it = TH1s->find(nameTH1s.c_str());
          if(it != TH1s->end()) TH1s->find(nameTH1s.c_str())->second.Fill(strip_LR);
	        nameTH1s =  Form("200_%1.f_TimeLR", strip_LR);  
          it =  TH1s->find(nameTH1s.c_str());
          if(it != TH1s->end()) TH1s->find(nameTH1s.c_str())->second.Fill(time_LR);
        //}
      }
    }
  }
  if(mod == "ANALYSIS END") {
	  std::cout << "HI " << std::endl;
  }
  else
    return false;
  return true;
}
bool LyPeti::raw(std::string mod, LyPetiEvent *event, std::map<int, LyPetiAnalysis> *analysis, 
                 DqRoot *dataRoot, std::map<int, TTree*> *treesHR, std::map<int, TTree*> *treesLR,
                 int run, 
                 Int_t *number_HR, Float_t *time_HR, Float_t *triger_HR, Float_t *ch_HR, Int_t *size_HR, Float_t *dtime_HR, uint16_t *BCID_HR, uint64_t *coarse_HR, uint8_t *fine_HR,   
                 Int_t *number_LR, Float_t *time_LR, Float_t *triger_LR, Float_t *ch_LR, Int_t *size_LR, Float_t *dtime_LR, uint16_t *BCID_LR, uint64_t *coarse_LR, uint8_t *fine_LR) 
{
  if(mod == "INIT") {
  }
  if(mod == "CONFIG") {
    for(unsigned int ic = 0; ic < event->chambers()->size(); ic++) {
      int chamber = event->chambers()->at(ic); 
      unsigned int numberOfTrigers = event->trigs(chamber)->size();
      bool isTrigers = analysis->find(chamber)->second.fillTrigers(numberOfTrigers);
    	if(isTrigers) {
      //std::cout << "=================================================" << std::endl;
        for(unsigned int js = 0; js < event->strips(chamber)->size(); js++) {
          if(event->strips(chamber)->at(js).trigs.size() == 1) {
            for(unsigned int jh = 0; jh < event->strips(chamber)->at(js).HR.size(); jh++) {
              *number_HR = event->strips(chamber)->at(js).trigs.at(0)->head->number;
              
              *ch_HR = event->strips(chamber)->at(js).number - chamber;
              *size_HR = event->strips(chamber)->at(js).HR.size();
              
              *coarse_HR = event->strips(chamber)->at(js).HR.at(jh)->coarse;
              *fine_HR = event->strips(chamber)->at(js).HR.at(jh)->fine;
              *BCID_HR = event->strips(chamber)->at(js).HR.at(jh)->BCID;
              
              *time_HR = event->strips(chamber)->at(js).HR.at(jh)->time;
              *triger_HR = event->strips(chamber)->at(js).trigs.at(0)->time;
              *dtime_HR = event->strips(chamber)->at(js).HR.at(jh)->time -  event->strips(chamber)->at(js).trigs.at(0)->time;
              // std::cout << std::setprecision(3) << "HR:" << *ch_HR << "|" << *time_HR << std::endl;
              treesHR->find(chamber)->second->Fill();
            }
     		    for(unsigned int jl = 0; jl < event->strips(chamber)->at(js).LR.size(); jl++) {
              *number_LR = event->strips(chamber)->at(js).trigs.at(0)->head->number;
              
              *ch_LR = event->strips(chamber)->at(js).number - chamber;
              *size_LR = event->strips(chamber)->at(js).LR.size();
              
              *coarse_LR = event->strips(chamber)->at(js).LR.at(jl)->coarse;
              *fine_LR = event->strips(chamber)->at(js).LR.at(jl)->fine;
              *BCID_LR = event->strips(chamber)->at(js).LR.at(jl)->BCID;
              
              *time_LR = event->strips(chamber)->at(js).LR.at(jl)->time;
              *triger_LR = event->strips(chamber)->at(js).trigs.at(0)->time;
              *dtime_LR = event->strips(chamber)->at(js).LR.at(jl)->time -  event->strips(chamber)->at(js).trigs.at(0)->time;
              treesLR->find(chamber)->second->Fill();
              // std::cout << std::setprecision(3) << "130LR:" << *ch_LR << "|" << *time_LR << std::endl; 
     		    }
          }
        }
    	}
    }
  }
  if(mod == "CONFIG END") {
    for(unsigned int ic = 0; ic < event->chambers()->size(); ic++) {
      int chamber = event->chambers()->at(ic); 
      std::string pathRAW = Form("%s_%s/RAWDataHR", _comment.c_str(), _logRunNumber.c_str()); 
      dataRoot->writeObject(pathRAW, treesHR->find(chamber)->second);
      pathRAW = Form("%s_%s/RAWDataLR", _comment.c_str(), _logRunNumber.c_str()); 
      dataRoot->writeObject(pathRAW, treesLR->find(chamber)->second);
    }
  }  
  else
    return false;
  return true;
}
bool LyPeti::offset(std::string mod, std::vector<std::string> *mods,  
                    int run, int chamber, LyPetiEvent *event, std::map<int, LyPetiAnalysis> *analysis,
                    DqRoot *dataRoot,
                    std::map<std::string, DqTGE*> *TGEs, 
                    std::map<std::string, DqTH1D> *TH1s, 
                    std::map<std::string, DqTH2D> *TH2s)
{
  std::string nameTGEs = "";    
  auto itTGEs = TGEs->find("");
  std::string nameTH1s = "";
  auto itTH1s = TH1s->find("");
  std::string nameTH2s = "";
  auto itTH2s = TH2s->find("");
  std::map<int, double> offsets; std::map<int, double> offsetsHR; std::map<int, double> offsetsLR;
    int size = event->chambers()->size();
    //if(chamber > 1) size = 1;
  if(mod ==  "OFFSET" + _strDelim + "CONFIG") {
    for(unsigned int ic = 0; ic < size; ic++) {
      if(size > 1) chamber = event->chambers()->at(ic);
      chamber = event->chambers()->at(ic);
      analysis->find(chamber)->second.setStrips(event->strips(chamber));
      unsigned int numberOfTrigers = event->trigs(chamber)->size();
      bool isTrigers = analysis->find(chamber)->second.fillTrigers(numberOfTrigers);
	    if(isTrigers) {
        for(unsigned int js = 0; js < event->strips(chamber)->size(); js++) {
          // Fill raw data with maping. 
          for(unsigned int jh = 0; jh < event->strips(chamber)->at(js).HR.size(); jh++) {
            bool isFilterHR = analysis->find(chamber)->second.filters("data_HR", js, jh); 
            if(isFilterHR) 
              analysis->find(chamber)->second.fillEvents("data_HR", js, jh); 
          //analysis->find(chamber)->second.filters("deadTime_HR", js, jh); // skip if deadTime 
          }
          for(unsigned int jl = 0; jl < event->strips(chamber)->at(js).LR.size(); jl++) {
            bool isFilterLR = analysis->find(chamber)->second.filters("data_LR", js, jl); 
            if(isFilterLR)
              analysis->find(chamber)->second.fillEvents("data_LR", js, jl); 
          //analysis->find(chamber)->second.filters("deadTime_LR", js, jl); // skip if deadTime 
        	}
	      }
      }
      if(!analysis->find(chamber)->second.skipEvent()) {  
        // ANALYSIS PART
        std::string filters = analysis->find(chamber)->second.filtersStr(); 
        for(unsigned int im = 0; im < mods->size(); im++) {
          analysis->find(chamber)->second.algos(mods->at(im));
          std::string modParams = analysis->find(chamber)->second.modParamsStr(mods->at(im)); 
          std::vector<std::pair<double, double>> data = analysis->find(chamber)->second.data(mods->at(im).c_str());
          std::map<int, std::vector<double>> offSet;
          
          // offset
          for(unsigned int d = 0; d < data.size(); d++) {
            std::vector<std::pair<double, double>> dataAND = analysis->find(chamber)->second.data("AND");
            bool isOffSet = analysis->find(chamber)->second.offSetTime(data.at(d).first, &data, &dataAND, &offSet, mods->at(im).c_str());
            if(isOffSet) {
              nameTH2s = Form("%d%s%s%s_%d_%s%stimeProfileStrip", chamber, filters.c_str(), modParams.c_str(), _strDelim.c_str(),  (int)data.at(d).first, _strDelim.c_str(),  mods->at(im).c_str()); 
              for(auto& itOffSet: offSet) {
                itTH2s = dataRoot->addDqTH2D(nameTH2s, TH2s); itTH2s->second.config(nameTH2s, &_params);
                for(unsigned int iOs = 0; iOs < itOffSet.second.size(); iOs++) {
                  itTH2s->second.setting(itOffSet.second.at(iOs), itOffSet.first);
                //  std::cout << itOffSet.first << " " << itOffSet.second.at(iOs) << std::endl;
                }
              }
            }
          }
        }
      }
    }
  }
  if(mod ==  "OFFSET" + _strDelim + "CONFIG END") {
    for(auto& it: *TH1s) it.second.setting();
    for(auto& it: *TH2s) it.second.setting();
  }
  if(mod ==  "OFFSET" + _strDelim + "ANALYSIS") {
    for(unsigned int ic = 0; ic < size; ic++) {
      if(size > 1) chamber = event->chambers()->at(ic);
      analysis->find(chamber)->second.setStrips(event->strips(chamber));
      unsigned int numberOfTrigers = event->trigs(chamber)->size();
      bool isTrigers = analysis->find(chamber)->second.fillTrigers(numberOfTrigers);
	    if(isTrigers) {
        for(unsigned int js = 0; js < event->strips(chamber)->size(); js++) {
          for(unsigned int jh = 0; jh < event->strips(chamber)->at(js).HR.size(); jh++) {
            bool isFilterHR = analysis->find(chamber)->second.filters("data_HR", js, jh); 
            if(isFilterHR)
              analysis->find(chamber)->second.fillEvents("data_HR", js, jh); 
            //analysis->find(chamber)->second.filters("deadTime_HR", js, jh); // skip if deadTime 
          }
          // LR
      	  for(unsigned int jl = 0; jl < event->strips(chamber)->at(js).LR.size(); jl++) {
            bool isFilterLR = analysis->find(chamber)->second.filters("data_LR", js, jl); 
            if(isFilterLR)
              analysis->find(chamber)->second.fillEvents("data_LR", js, jl); 
            //analysis->find(chamber)->second.filters("deadTime_LR", js, jl); // skip if deadTime 
      	  }
        }
	    }
      if(!analysis->find(chamber)->second.skipEvent()) {  
        // ANALYSIS PART
        std::string filters = analysis->find(chamber)->second.filtersStr(); 
        for(unsigned int im = 0; im < mods->size(); im++) {
          analysis->find(chamber)->second.algos(mods->at(im));
          std::string modParams = analysis->find(chamber)->second.modParamsStr(mods->at(im)); 
          std::vector<std::pair<double, double>> data = analysis->find(chamber)->second.data(mods->at(im).c_str());
          
          // offset
          for(unsigned int d = 0; d < data.size(); d++) {
            std::map<int, std::vector<double>> offSet;
            std::vector<std::pair<double, double>> dataAND = analysis->find(chamber)->second.data("AND");
            bool isOffSet = analysis->find(chamber)->second.offSetTime(data.at(d).first, &data, &dataAND, &offSet, mods->at(im).c_str());
            if(isOffSet) {
              nameTH2s = Form("%d%s%s%s_%d_%s%stimeProfileStrip", chamber, filters.c_str(), modParams.c_str(), _strDelim.c_str(),  (int)data.at(d).first, _strDelim.c_str(),  mods->at(im).c_str()); 
              for(auto& itOffSet: offSet) {
                for(unsigned int iOs = 0; iOs < itOffSet.second.size(); iOs++) {
                  auto it = TH2s->find(nameTH2s.c_str()); if(it != TH2s->end())
                    TH2s->find(nameTH2s.c_str())->second.Fill(itOffSet.second.at(iOs), itOffSet.first);
                }
              }
            }
          }
        }
      }
    }
  }
  if(mod ==  "OFFSET" + _strDelim + "ANALYSIS END") {
    for(unsigned int ic = 0; ic < size; ic++) {
      if(size > 1) chamber = event->chambers()->at(ic);
      std::string filters = analysis->find(chamber)->second.filtersStr(); 
      for(unsigned int im = 0; im < mods->size(); im++) {
        std::string modParams = analysis->find(chamber)->second.modParamsStr(mods->at(im)); 
        std::string name = _parser.last(mods->at(im));
        std::string param = _parser.first(mods->at(im));
        if((name == "HR" || name == "LR")) {
          int strips = event->numberOfStrips(chamber);
          int thrEntries = 10;
          double refStrip = 0;
          nameTH1s = Form("%d%sTimeWalk", chamber, _strDelim.c_str());
          itTH1s = dataRoot->addDqTH1D(nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
          for(int s = 0; s < strips; s++) {
	    	    double value = analysis->find(chamber)->second.getValue(run);
            if(s%2==0) {
                if(chamber == 1000) refStrip = 22;
                if(chamber == 0) refStrip = 30;
            }
            else {
                if(chamber == 1000) refStrip = 23;
                if(chamber == 0) refStrip = 31;
            }
            nameTH2s = Form("%d%s%s%s_%d_%s%stimeProfileStrip", chamber, filters.c_str(), modParams.c_str(), _strDelim.c_str(),  s+chamber, _strDelim.c_str(),  mods->at(im).c_str()); 
            auto itOffSetMean = TH2s->find(nameTH2s);
            if(!(itOffSetMean == TH2s->end())) {
              if(s < strips - 2 && s > 1 &&
                  itOffSetMean->second.ProjectionX("hX", s-1, s-1, "")->GetEntries() >= thrEntries &&
                 itOffSetMean->second.ProjectionX("hXX", s+1, s+1, "")->GetEntries() >= thrEntries ) {
                TH1D * s1 = itOffSetMean->second.ProjectionX("h1", s-1, s-1, "");
                double pMean  = s1->GetMean();
                double pRMS  = s1->GetRMS();
                std::cout << pMean << std::endl;
                TF1* sg1 = new TF1("sg", "gaus",  pMean - 3*pRMS, pMean + 3*pRMS);
                s1->Fit("sg", "QS");
                TH1D * s2 = itOffSetMean->second.ProjectionX("h2", s+1, s+1, "");
                pMean  = s2->GetMean();
                pRMS  = s2->GetRMS();
                TF1* sg2 = new TF1("sg2", "gaus",  pMean - 3*pRMS, pMean + 3*pRMS);
                s2->Fit("sg2", "QS");
                nameTH1s = Form("%d%sTimeWalk", chamber, _strDelim.c_str());
                TH1s->find(nameTH1s.c_str())->second.Fill(sg1->GetParameter(1));
                TH1s->find(nameTH1s.c_str())->second.Fill(sg2->GetParameter(1));
                delete sg2; delete sg1; delete s1; delete s2;
              }
            }
            
            double offSet = 0;
            double eOffSet = 0;
            for(int is = s+1; is < refStrip; is=is+2) {
              nameTH2s = Form("%d%s%s%s_%d_%s%stimeProfileStrip", chamber, filters.c_str(), modParams.c_str(), _strDelim.c_str(),  is+chamber, _strDelim.c_str(),  mods->at(im).c_str()); 
              auto itOffSet = TH2s->find(nameTH2s);
              if(!(itOffSet == TH2s->end())) {
                if(itOffSet->second.ProjectionX("h", is, is, "")->GetEntries() <= thrEntries) {
                   offSet = 0; break;
                }
                else {
                TH1D * projh2X = itOffSet->second.ProjectionX("h", is, is, "");
                double pMean  = projh2X->GetMean();
                double pRMS  = projh2X->GetRMS();
                TF1* g1 = new TF1("g", "gaus",  pMean - 6*pRMS, pMean + 6*pRMS);
                projh2X->Fit("g", "QS");
                //offSet = offSet - projh2X->GetMean();
                offSet = offSet + g1->GetParameter(1);
                delete projh2X;
                delete g1;
                }
              }
              else
                break;
            }
            for(int is = s-1; is > refStrip; is = is - 2) { 
              nameTH2s = Form("%d%s%s%s_%d_%s%stimeProfileStrip", chamber, filters.c_str(), modParams.c_str(), _strDelim.c_str(),  is+chamber, _strDelim.c_str(),  mods->at(im).c_str()); 
              auto itOffSet = TH2s->find(nameTH2s);
              if(!(itOffSet == TH2s->end())) {
                if(itOffSet->second.ProjectionX("h", is, is, "")->GetEntries() <= thrEntries) {
                   offSet = 0; break;
                }
                else {
                TH1D * projh2X = itOffSet->second.ProjectionX("h", is, is, "");
                double pMean  = projh2X->GetMean();
                double pRMS  = projh2X->GetRMS();
                TF1* g1 = new TF1("g", "gaus",  pMean - 6*pRMS, pMean + 6*pRMS);
                projh2X->Fit("g", "QS");
                //offSet = offSet - projh2X->GetMean();
                offSet = offSet + g1->GetParameter(1);
                delete projh2X;
                delete g1;
                }
              }
              else
                break;
            }
            if(name == "HR")
              offsetsHR.insert(std::make_pair(s+chamber, offSet));
            if(name == "LR")
              offsetsLR.insert(std::make_pair(s+chamber, offSet));
            if(name == "AND")
              offsets.insert(std::make_pair(s+chamber, offSet));
	    	    nameTGEs = Form("%d%s%s_%d_%soffSet", chamber, _strDelim.c_str(), mods->at(im).c_str(), (int)value, _strDelim.c_str());   
            itTGEs = dataRoot->addDqTGE(nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
            TGEs->find(nameTGEs.c_str())->second->setting(s + chamber, 0, offSet, eOffSet);
          }
        }
      }
    }
  }
  event->setTimeOffSetHR(&offsetsHR); 
  event->setTimeOffSetLR(&offsetsLR);
  event->setTimeOffSet(&offsets);
  return true;
}

bool LyPeti::ATime(std::string mod, std::vector<std::string> *mods, 
                  int run, LyPetiEvent *event, std::map<int, LyPetiAnalysis> *analysis,
                  DqRoot *dataRoot,
                  std::map<std::string, DqTGE*> *TGEs, 
                  std::map<std::string, DqTH1D> *TH1s, 
                  std::map<std::string, DqTH2D> *TH2s)
{
  std::string nameTGEs = "";    
  auto itTGEs = TGEs->find("");
  std::string nameTH1s = "";
  auto itTH1s = TH1s->find("");
  std::string nameTH2s = "";
  auto itTH2s = TH2s->find("");
  std::string sR = this->findParam("ATime_R", &_params);
  std::vector<std::string> vsR = _parser.parsArray(sR);
  std::vector<double> R = _parser.strToDoubleArray(vsR);
  	
  std::string sC = this->findParam("ATime_C", &_params);
  std::vector<std::string> vsC = _parser.parsArray(sC);
  std::vector<double> C = _parser.strToDoubleArray(vsC);

	std::string sT = this->findParam("ATime_T", &_params);
  std::vector<std::string> vsT = _parser.parsArray(sT);
  std::vector<double> T = _parser.strToDoubleArray(vsT);

  if(mod == "CONFIG") {

    std::map<int, lyCB::data> RETURN = analysis->find(1000)->second.data();
    std::map<int, lyCB::data> COAX = analysis->find(0)->second.data();
    
    std::vector<std::pair<int,int>> coupes;
    for(auto& itR: RETURN) {
      for(auto& itC: COAX) {
        if(itR.second.HR.size() == itC.second.HR.size() && (itR.first == itC.first || RETURN.size() == COAX.size())) // не совсем правильный выбоp
          coupes.push_back(std::make_pair(itR.first, itC.first));
      }
    }
    for(unsigned int i = 0; i < coupes.size(); i++) {
      double timeC_HR = analysis->find(0)->second.centralTimeCB(&COAX.find(coupes.at(i).first)->second.HR);
      double timeC_LR = analysis->find(0)->second.centralTimeCB(&COAX.find(coupes.at(i).first)->second.LR);
      double timeR_HR = analysis->find(1000)->second.centralTimeCB(&RETURN.find(coupes.at(i).first)->second.HR);
      double timeR_LR = analysis->find(1000)->second.centralTimeCB(&RETURN.find(coupes.at(i).first)->second.LR);
      //double timeC_HR = analysis->find(0)->second.middleTimeCB(&COAX.find(coupes.at(i).first)->second.HR);
      //double timeC_LR = analysis->find(0)->second.middleTimeCB(&COAX.find(coupes.at(i).first)->second.LR);
      //double timeR_HR = analysis->find(1000)->second.middleTimeCB(&RETURN.find(coupes.at(i).first)->second.HR);
      //double timeR_LR = analysis->find(1000)->second.middleTimeCB(&RETURN.find(coupes.at(i).first)->second.LR);
      nameTH2s = Form("500_CorrelationHRLR"); 
	    itTH2s = dataRoot->addDqTH2D(nameTH2s, TH2s); itTH2s->second.config(nameTH2s, &_params);
      itTH2s->second.setting(timeR_HR-timeC_HR, timeR_LR - timeC_LR);
      
			if(COAX.find(coupes.at(i).first)->second.HR.size() == 1 || COAX.find(coupes.at(i).first)->second.HR.size() == 3) {
			//if(true) {
				bool isRC = false; int J = 0;
        double stripR_HR = analysis->find(1000)->second.centralStripCB(&RETURN.find(coupes.at(i).first)->second.HR);
        double stripR_LR = analysis->find(1000)->second.centralStripCB(&RETURN.find(coupes.at(i).first)->second.LR);
        double stripC_HR = analysis->find(0)->second.centralStripCB(&COAX.find(coupes.at(i).first)->second.HR);
        double stripC_LR = analysis->find(0)->second.centralStripCB(&COAX.find(coupes.at(i).first)->second.LR);
				for(unsigned int jj = 0; jj < R.size(); jj++) {
					if(R.at(jj) == stripR_HR && C.at(jj) == stripC_HR) {
						isRC = true;
						J=jj;
						break;
					}
				}
        if(isRC) { 
	    		nameTH1s =  Form("500_TimeResolutionHR"); 
      		itTH1s = dataRoot->addDqTH1D(nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
					itTH1s->second.setting(timeR_HR - timeC_HR);
	    		nameTH1s =  Form("500_%1.f_%1.f_TimeResolutionHR", stripR_HR,  stripC_HR);  
      		itTH1s = dataRoot->addDqTH1D(nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
					itTH1s->second.setting(timeR_HR - timeC_HR);
	    		
					nameTH1s =  Form("500_TimeResolutionLR"); 
      		itTH1s = dataRoot->addDqTH1D(nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
					itTH1s->second.setting(timeR_LR - timeC_LR);
	    		nameTH1s =  Form("500_%1.f_%1.f_TimeResolutionLR", stripR_LR,  stripC_LR);  
      		itTH1s = dataRoot->addDqTH1D(nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
					itTH1s->second.setting(timeR_LR - timeC_LR);
					
          nameTH1s =  Form("500_TimeResolutionHRLR"); 
      		itTH1s = dataRoot->addDqTH1D(nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
					itTH1s->second.setting(((timeR_LR - timeC_LR) + (timeR_HR-timeC_HR))/2);
	    		nameTH1s =  Form("500_%1.f_%1.f_TimeResolutionHRLR", stripR_LR,  stripC_LR);  
      		itTH1s = dataRoot->addDqTH1D(nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
					itTH1s->second.setting(((timeR_LR - timeC_LR) + (timeR_HR-timeC_HR))/2);
				}
      }
      for(unsigned int j = 0; j < RETURN.find(coupes.at(i).first)->second.HR.size(); j++) {
        nameTH2s = Form("500_CorrelationHR"); 
	      itTH2s = dataRoot->addDqTH2D(nameTH2s, TH2s); itTH2s->second.config(nameTH2s, &_params);
        itTH2s->second.setting(RETURN.find(coupes.at(i).first)->second.HR.at(j).first, COAX.find(coupes.at(i).second)->second.HR.at(j).first);
        
				nameTH2s = Form("500_CorrelationLR"); 
	      itTH2s = dataRoot->addDqTH2D(nameTH2s, TH2s); itTH2s->second.config(nameTH2s, &_params);
        itTH2s->second.setting(RETURN.find(coupes.at(i).first)->second.LR.at(j).first, COAX.find(coupes.at(i).second)->second.LR.at(j).first);
        
				nameTH2s = Form("500_CorrelationCBhr"); 
	      itTH2s = dataRoot->addDqTH2D(nameTH2s, TH2s); itTH2s->second.config(nameTH2s, &_params);
        double stripR = analysis->find(1000)->second.centralStripCB(&RETURN.find(coupes.at(i).first)->second.HR);
        double stripC = analysis->find(0)->second.centralStripCB(&COAX.find(coupes.at(i).first)->second.HR);
        itTH2s->second.setting(stripR, stripC);
				
				nameTH2s = Form("500_CorrelationCBlr"); 
	      itTH2s = dataRoot->addDqTH2D(nameTH2s, TH2s); itTH2s->second.config(nameTH2s, &_params);
       	stripR = analysis->find(1000)->second.centralStripCB(&RETURN.find(coupes.at(i).first)->second.LR);
        stripC = analysis->find(0)->second.centralStripCB(&COAX.find(coupes.at(i).first)->second.LR);
        itTH2s->second.setting(stripR, stripC);
      }
    }
  }
  if(mod == "CONFIG END") {
    for(auto& it: *TH1s) it.second.setting();
    for(auto& it: *TH2s) it.second.setting();
  }
  if(mod == "ANALYSIS") {

    std::map<int, lyCB::data> RETURN = analysis->find(1000)->second.data();
    std::map<int, lyCB::data> COAX = analysis->find(0)->second.data();
    std::vector<std::pair<int,int>> coupes;
    for(auto& itR: RETURN) {
      for(auto& itC: COAX) {
        if(itR.second.HR.size() == itC.second.HR.size() && (itR.first == itC.first || RETURN.size() == COAX.size())) // не совсем правильный выбоp
          coupes.push_back(std::make_pair(itR.first, itC.first));
      }
    }
    for(unsigned int i = 0; i < coupes.size(); i++) {
      double timeC_HR = analysis->find(0)->second.centralTimeCB(&COAX.find(coupes.at(i).first)->second.HR);
      double timeC_LR = analysis->find(0)->second.centralTimeCB(&COAX.find(coupes.at(i).first)->second.LR);
      double timeR_HR = analysis->find(1000)->second.centralTimeCB(&RETURN.find(coupes.at(i).first)->second.HR);
      double timeR_LR = analysis->find(1000)->second.centralTimeCB(&RETURN.find(coupes.at(i).first)->second.LR);
      //double timeC_HR = analysis->find(0)->second.middleTimeCB(&COAX.find(coupes.at(i).first)->second.HR);
      //double timeC_LR = analysis->find(0)->second.middleTimeCB(&COAX.find(coupes.at(i).first)->second.LR);
      //double timeR_HR = analysis->find(1000)->second.middleTimeCB(&RETURN.find(coupes.at(i).first)->second.HR);
      //double timeR_LR = analysis->find(1000)->second.middleTimeCB(&RETURN.find(coupes.at(i).first)->second.LR);
      nameTH2s = Form("500_CorrelationHRLR"); 
  		TH2s->find(nameTH2s.c_str())->second.Fill(timeR_HR - timeC_HR, timeR_LR - timeC_LR);
      if(COAX.find(coupes.at(i).first)->second.HR.size() == 1 || COAX.find(coupes.at(i).first)->second.HR.size() == 3) {
			//if(true) {
				bool isRC = false; int J = 0;
        double stripR_HR = analysis->find(1000)->second.centralStripCB(&RETURN.find(coupes.at(i).first)->second.HR);
        double stripR_LR = analysis->find(1000)->second.centralStripCB(&RETURN.find(coupes.at(i).first)->second.LR);
        double stripC_HR = analysis->find(0)->second.centralStripCB(&COAX.find(coupes.at(i).first)->second.HR);
        double stripC_LR = analysis->find(0)->second.centralStripCB(&COAX.find(coupes.at(i).first)->second.LR);
				for(unsigned int jj = 0; jj < R.size(); jj++) {
					if(R.at(jj) == stripR_HR  && C.at(jj) == stripC_HR) {
						isRC = true;
						J=jj;
						break;
					}
				} if(isRC) { 
				//	std::cout << std::setprecision(6) << stripR << " " << stripC << std::endl;
	    		nameTH1s =  Form("500_TimeResolutionHR"); 
					TH1s->find(nameTH1s.c_str())->second.Fill(timeR_HR - timeC_HR);
	    		nameTH1s =  Form("500_%1.f_%1.f_TimeResolutionHR", stripR_HR, stripC_HR);
					TH1s->find(nameTH1s.c_str())->second.Fill(timeR_HR - timeC_HR);
	    		
					nameTH1s =  Form("500_TimeResolutionLR"); 
					TH1s->find(nameTH1s.c_str())->second.Fill(timeR_LR - timeC_LR);
	    		nameTH1s =  Form("500_%1.f_%1.f_TimeResolutionLR", stripR_LR, stripC_LR);
					TH1s->find(nameTH1s.c_str())->second.Fill(timeR_LR - timeC_LR);
					
          nameTH1s =  Form("500_TimeResolutionHRLR"); 
					TH1s->find(nameTH1s.c_str())->second.Fill((((timeR_LR - timeC_LR) + (timeR_HR-timeC_HR))/2));
	    		nameTH1s =  Form("500_%1.f_%1.f_TimeResolutionHRLR", stripR_LR, stripC_LR);
					TH1s->find(nameTH1s.c_str())->second.Fill((((timeR_LR - timeC_LR) + (timeR_HR-timeC_HR))/2));
					}
			}
      for(unsigned int j = 0; j < RETURN.find(coupes.at(i).first)->second.HR.size(); j++) {
        nameTH2s = Form("500_CorrelationHR"); 
  		  TH2s->find(nameTH2s.c_str())->second.Fill(RETURN.find(coupes.at(i).first)->second.HR.at(j).first, COAX.find(coupes.at(i).second)->second.HR.at(j).first);
        nameTH2s = Form("500_CorrelationLR"); 
  		  TH2s->find(nameTH2s.c_str())->second.Fill(RETURN.find(coupes.at(i).first)->second.LR.at(j).first, COAX.find(coupes.at(i).second)->second.LR.at(j).first);

        nameTH2s = Form("500_CorrelationCBhr"); 
        double stripR = analysis->find(1000)->second.centralStripCB(&RETURN.find(coupes.at(i).first)->second.HR);
        double stripC = analysis->find(0)->second.centralStripCB(&COAX.find(coupes.at(i).first)->second.HR);
  		  TH2s->find(nameTH2s.c_str())->second.Fill(stripR, stripC);
        nameTH2s = Form("500_CorrelationCBlr"); 
        stripR = analysis->find(1000)->second.centralStripCB(&RETURN.find(coupes.at(i).first)->second.LR);
        stripC = analysis->find(0)->second.centralStripCB(&COAX.find(coupes.at(i).first)->second.LR);
  		  TH2s->find(nameTH2s.c_str())->second.Fill(stripR, stripC);
      }
    }
    
    
  //nameTH1s =  Form("500&%d_%d&TimeResolution", (int)data1.at(jd1).first,  (int)data2.at(jd2).first); 
  //TH1s->find(nameTH1s.c_str())->second.Fill(data1.at(jd1).second - data2.at(jd2).second);
	//
	//nameTH1s =  Form("500&TimeResolution"); 
  //TH1s->find(nameTH1s.c_str())->second.Fill(data1.at(jd1).second - data2.at(jd2).second - T.at(J));
  }
  if(mod == "ANALYSIS END") {
		//nameTGEs = Form("500&Correlation");   
  	//itTGEs = dataRoot->addDqTGE(nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
  	//std::cout << TH2s->find(nameTH2s.c_str())->second.GetNbinsX() << std::endl;
  	//for(unsigned int c = 1014; c < 1034; c++) {
  	//	TH1D *h = TH2s->find(nameTH2s.c_str())->second.ProjectionY("h", c, c);
  	//	int binmax = h->GetMaximumBin(); double x = h->GetXaxis()->GetBinCenter(binmax);
  	//	TF1 *f1 = new TF1("f1", "gaus", x-2, x+2);
  	//	h->Fit("f1", "R");
  	//	TGEs->find(nameTGEs.c_str())->second->setting(c, 0, f1->GetParameter(1),  f1->GetParameter(2));
		//
		//	nameTH1s =  Form("500&%d_%d&TimeResolution", c,  c+1-1000); 
  	//	int binmax1 = TH1s->find(nameTH1s.c_str())->second.GetMaximumBin(); 
  	//	double x1 = TH1s->find(nameTH1s.c_str())->second.GetXaxis()->GetBinCenter(binmax1);
  	//	TF1 *f2 = new TF1("f2", "gaus", x1-2, x1+2);
  	//	TH1s->find(nameTH1s.c_str())->second.Fit("f2", "R");
  	//}
	}
  else
    return false;
  return true;
}

bool LyPeti::ATimeSingleChamber(std::string mod, std::vector<std::string> *mods, 
                  int run, LyPetiEvent *event, std::map<int, LyPetiAnalysis> *analysis,
                  DqRoot *dataRoot,
                  std::map<std::string, DqTGE*> *TGEs, 
                  std::map<std::string, DqTH1D> *TH1s, 
                  std::map<std::string, DqTH2D> *TH2s)
{
  std::string nameTGEs = "";    
  auto itTGEs = TGEs->find("");
  std::string nameTH1s = "";
  auto itTH1s = TH1s->find("");
  std::string nameTH2s = "";
  auto itTH2s = TH2s->find("");
  if(mod == "CONFIG") {
    std::map<int, lyCB::data> chamber = analysis->find(1000)->second.data();
    for(auto& it: chamber) {
      for(unsigned int j = 0; j < chamber.find(it.first)->second.HR.size(); j++) {
	      //if(chamber.find(it.first)->second.HR.size() == 1 || chamber.find(it.first)->second.HR.size() == 3) {
          double strip_HR = analysis->find(1000)->second.centralStripCB(&chamber.find(it.first)->second.HR);
          double time_HR = analysis->find(1000)->second.middleTimeCB(&chamber.find(it.first)->second.HR);

          nameTH1s =  Form("1010&TimeResolutionHR"); 
          itTH1s = dataRoot->addDqTH1D(nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
			    itTH1s->second.setting(time_HR);
	        nameTH1s =  Form("1010&%1.f&TimeResolutionHR", strip_HR);  
          itTH1s = dataRoot->addDqTH1D(nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
			    itTH1s->second.setting(time_HR);
        //}
      }
      for(unsigned int j = 0; j < chamber.find(it.first)->second.LR.size(); j++) {
	      //if(chamber.find(it.first)->second.LR.size() == 1 || chamber.find(it.first)->second.LR.size() == 3) {
          double strip_LR = analysis->find(1000)->second.centralStripCB(&chamber.find(it.first)->second.LR);
          double time_LR = analysis->find(1000)->second.middleTimeCB(&chamber.find(it.first)->second.LR);
          
          nameTH1s =  Form("1010&TimeResolutionLR"); 
          itTH1s = dataRoot->addDqTH1D(nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
			    itTH1s->second.setting(time_LR);
	        nameTH1s =  Form("1010&%1.f&TimeResolutionLR", strip_LR);  
          itTH1s = dataRoot->addDqTH1D(nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
			    itTH1s->second.setting(time_LR);
        //}
      }
    }
  }
  if(mod == "CONFIG END") {
    // проверка
    for(auto& it: *TH1s) it.second.setting();
    for(auto& it: *TH2s) it.second.setting();
  }
  if(mod == "ANALYSIS") {
    std::map<int, lyCB::data> chamber = analysis->find(1000)->second.data();
    for(auto& it: chamber) {
      for(unsigned int j = 0; j < chamber.find(it.first)->second.HR.size(); j++) {
	      //if(chamber.find(it.first)->second.HR.size() == 1 || chamber.find(it.first)->second.HR.size() == 3) {
          double strip_HR = analysis->find(1000)->second.centralStripCB(&chamber.find(it.first)->second.HR);
          double time_HR = analysis->find(1000)->second.middleTimeCB(&chamber.find(it.first)->second.HR);
          nameTH1s =  Form("1010&TimeResolutionHR"); 
          auto it =  TH1s->find(nameTH1s.c_str());
          if(it != TH1s->end()) TH1s->find(nameTH1s.c_str())->second.Fill(time_HR);
	    		nameTH1s =  Form("1010&%1.f&TimeResolutionHR", strip_HR);
          it =  TH1s->find(nameTH1s.c_str());
          if(it != TH1s->end()) TH1s->find(nameTH1s.c_str())->second.Fill(time_HR);
        //}
      }
      for(unsigned int j = 0; j < chamber.find(it.first)->second.LR.size(); j++) {
	      //if(chamber.find(it.first)->second.LR.size() == 1 || chamber.find(it.first)->second.LR.size() == 3) {
          double strip_LR = analysis->find(1000)->second.centralStripCB(&chamber.find(it.first)->second.LR);
          double time_LR = analysis->find(1000)->second.middleTimeCB(&chamber.find(it.first)->second.LR);
	    		nameTH1s =  Form("1010&TimeResolutionLR"); 
           auto it = TH1s->find(nameTH1s.c_str());
          if(it != TH1s->end()) TH1s->find(nameTH1s.c_str())->second.Fill(time_LR);
	    		nameTH1s =  Form("1010&%1.f&TimeResolutionLR", strip_LR);
          it =  TH1s->find(nameTH1s.c_str());
          if(it != TH1s->end()) TH1s->find(nameTH1s.c_str())->second.Fill(time_LR);
        //}
      }
    }
  }
  if(mod == "ANALYSIS END") {
	  std::cout << "HI " << std::endl;
  }
  else
    return false;
  return true;
}
bool LyPeti::ClusterAlgoTest(std::string mod, std::vector<std::string> *mods, 
                  int run, LyPetiEvent *event, std::map<int, LyPetiAnalysis> *analysis,
                  DqRoot *dataRoot,
                  std::map<std::string, DqTGE*> *TGEs, 
                  std::map<std::string, DqMTGE*> *MTGEs, 
                  std::map<std::string, DqTH1D> *TH1s, 
                  std::map<std::string, DqTH2D> *TH2s)
{
  std::string nameMTGEs = "";    
  auto itMTGEs = MTGEs->find("");
  std::string nameTGEs = "";    
  auto itTGEs = TGEs->find("");
  std::string nameTH1s = "";
  auto itTH1s = TH1s->find("");
  std::string nameTH2s = "";
  auto itTH2s = TH2s->find("");
  if(mod == "CONFIG") {
    std::map<int, lyCB::data> chamber = analysis->find(1000)->second.data();
    std::vector<std::pair<int, double>> sortTime;
    for(auto& it: chamber) {
      double time = analysis->find(1000)->second.firstTimeCB(&chamber.find(it.first)->second.HR);
      double index = it.first;
      sortTime.push_back(std::pair<int, double>((double)index, time));
    }
	  std::sort(sortTime.begin(), sortTime.end(), [](auto &left, auto &right) {
      return std::abs(left.second) < std::abs(right.second);
	  });
    for(int i = 0; i < sortTime.size(); i++) {
      for(int s = 2; s < 20; s++) {
        if(chamber.size() == s) {
          int nEvent = event->trigs(1000)->at(0)->head->number;
          for(unsigned int j = 0; j < chamber.find(sortTime.at(i).first)->second.HR.size(); j++) {
            double strip_HR = chamber.find(sortTime.at(i).first)->second.HR.at(j).first;
            double time_HR  = chamber.find(sortTime.at(i).first)->second.HR.at(j).second;
            nameTH2s = Form("1000_CTstripTimeHR%ssize%d%s%d", _strDelim.c_str(), s, _strDelim.c_str(), i); 
	          itTH2s = dataRoot->addDqTH2D(nameTH2s, TH2s); itTH2s->second.config(nameTH2s, &_params);
            itTH2s->second.setting(time_HR, strip_HR);
          }
          for(unsigned int j = 0; j < chamber.find(sortTime.at(i).first)->second.LR.size(); j++) {
            double strip_LR = chamber.find(sortTime.at(i).first)->second.LR.at(j).first;
            double time_LR  = chamber.find(sortTime.at(i).first)->second.LR.at(j).second;
            nameTH2s = Form("1000_CTstripTimeLR%ssize%d%s%d", _strDelim.c_str(), s, _strDelim.c_str(), i); 
	          itTH2s = dataRoot->addDqTH2D(nameTH2s, TH2s); itTH2s->second.config(nameTH2s, &_params);
            itTH2s->second.setting(time_LR, strip_LR);
          }
          double strip = chamber.find(sortTime.at(i).first)->second.firstStrip;
          double time = chamber.find(sortTime.at(i).first)->second.firstTime;
          nameTH2s = Form("1000_CTstripTimeCB%ssize%d%s%d", _strDelim.c_str(), s, _strDelim.c_str(), i); 
	        itTH2s = dataRoot->addDqTH2D(nameTH2s, TH2s); itTH2s->second.config(nameTH2s, &_params);
          itTH2s->second.setting(time, strip);
        }
      }
    }
  }
  if(mod == "CONFIG END") {
    // проверка
    for(auto& it: *TH1s) it.second.setting();
    for(auto& it: *TH2s) it.second.setting();
  }
  if(mod == "ANALYSIS") {
    std::map<int, lyCB::data> chamber = analysis->find(1000)->second.data();
    std::vector<std::pair<int, double>> sortTime;
    for(auto& it: chamber) {
      double time = analysis->find(1000)->second.firstTimeCB(&chamber.find(it.first)->second.HR);
      double index = it.first;
      sortTime.push_back(std::pair<int, double>((double)index, time));
    }
	  std::sort(sortTime.begin(), sortTime.end(), [](auto &left, auto &right) {
      return std::abs(left.second) < std::abs(right.second);
	  });
    for(int i = 0; i < sortTime.size(); i++) {
      for(int s = 2; s < 20; s++) {
        if(chamber.size() == s) {
          int nEvent = event->trigs(1000)->at(0)->head->number;
          nameMTGEs = Form("1000_mgrCTstripTimeHR%ssize%d%s%d", _strDelim.c_str(), s, _strDelim.c_str(), nEvent);   
          itMTGEs = dataRoot->addDqMTGE(nameMTGEs, MTGEs); itMTGEs->second->config(nameMTGEs, &_params);
          for(unsigned int j = 0; j < chamber.find(sortTime.at(i).first)->second.HR.size(); j++) {
            double strip_HR = chamber.find(sortTime.at(i).first)->second.HR.at(j).first;
            double time_HR  = chamber.find(sortTime.at(i).first)->second.HR.at(j).second;
            nameTH2s = Form("1000_CTstripTimeHR%ssize%d%s%d", _strDelim.c_str(), s, _strDelim.c_str(), i); 
            auto it = TH2s->find(nameTH2s.c_str());
  		      if(it != TH2s->end()) TH2s->find(nameTH2s.c_str())->second.Fill(time_HR, strip_HR);
    		    
            nameTGEs = Form("1000_grCTstripTimeHR%ssize%d%s%d%s%d", _strDelim.c_str(), s, _strDelim.c_str(), i, _strDelim.c_str(), nEvent);   
            itTGEs = dataRoot->addDqTGE(nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
            TGEs->find(nameTGEs.c_str())->second->setting(time_HR, 0, strip_HR, 0);
          }
          TGEs->find(nameTGEs.c_str())->second->plot();
          MTGEs->find(nameMTGEs.c_str())->second->addDqTGE(TGEs->find(nameTGEs.c_str())->second);
          
          nameMTGEs = Form("1000_mgrCTstripTimeLR%ssize%d%s%d", _strDelim.c_str(), s, _strDelim.c_str(), nEvent);   
          itMTGEs = dataRoot->addDqMTGE(nameMTGEs, MTGEs); itMTGEs->second->config(nameMTGEs, &_params);
          for(unsigned int j = 0; j < chamber.find(sortTime.at(i).first)->second.LR.size(); j++) {
            double strip_LR = chamber.find(sortTime.at(i).first)->second.LR.at(j).first;
            double time_LR  = chamber.find(sortTime.at(i).first)->second.LR.at(j).second;
            nameTH2s = Form("1000_CTstripTimeLR%ssize%d%s%d", _strDelim.c_str(), s, _strDelim.c_str(), i); 
            auto it = TH2s->find(nameTH2s.c_str());
  		      if(it != TH2s->end()) TH2s->find(nameTH2s.c_str())->second.Fill(time_LR, strip_LR);
    		    
            nameTGEs = Form("1000_grCTstripTimeLR%ssize%d%s%d%s%d", _strDelim.c_str(), s, _strDelim.c_str(), i, _strDelim.c_str(), nEvent);   
            itTGEs = dataRoot->addDqTGE(nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
            TGEs->find(nameTGEs.c_str())->second->setting(time_LR, 0, strip_LR, 0);

          }
          TGEs->find(nameTGEs.c_str())->second->plot();
          MTGEs->find(nameMTGEs.c_str())->second->addDqTGE(TGEs->find(nameTGEs.c_str())->second);

          double strip = chamber.find(sortTime.at(i).first)->second.firstStrip;
          double time = chamber.find(sortTime.at(i).first)->second.firstTime;
          nameTH2s = Form("1000_CTstripTimeCB%ssize%d%s%d", _strDelim.c_str(), s, _strDelim.c_str(), i); 
          auto it = TH2s->find(nameTH2s.c_str());
  		    if(it != TH2s->end()) TH2s->find(nameTH2s.c_str())->second.Fill(time, strip);
        }
      }
    }
  }
  if(mod == "ANALYSIS END") {
  }
  else
    return false;
  return true;
}
  bool LyPeti::core(std::string mod, std::vector<std::string> *mods, 
                  int run, LyPetiEvent *event, std::map<int, LyPetiAnalysis> *analysis,
                  DqRoot *dataRoot,
                  std::map<std::string, DqTGE*> *TGEs, 
                  std::map<std::string, DqTH1D> *TH1s, 
                  std::map<std::string, DqTH2D> *TH2s)
{
  std::string nameTGEs = "";    
  auto itTGEs = TGEs->find("");
  std::string nameTH1s = "";
  auto itTH1s = TH1s->find("");
  std::string nameTH2s = "";
  auto itTH2s = TH2s->find("");
  if(mod == "CONFIG") {
    // FIRST ANALYSIS BEGIN ------------------------------------------------
    for(unsigned int ic = 0; ic < event->chambers()->size(); ic++) {
      int chamber = event->chambers()->at(ic);
      analysis->find(chamber)->second.setStrips(event->strips(chamber));
      
      // TRIGS
      for(unsigned int jt = 0; jt < event->trigs(chamber)->size(); jt++) {
        int boardID = event->trigs(chamber)->at(jt)->head->boardID; 
        nameTH1s = Form("%d%snum%d%sbuffer", chamber, _strDelim.c_str(), boardID, _strDelim.c_str());
        itTH1s = dataRoot->addDqTH1D(nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
        itTH1s->second.setting(event->trigs(chamber)->at(jt)->head->numOfCh);
        //---
        nameTH1s = Form("%d%snum%d%strigBCID", chamber, _strDelim.c_str(), boardID,_strDelim.c_str());
        itTH1s = dataRoot->addDqTH1D(nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
        itTH1s->second.setting(event->trigs(chamber)->at(jt)->BCID);
      }
    
      unsigned int numberOfTrigers = event->trigs(chamber)->size();
      nameTH1s = Form("%d%sNumberOfTrigers", chamber, _strDelim.c_str()); 
      itTH1s = dataRoot->addDqTH1D(nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
      itTH1s->second.setting(numberOfTrigers);
      //--- 
      
      bool isTrigers = analysis->find(chamber)->second.fillTrigers(numberOfTrigers);
    	if(isTrigers) {
        for(unsigned int js = 0; js < event->strips(chamber)->size(); js++) {
         
          // HR
          double numberOfHitsHR = event->strips(chamber)->at(js).HR.size(); 
          nameTH1s = Form("%d%sNumberOfHitsHR", chamber, _strDelim.c_str()); 
          itTH1s = dataRoot->addDqTH1D(nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
          itTH1s->second.setting(numberOfHitsHR);
          // LR
        	double numberOfHitsLR = event->strips(chamber)->at(js).LR.size(); 
          nameTH1s = Form("%d%sNumberOfHitsLR", chamber, _strDelim.c_str()); 
          itTH1s = dataRoot->addDqTH1D(nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
          itTH1s->second.setting(numberOfHitsLR);
          //if(numberOfHitsLR == 2) {
          //  std::cout << event->header(chamber)->number << " s:" << event->strips(chamber)->at(js).number << " HitsPerEventLR=" << numberOfHitsLR << " "; 
          //  for(int in = 0; in < numberOfHitsLR; in++) {
          //    double time = event->strips(chamber)->at(js).LR.at(in)->time -  event->strips(chamber)->at(js).trigs.at(in)->time;
          //    std::cout << std::setprecision(6) << time << " "; 
          //                  
          //  }
          //  std::cout << std::endl;
          //}
          //if(numberOfHitsHR == 2) {
          //  std::cout << event->header(chamber)->number << " HitsPerEventHR=" << numberOfHitsHR << " "; 
          //  for(int in = 0; in < numberOfHitsHR; in++) {
          //    double time = event->strips(chamber)->at(js).HR.at(in)->time -  event->strips(chamber)->at(js).trigs.at(0)->time;
          //    std::cout << std::setprecision(6) << time << " "; 
          //                  
          //  }
          //  std::cout << std::endl;
          //}
          //--- 
         
          // Fill raw data with maping. 
          for(unsigned int jh = 0; jh < event->strips(chamber)->at(js).HR.size(); jh++) {
            bool isFilterHR = analysis->find(chamber)->second.filters("data_HR", js, jh); 
            if(isFilterHR) 
              analysis->find(chamber)->second.fillEvents("data_HR", js, jh); 
          bool isFilterNoiseHR = analysis->find(chamber)->second.filters("noise_HR", js, jh); 
          if(isFilterNoiseHR) 
            analysis->find(chamber)->second.fillEvents("noise_HR", js, jh); 
          analysis->find(chamber)->second.filters("deadTime_HR", js, jh); // skip if deadTime 
          }
          
          for(unsigned int jl = 0; jl < event->strips(chamber)->at(js).LR.size(); jl++) {
            bool isFilterLR = analysis->find(chamber)->second.filters("data_LR", js, jl); 
            if(isFilterLR)
              analysis->find(chamber)->second.fillEvents("data_LR", js, jl); 
            bool isFilterNoiseLR = analysis->find(chamber)->second.filters("noise_LR", js, jl); 
            if(isFilterNoiseLR) 
              analysis->find(chamber)->second.fillEvents("noise_LR", js, jl); 
          analysis->find(chamber)->second.filters("deadTime_LR", js, jl); // skip if deadTime 
     	  	}
    	  }
      }
      if(!analysis->find(chamber)->second.skipEvent()) {  
        // ANALYSIS PART
        std::string filters = analysis->find(chamber)->second.filtersStr(); 
        for(unsigned int im = 0; im < mods->size(); im++) {
          analysis->find(chamber)->second.algos(mods->at(im));
          std::string modParams = analysis->find(chamber)->second.modParamsStr(mods->at(im)); 
          std::vector<std::pair<double, double>> data = analysis->find(chamber)->second.data(mods->at(im).c_str());
          for(unsigned int jd = 0; jd < data.size(); jd++) { 
            nameTH1s = Form("%d%s%s%s%sstripProfile", chamber, filters.c_str(), modParams.c_str(), _strDelim.c_str(), mods->at(im).c_str()); 
            itTH1s = dataRoot->addDqTH1D(nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
            itTH1s->second.setting(data.at(jd).first);
            //---
            nameTH1s = Form("%d%s%s%s%stimeProfile", chamber, filters.c_str(), modParams.c_str(), _strDelim.c_str(), mods->at(im).c_str()); 
            itTH1s = dataRoot->addDqTH1D(nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
            itTH1s->second.setting(data.at(jd).second);
          
            nameTH2s = Form("%d%s%s%s%sstripTimeProfile", chamber, filters.c_str(), modParams.c_str(), _strDelim.c_str(), mods->at(im).c_str()); 
            itTH2s = dataRoot->addDqTH2D(nameTH2s, TH2s); itTH2s->second.config(nameTH2s, &_params);
            itTH2s->second.setting(data.at(jd).second, data.at(jd).first);
            
            nameTH1s = Form("%d%s%s%s%d%s%sTimeProfile", chamber, filters.c_str(), modParams.c_str(), _strDelim.c_str(), (int)data.at(jd).first, _strDelim.c_str(), mods->at(im).c_str()); 
            itTH1s = dataRoot->addDqTH1D(nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
            itTH1s->second.setting(data.at(jd).second);
          }
          
          double minTime = 0; bool isMinTime = analysis->find(chamber)->second.minTime(&minTime, mods->at(im).c_str());
          if(isMinTime) {
            nameTH1s = Form("%d%s%s%s%sminTime", chamber, filters.c_str(), modParams.c_str(), _strDelim.c_str(), mods->at(im).c_str()); 
            itTH1s = dataRoot->addDqTH1D(nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
            itTH1s->second.setting(minTime);
          }
          double maxTime = 0; bool isMaxTime = analysis->find(chamber)->second.maxTime(&maxTime, mods->at(im).c_str());
          if(isMaxTime) {
            nameTH1s = Form("%d%s%s%s%smaxTime", chamber, filters.c_str(), modParams.c_str(), _strDelim.c_str(), mods->at(im).c_str()); 
            itTH1s = dataRoot->addDqTH1D(nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
            itTH1s->second.setting(maxTime);
          }
          int mult = 0; bool isMult = analysis->find(chamber)->second.multiplicity(&mult, mods->at(im).c_str());
          if(isMult) {
            nameTH1s = Form("%d%s%s%s%smultiplicity", chamber, filters.c_str(), modParams.c_str(), _strDelim.c_str(), mods->at(im).c_str()); 
            itTH1s = dataRoot->addDqTH1D(nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
            itTH1s->second.setting(mult);
          }
          std::vector<int> clusterSize; bool isClusterSize = analysis->find(chamber)->second.clusterSize(&clusterSize, mods->at(im).c_str());
          if(isClusterSize) {
            nameTH1s = Form("%d%s%s%s%sclusterSize", chamber, filters.c_str(), modParams.c_str(), _strDelim.c_str(), mods->at(im).c_str()); 
            itTH1s = dataRoot->addDqTH1D(nameTH1s, TH1s); itTH1s->second.config(nameTH1s, &_params);
            for( unsigned int i = 0; i < clusterSize.size(); i++)
              itTH1s->second.setting(clusterSize.at(i));
            clusterSize.clear();
          }
          // --
        }
      }
    }
  }
  if(mod == "CONFIG END") {
    for(auto& it: *TH1s) it.second.setting();
    for(auto& it: *TH2s) it.second.setting();
  }
  if(mod == "ANALYSIS") {

    for(unsigned int ic = 0; ic < event->chambers()->size(); ic++) {
      int chamber = event->chambers()->at(ic); 
      analysis->find(chamber)->second.setStrips(event->strips(chamber));
    
      // TRIGS
      for(unsigned int jt = 0; jt < event->trigs(chamber)->size(); jt++) {
        int boardID = event->trigs(chamber)->at(jt)->head->boardID; 
        nameTH1s = Form("%d%snum%d%sbuffer", chamber, _strDelim.c_str(), boardID, _strDelim.c_str());
        TH1s->find(nameTH1s.c_str())->second.Fill(event->trigs(chamber)->at(jt)->head->numOfCh);
        //---
        nameTH1s = Form("%d%snum%d%strigBCID", chamber, _strDelim.c_str(), boardID,_strDelim.c_str());
        TH1s->find(nameTH1s.c_str())->second.Fill(event->trigs(chamber)->at(jt)->BCID);
      }
      unsigned int numberOfTrigers = event->trigs(chamber)->size();
      nameTH1s = Form("%d%sNumberOfTrigers", chamber, _strDelim.c_str()); 
      TH1s->find(nameTH1s.c_str())->second.Fill(numberOfTrigers);
     
      bool isTrigers = analysis->find(chamber)->second.fillTrigers(numberOfTrigers);
    	if(isTrigers) {
        for(unsigned int js = 0; js < event->strips(chamber)->size(); js++) {
          // HR
          double numberOfHitsHR = event->strips(chamber)->at(js).HR.size(); 
          nameTH1s = Form("%d%sNumberOfHitsHR", chamber, _strDelim.c_str()); 
          TH1s->find(nameTH1s.c_str())->second.Fill(numberOfHitsHR);
          double numberOfHitsLR = event->strips(chamber)->at(js).LR.size();
          nameTH1s = Form("%d%sNumberOfHitsLR", chamber, _strDelim.c_str()); 
          TH1s->find(nameTH1s.c_str())->second.Fill(numberOfHitsLR);
          for(unsigned int jh = 0; jh < event->strips(chamber)->at(js).HR.size(); jh++) {
            bool isFilterHR = analysis->find(chamber)->second.filters("data_HR", js, jh); 
            if(isFilterHR)
              analysis->find(chamber)->second.fillEvents("data_HR", js, jh); 
            bool isFilterNoiseHR = analysis->find(chamber)->second.filters("noise_HR", js, jh); 
            if(isFilterNoiseHR) 
              analysis->find(chamber)->second.fillEvents("noise_HR", js, jh); 
            
            analysis->find(chamber)->second.filters("deadTime_HR", js, jh); // skip if deadTime 
          }
          // LR
     		  for(unsigned int jl = 0; jl < event->strips(chamber)->at(js).LR.size(); jl++) {
            bool isFilterLR = analysis->find(chamber)->second.filters("data_LR", js, jl); 
            if(isFilterLR)
              analysis->find(chamber)->second.fillEvents("data_LR", js, jl); 
            bool isFilterNoiseLR = analysis->find(chamber)->second.filters("noise_LR", js, jl); 
            if(isFilterNoiseLR) 
              analysis->find(chamber)->second.fillEvents("noise_LR", js, jl); 
            
            analysis->find(chamber)->second.filters("deadTime_LR", js, jl); // skip if deadTime 
     		  }
        }
    	}
      if(!analysis->find(chamber)->second.skipEvent()) {  
        // ANALYSIS PART
        std::string filters = analysis->find(chamber)->second.filtersStr(); 
        for(unsigned int im = 0; im < mods->size(); im++) {
          //std::cout << mods->at(im).c_str() << std::endl;
          analysis->find(chamber)->second.algos(mods->at(im));
          std::string modParams = analysis->find(chamber)->second.modParamsStr(mods->at(im)); 
          std::vector<std::pair<double, double>> data = analysis->find(chamber)->second.data(mods->at(im).c_str());
          for(unsigned int jd = 0; jd < data.size(); jd++) { 
            nameTH1s = Form("%d%s%s%s%sstripProfile", chamber, filters.c_str(), modParams.c_str(), _strDelim.c_str(), mods->at(im).c_str()); 
            TH1s->find(nameTH1s.c_str())->second.Fill(data.at(jd).first);
            //--- 
            nameTH1s = Form("%d%s%s%s%stimeProfile", chamber, filters.c_str(), modParams.c_str(), _strDelim.c_str(), mods->at(im).c_str()); 
            TH1s->find(nameTH1s.c_str())->second.Fill(data.at(jd).second);
            
            nameTH2s = Form("%d%s%s%s%sstripTimeProfile", chamber, filters.c_str(), modParams.c_str(), _strDelim.c_str(), mods->at(im).c_str()); 
            TH2s->find(nameTH2s.c_str())->second.Fill(data.at(jd).second, data.at(jd).first);
            
            //nameTH1s = Form("%d%s%s%s%d%s%sTimeProfile", chamber, filters.c_str(), modParams.c_str(), _strDelim.c_str(), (int)data.at(jd).first, _strDelim.c_str(), mods->at(im).c_str()); 
            //TH1s->find(nameTH1s.c_str())->second.Fill(data.at(jd).second);
          }
          double minTime = 0; bool isMinTime = analysis->find(chamber)->second.minTime(&minTime, mods->at(im).c_str());
          if(isMinTime) {
            nameTH1s = Form("%d%s%s%s%sminTime", chamber, filters.c_str(), modParams.c_str(), _strDelim.c_str(), mods->at(im).c_str()); 
            TH1s->find(nameTH1s.c_str())->second.Fill(minTime);
          }
          double maxTime = 0; bool isMaxTime = analysis->find(chamber)->second.maxTime(&maxTime, mods->at(im).c_str());
          if(isMaxTime) {
            nameTH1s = Form("%d%s%s%s%smaxTime", chamber, filters.c_str(), modParams.c_str(), _strDelim.c_str(), mods->at(im).c_str()); 
            TH1s->find(nameTH1s.c_str())->second.Fill(maxTime);
          }
          int mult = 0; bool isMult = analysis->find(chamber)->second.multiplicity(&mult, mods->at(im).c_str());
          if(isMult) {
            nameTH1s = Form("%d%s%s%s%smultiplicity", chamber, filters.c_str(), modParams.c_str(), _strDelim.c_str(), mods->at(im).c_str()); 
            TH1s->find(nameTH1s.c_str())->second.Fill(mult);
          }
          std::vector<int> clusterSize; bool isClusterSize = analysis->find(chamber)->second.clusterSize(&clusterSize, mods->at(im).c_str());
          if(isClusterSize) {
            nameTH1s = Form("%d%s%s%s%sclusterSize", chamber, filters.c_str(), modParams.c_str(), _strDelim.c_str(), mods->at(im).c_str()); 
            auto itCS = TH1s->find(nameTH1s.c_str());
            for(unsigned int i = 0; i < clusterSize.size(); i++)
              itCS->second.Fill(clusterSize.at(i));
            clusterSize.clear();
          }
    	  	bool isEff = analysis->find(chamber)->second.efficiency(mods->at(im).c_str()); // count events
          if(isEff) {
    	  		nameTGEs = Form("%d%s%sefficiency", chamber, _strDelim.c_str(), mods->at(im).c_str());   
          	itTGEs = dataRoot->addDqTGE(nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
    	    }
        }
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
        
        //// time fit
        //nameTH1s = Form("%d%s%s%s%stimeProfile", chamber, filters.c_str(), modParams.c_str(), _strDelim.c_str(), mods->at(im).c_str()); 
        //auto itMult = TH1s->find(nameTH1s);
        //if(!(itMult == TH1s->end())) {
        //  std::map<std::string, double> args; args.insert(std::pair<std::string, double>("nRMS", 6));
        //  TH1s->find(nameTH1s.c_str())->second.fit("gaus", &args);
        //}

      }
    }

    for(unsigned int ic = 0; ic < event->chambers()->size(); ic++) {
      int chamber = event->chambers()->at(ic); 
      std::string filters = analysis->find(chamber)->second.filtersStr(); 
      double numSkipedTrigers = 0; double eNumSkipedTrigers = 0; bool isNumSkipedTrigers = analysis->find(chamber)->second.getNumSkipedTrigers(&numSkipedTrigers); 
    	if(isNumSkipedTrigers) {
    	  double value = analysis->find(chamber)->second.getValue(run); double eValue = 0;
    		nameTGEs = Form("%d%snumberOfSkipedEventsWithTriger", chamber,  _strDelim.c_str());   
        itTGEs = dataRoot->addDqTGE(nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
        TGEs->find(nameTGEs.c_str())->second->setting(value, eValue, numSkipedTrigers, eNumSkipedTrigers);
    	}
    	double numTrigers = 0; double eNumTrigers = 0; bool isNumTrigers = analysis->find(chamber)->second.getNumTrigers(&numTrigers); 
    	if(isNumTrigers) {
    	  double value = analysis->find(chamber)->second.getValue(run); double eValue = 0;
    		nameTGEs = Form("%d%snumberOfEventsWithTriger", chamber,  _strDelim.c_str());   
        itTGEs = dataRoot->addDqTGE(nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
        TGEs->find(nameTGEs.c_str())->second->setting(value, eValue, numTrigers, eNumTrigers);
    	}
    	int numEvents = analysis->find(chamber)->second.getNumberEvents();
    	double value = analysis->find(chamber)->second.getValue(run); double eValue = 0;
    	nameTGEs = Form("%d%snumberOfEvents", chamber, _strDelim.c_str());   
      itTGEs = dataRoot->addDqTGE(nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
      TGEs->find(nameTGEs.c_str())->second->setting(value, eValue, numEvents, 0);
      for(unsigned int im = 0; im < mods->size(); im++) {
         std::string modParams = analysis->find(chamber)->second.modParamsStr(mods->at(im)); 
    		double eff = 0; double eEff = 0; bool isEff = analysis->find(chamber)->second.efficiency(&eff, &eEff, mods->at(im).c_str()); 
    		if(isEff) {
    			double value = analysis->find(chamber)->second.getValue(run); double eValue = 0;
    			nameTGEs = Form("%d%s%sefficiency", chamber, _strDelim.c_str(), mods->at(im).c_str());   
          TGEs->find(nameTGEs.c_str())->second->setting(value, eValue, eff, eEff);
    		}
    		double fraction = 0; bool isFraction = analysis->find(chamber)->second.eventFraction(&fraction, mods->at(im).c_str());
    		if(isFraction) {
    			double value = analysis->find(chamber)->second.getValue(run); double eValue = 0;
    			nameTGEs = Form("%d%s%sfraction", chamber, _strDelim.c_str(), mods->at(im).c_str());   
          itTGEs = dataRoot->addDqTGE(nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
          TGEs->find(nameTGEs.c_str())->second->setting(value, eValue, fraction, 0);
    		}
    		double noise = 0; double eNoise = 0; bool isNoise = analysis->find(chamber)->second.noise(&noise, &eNoise, -1, mods->at(im).c_str());
    		eNoise = 0;
    		if(isNoise) {
    			double value = analysis->find(chamber)->second.getValue(run); double eValue = 0;
    			nameTGEs = Form("%d%s%srate", chamber, _strDelim.c_str(), mods->at(im).c_str());   
          itTGEs = dataRoot->addDqTGE(nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
          TGEs->find(nameTGEs.c_str())->second->setting(value, eValue, noise, eNoise);
    		}

    		nameTH1s = Form("%d%s%s%s%smultiplicity", chamber, filters.c_str(), modParams.c_str(), _strDelim.c_str(), mods->at(im).c_str());
        auto itMult = TH1s->find(nameTH1s);
        if(!(itMult == TH1s->end())) {
          double mult = itMult->second.GetMean(); double eMult = itMult->second.GetRMS()/std::sqrt(itMult->second.GetEntries());
    		  double value = analysis->find(chamber)->second.getValue(run); double eValue = 0;
    		  nameTGEs = Form("%d%s%smultiplicity", chamber, _strDelim.c_str(), mods->at(im).c_str());   
          itTGEs = dataRoot->addDqTGE(nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
          TGEs->find(nameTGEs.c_str())->second->setting(value, eValue, mult, eMult);
        }
    		nameTH1s = Form("%d%s%s%s%sclusterSize", chamber, filters.c_str(), modParams.c_str(), _strDelim.c_str(), mods->at(im).c_str());
        auto itClusterSize = TH1s->find(nameTH1s);
        if(!(itClusterSize == TH1s->end())) {
          double clusterSize = itClusterSize->second.GetMean(); double eClusterSize = itClusterSize->second.GetRMS(itClusterSize->second.GetEntries());
    		  double value = analysis->find(chamber)->second.getValue(run); double eValue = 0;
    		  nameTGEs = Form("%d%s%sclusterSize", chamber, _strDelim.c_str(), mods->at(im).c_str());   
          itTGEs = dataRoot->addDqTGE(nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
          TGEs->find(nameTGEs.c_str())->second->setting(value, eValue, clusterSize, eClusterSize);
        }
    		nameTH1s = Form("%d%sNumberOfHits%s", chamber, _strDelim.c_str(), mods->at(im).c_str());
        auto itMHR = TH1s->find(nameTH1s);
        if(!(itMHR == TH1s->end())) {
          double mult = itMHR->second.GetEntries(); double eMult = 0;
    		  double value = analysis->find(chamber)->second.getValue(run); double eValue = 0;
    		  nameTGEs = Form("%d%s%sNumberOfHits", chamber, _strDelim.c_str(), mods->at(im).c_str());   
          itTGEs = dataRoot->addDqTGE(nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
          TGEs->find(nameTGEs.c_str())->second->setting(value, eValue, mult, eMult);
        }
        //nameTH2s = Form("%d%s%s%s%sstripTimeProfile", chamber, filters.c_str(), modParams.c_str(), _strDelim.c_str(), mods->at(im).c_str()); 
        //auto itFitST = TH2s->find(nameTH2s);
        //if(!(itFitST == TH2s->end())) {
        //  double value = analysis->find(chamber)->second.getValue(run); 
        //  nameTGEs = Form("%d%s%d%s%sgStripTimeProfile", chamber, _strDelim.c_str(), (int)value, _strDelim.c_str(), mods->at(im).c_str());   
        //  itTGEs = dataRoot->addDqTGE(nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
        // // std::cout << TH2s->find(nameTH2s.c_str())->second.GetNbinsX() << std::endl;
        //    int strips = event->numberOfStrips(chamber);
        //    std::cout << mods->at(im).c_str() << "|";
        //    for(unsigned int c = 0; c < strips; c++) {
        //    nameTH1s = Form("%d%s%s%s%d%s%sTimeProfile", chamber, filters.c_str(), modParams.c_str(), _strDelim.c_str(), c, _strDelim.c_str(), mods->at(im).c_str()); 
        //    //if(TH2s->find(nameTH2s.c_str())->second.ProjectionX("h", c, c)->GetEntries() > 100) {
        //      double mean = TH1s->find(nameTH1s.c_str())->second.GetMean(); double rms = TH1s->find(nameTH1s.c_str())->second.GetRMS();
        //      //TF1 *f1 = new TF1("f1", "gaus", mean-5, mean+5);
        //      //h->Fit("f1", "R");
        //      //TGEs->find(nameTGEs.c_str())->second->setting(c, 0, f1->GetParameter(1),  f1->GetParError(1));
        //      TGEs->find(nameTGEs.c_str())->second->setting(c, 0, mean,  rms);
        //    //}
        //      std::cout << std::setprecision(5) << mean << "|";
        //  }
        //  std::cout << std::endl;
        //}
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
                     std::map<std::string, DqMTGE*> *MTGEs, 
                     std::map<std::string, DqTH1D> *TH1s, 
                     std::map<std::string, DqTH2D> *TH2s)
{
    LyPetiRead stream; bool isOpen = false; bool isClose = false;
   
      std::map<int, TTree*> treesHR;
      std::map<int, TTree*> treesLR;
      Int_t number_HR; Float_t time_HR; Float_t triger_HR; Float_t ch_HR; Int_t size_HR; Float_t dtime_HR; uint16_t BCID_HR; uint64_t coarse_HR; uint8_t fine_HR; 
      Int_t number_LR; Float_t time_LR; Float_t triger_LR; Float_t ch_LR; Int_t size_LR; Float_t dtime_LR; uint16_t BCID_LR; uint64_t coarse_LR; uint8_t fine_LR; 
    for(unsigned int ic = 0; ic < event->chambers()->size(); ic++) {
      int chamber = event->chambers()->at(ic); 
      
      std::string pathRAW = Form("%s_%s/RAWDataHR", _comment.c_str(), _logRunNumber.c_str()); 
      
      treesHR.insert(std::make_pair(chamber, new TTree{Form("%d_%d", chamber, run.first), pathRAW.c_str()}));
      treesHR.find(chamber)->second->Branch("number_HR", &number_HR,"number_HR/I");
      
      treesHR.find(chamber)->second->Branch("ch_HR", &ch_HR,"ch_HR/F");
      treesHR.find(chamber)->second->Branch("size_HR", &size_HR,"size_HR/I");
     
      treesHR.find(chamber)->second->Branch("BCID_HR", &BCID_HR,"BCID_HR/I");
      treesHR.find(chamber)->second->Branch("coarse_HR", &coarse_HR,"coarse_HR/I");
      treesHR.find(chamber)->second->Branch("fine_HR", &fine_HR,"fine_HR/I");

      treesHR.find(chamber)->second->Branch("time_HR", &time_HR,"time_HR/F");
      treesHR.find(chamber)->second->Branch("triger_HR", &triger_HR,"triger_HR/F");
      treesHR.find(chamber)->second->Branch("dtime_HR", &dtime_HR,"dtime_HR/F");
      // ---
      pathRAW = Form("%s_%s/RAWDataLR", _comment.c_str(), _logRunNumber.c_str()); 
      
      treesLR.insert(std::make_pair(chamber, new TTree{Form("%d_%d", chamber, run.first), pathRAW.c_str()}));
      treesLR.find(chamber)->second->Branch("number_LR", &number_LR,"number_LR/I");
      
      treesLR.find(chamber)->second->Branch("ch_LR", &ch_LR,"ch_LR/F");
      treesLR.find(chamber)->second->Branch("size_LR", &size_HR,"size_LR/I");
      
      treesLR.find(chamber)->second->Branch("BCID_LR", &BCID_LR,"BCID_LR/I");
      treesLR.find(chamber)->second->Branch("coarse_LR", &coarse_LR,"coarse_LR/I");
      treesLR.find(chamber)->second->Branch("fine_LR", &fine_LR,"fine_LR/I");
      
      treesLR.find(chamber)->second->Branch("time_LR", &time_LR,"time_LR/F");
      treesLR.find(chamber)->second->Branch("triger_LR", &triger_HR,"triger_HR/F");
      treesLR.find(chamber)->second->Branch("dtime_LR", &dtime_LR,"dtime_LR/F");
    }
    
    std::vector<std::string> loops;
    if(run.second == 0) {
      loops.push_back("CONFIG"); loops.push_back("ANALYSIS");
    }
    else if(run.second > 0) {
      loops.push_back("OFFSET" + _strDelim + "CONFIG"); 
      loops.push_back("OFFSET" + _strDelim +  "ANALYSIS");
    }

    for(unsigned int l = 0; l < loops.size(); l++) {
      std::cout << loops.at(l) << " loop..." << std::endl;

      bool isFindRun = false;
		  for(unsigned int i = 0; i < _inputs.size(); i++) {
		  	
        if(run.first == this->numRun(_parser.fileName(_inputs.at(i))))
          isFindRun = true;
        if(run.first != this->numRun(_parser.fileName(_inputs.at(i)))) {
          if(i < _inputs.size() - 1) continue;
          else if(!isFindRun) {
            _log += " WARNING! RUN: " 
	    				 + std::to_string(run.first) + " was skipped! Can't find files!" 
	    				 + _rowDelimiter;
            continue;
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
            bool isRaw = raw(loops.at(l), event, analysis, dataRoot, &treesHR, &treesLR, run.first, 
                             &number_HR, &time_HR, &triger_HR, &ch_HR, &size_HR, &dtime_HR, &BCID_HR, &coarse_HR, &fine_HR, 
                             &number_LR, &time_LR, &triger_LR, &ch_LR, &size_LR, &dtime_LR, &BCID_LR, &coarse_LR, &fine_LR);
//            bool isOffset = offset(loops.at(l), mods, run.first, run.second, event, analysis, dataRoot, TGEs, TH1s, TH2s);
            bool isCore = core(loops.at(l), mods, run.first, event, analysis, dataRoot, TGEs, TH1s, TH2s);
//            bool isATime = ATime(loops.at(l), mods, run.first, event, analysis, dataRoot, TGEs, TH1s, TH2s);
//            bool isATimeSingleChamber = ATimeSingleChamber(loops.at(l), mods, run.first, event, analysis, dataRoot, TGEs, TH1s, TH2s);
//            bool isStripStudy = stripStudy(loops.at(l), mods, run.first, event, analysis, dataRoot, TGEs, TH1s, TH2s);
//            bool isClusterAlgoTest = ClusterAlgoTest(loops.at(l), mods, run.first, event, analysis, dataRoot, TGEs, MTGEs, TH1s, TH2s);
          // =====================================================================
          for(unsigned int ic = 0; ic < event->chambers()->size(); ic++)
            analysis->find(event->chambers()->at(ic))->second.clear(loops.at(l) + " EVENT");
  	      
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
	    // =======================================================================
      bool isRaw = raw(loops.at(l) + " END", event, analysis, dataRoot, &treesHR, &treesLR, run.first, 
                             &number_HR, &time_HR, &triger_HR, &ch_HR, &size_HR, &dtime_HR, &BCID_HR, &coarse_HR, &fine_HR, 
                             &number_LR, &time_LR, &triger_LR, &ch_LR, &size_LR, &dtime_LR, &BCID_LR, &coarse_LR, &fine_LR);
//      bool isOffset = offset(loops.at(l) + " END", mods, run.first, run.second, event, analysis, dataRoot, TGEs, TH1s, TH2s);
      bool isCore = core(loops.at(l) + " END", mods, run.first, event, analysis, dataRoot, TGEs, TH1s, TH2s);
//      bool isATime = ATime(loops.at(l) + " END", mods, run.first, event, analysis, dataRoot, TGEs, TH1s, TH2s);
//      bool isATimeSingleChamber = ATimeSingleChamber(loops.at(l) + " END", mods, run.first, event, analysis, dataRoot, TGEs, TH1s, TH2s);
//      bool isStripStudy = stripStudy(loops.at(l) + " END", mods, run.first, event, analysis, dataRoot, TGEs, TH1s, TH2s);
//      bool isClusterAlgoTest = ClusterAlgoTest(loops.at(l) + " END", mods, run.first, event, analysis, dataRoot, TGEs, MTGEs, TH1s, TH2s);
	    // =======================================================================
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
  std::map<std::string, DqMTGE*> MTGEs;
  for(unsigned int r = 0; r < RUNS.size(); r++) {
    std::cout << std::endl << "Run[" << r << "]: " << RUNS.at(r).first << std::endl;
    
    std::map<std::string, DqTH1D> TH1s;
    std::map<std::string, DqTH2D> TH2s;

    // =========================================================================
    bool isPetiroc = this->petiroc(RUNS.at(r), &mods, &event, &analysis, &dataRoot, &TGEs, &MTGEs, &TH1s, &TH2s);
    // =========================================================================
    
    // ANALYSIS ALL RUNS
    // =========================================================================
    
    // =========================================================================
    
    bool isSaveHists = this->saveHists(RUNS.at(r).first, &analysis, &dataRoot, &TH1s, &TH2s);
    TH1s.clear(); TH2s.clear();
  }
      
  std::cout << "End of Processing!" << std::endl;
  std::cout << "#-------------------------------------------------------------------------------\n" << std::endl;
	std::cout  << "Comparing cookies and Cleaning the kitchen..." << std::endl;

  
  bool isSaveGraphs = this->saveGraphs(&analysis, &dataRoot, &TGEs);
  TGEs.clear();
  bool isSaveMGraphs = this->saveMGraphs(&analysis, &dataRoot, &MTGEs);
  MTGEs.clear();
  
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
