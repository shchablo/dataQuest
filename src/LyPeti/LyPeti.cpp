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
/* analysis params and input files */
std::vector<int> LyPeti::parsNumRuns(std::vector<std::string> files) 
{
  DqParser parser;
  std::vector<int> result;
  std::string name;
  int number = -1;
  int prevNumber = -1;
  for(unsigned int i = 0; i < files.size(); i++) {
    name = parser.fileName(files.at(i));
    parser.deleteFormat(&name);
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
  name = parser.fileName(file);
  parser.deleteFormat(&name);
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
//------------------------------------------------------------------------------
bool LyPeti::offSet(int chamber, int offSetRun, 
                    std::map<int, double>* offsets, std::map<int, double>* offsetsHR, std::map<int, double>* offsetsLR,
                    std::map<std::string, DqTGE*>* TGEs, DqRoot* dataRoot)
{

  // GENERAL ANALYSIS PARAMETORS -----------------------------------------------
  std::string logRunNumber; 
  std::string plusOutDir; 
  std::string outName;
  std::string rootFilePath;
  LyPetiEvent event;
  std::vector<std::string> mods;
  std::map<int, LyPetiAnalysis> analysis;
  std::vector<int> runs;
  // ---------------------------------------------------------------------------

  // PARSING CARD PARAMETORS ---------------------------------------------------
  
  // system parametors
  std::vector<std::string> parserArrayDelimiters;
  parserArrayDelimiters.push_back(this->findParam("sys_DqParser_AD_1", &_params));
  parserArrayDelimiters.push_back(this->findParam("sys_DqParser_AD_2", &_params));
  parserArrayDelimiters.push_back(this->findParam("sys_DqParser_AD_3", &_params));
  parserArrayDelimiters.push_back(this->findParam("sys_DqParser_AD_4", &_params));
  parserArrayDelimiters.push_back(this->findParam("sys_DqParser_AD_5", &_params));
  DqParser parser; parser.setArrayDelimiter(parserArrayDelimiters);

  std::string strDelim = this->findParam("sys_DqParser_AD_3", &_params);
  
  logRunNumber = this->findParam("logNumber", &_params);  
  
  plusOutDir = this->findParam("dir", &_params);  
  outName = this->findParam("fN", &_params);
  std::string defaultPath = parser.filePath(_outputs.at(0));
  rootFilePath = defaultPath + plusOutDir + "/" + outName;

  std::string sMods = this->findParam("mods", &_params);
  mods = parser.parsArray(sMods);

	// Runs info
  std::string sRuns = this->findParam("runs", &_params);
  std::vector<std::string> vsRuns = parser.parsArray(sRuns);
  runs = parser.strToIntArray(vsRuns);
  sRuns.clear(); vsRuns.clear();
  
  std::string sStrips = this->findParam("strips", &_params);
  std::vector<std::string> vsStrips = parser.parsArray(sStrips);
  std::vector<int> strips = parser.strToIntArray(vsStrips);
  sStrips.clear(); vsStrips.clear();
  
  std::string sTrigCh = this->findParam("trigCh", &_params);
  std::vector<std::string> vsTrigCh = parser.parsArray(sTrigCh);
  std::vector<double> trigCh = parser.strToDoubleArray(vsTrigCh);
  sTrigCh.clear(); vsTrigCh.clear();
  
  std::string sHR = this->findParam("HR", &_params);
  std::vector<std::string> vsHR = parser.parsArray(sHR);
  std::vector<double> HR = parser.strToDoubleArray(vsHR);
  sHR.clear(); vsHR.clear();
  
  std::string sLR = this->findParam("LR", &_params);
  std::vector<std::string> vsLR = parser.parsArray(sLR);
  std::vector<double> LR = parser.strToDoubleArray(vsLR);
  sLR.clear(); vsLR.clear();
  
  std::string sTDCs = this->findParam("TDCs", &_params);
  std::vector<std::string> vsTDCs = parser.parsArray(sTDCs);
  std::vector<double> TDCs = parser.strToDoubleArray(vsTDCs);
  sTDCs.clear(); vsTDCs.clear();
  
  std::string sTimeOffSet = this->findParam("timeOffSet", &_params);
  std::vector<std::string> vsTimeOffSet = parser.parsArray(sTimeOffSet);
  std::vector<double> timeOffSet = parser.strToDoubleArray(vsTimeOffSet);
  sTimeOffSet.clear(); vsTimeOffSet.clear();
  
  std::string sTimeOffSetHR = this->findParam("timeOffSetHR", &_params);
  std::vector<std::string> vsTimeOffSetHR = parser.parsArray(sTimeOffSetHR);
  std::vector<double> timeOffSetHR = parser.strToDoubleArray(vsTimeOffSetHR);
  sTimeOffSetHR.clear(); vsTimeOffSetHR.clear();
 
  std::string sTimeOffSetLR = this->findParam("timeOffSetLR", &_params);
  std::vector<std::string> vsTimeOffSetLR = parser.parsArray(sTimeOffSetLR);
  std::vector<double> timeOffSetLR = parser.strToDoubleArray(vsTimeOffSetLR);
  sTimeOffSetLR.clear(); vsTimeOffSetLR.clear();
  
  std::string sInverse = this->findParam("inverse", &_params);
  std::vector<std::string> vsInverse = parser.parsArray(sInverse);
  std::vector<double> inverse = parser.strToDoubleArray(vsInverse);
  sInverse.clear(); vsInverse.clear();
  
  std::string sChambers = this->findParam("chambers", &_params);
  std::vector<std::string> vsChambers = parser.parsArray(sChambers);
  std::vector<double> chambers = parser.strToDoubleArray(vsChambers);
  sChambers.clear(); vsChambers.clear(); 
  
  // ---------------------------------------------------------------------------
	
  // ANALYSIS INPUT PARAMETORS -------------------------------------------------
	// fill maping 
  bool isMap = false;
  isMap = event.mapping(strips, trigCh, TDCs, 
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
  
  std::cout << "\n#---" << std::endl;
//  // chambers in processing  
//	std::cout  << "N: " << event.chambers()->size() << " chamber(s) in Processing:";
//	for(unsigned int i = 0; i < event.chambers()->size(); i++)
//		std::cout << " index=" << event.chambers()->at(i);
//	std::cout << std::endl;
	
  // COMAND LINE ---------------------------------------------------------------
  if(runs.size() > 0) {
    int processingFiles = 0;

    for(unsigned int r = 0; r < runs.size(); r++) {
	    for(unsigned int i = 0; i < _inputs.size(); i++) {
        if(runs.at(r) == this->numRun(parser.fileName(_inputs.at(i))))
          processingFiles += 1;
      }
    }
//    if(processingFiles > 0)
//      std::cout <<"N: " << processingFiles << " file(s) in Processing... / ";
//    else {
//      _log += " ERROR! Processing file(s): " 
//	  			 + std::to_string(processingFiles)  + "!" + _rowDelimiter;
//      return false;
//    }
  }
  else {  
    runs.clear();
  	runs = this->parsNumRuns(_inputs);
//    std::cout <<"N: " << _inputs.size() << " file(s) in Processing... / ";
  }
//  std::cout <<"N: " << runs.size() << " run(s) in Processing..." << std::endl;
  // ---------------------------------------------------------------------------  
  

	// ANALYSIS CLASS SETTING ----------------------------------------------------	
  auto itA = analysis.find(chamber);
  if(itA == analysis.end())
    analysis.insert(std::make_pair(chamber, LyPetiAnalysis{}));

  analysis.find(chamber)->second.setParser(&parser);
  
  // take values for runs 
  std::vector<double> values; 
  std::string sValues; 
  bool isValues = this->findParam((std::to_string(chamber) + strDelim + "values"), &sValues, &_params);
  if(isValues) {
    std::vector<std::string> vsValues = parser.parsArray(sValues);
    values = parser.strToDoubleArray(vsValues);
    sValues.clear(); vsValues.clear();
  }
  else {
    bool isValuesDef = this->findParam("values", &sValues, &_params);
    if(isValuesDef) {
  	  _log += " INFO: Chamber:  " + std::to_string(chamber) + " is using values !" + _rowDelimiter;
      std::vector<std::string> vsValues = parser.parsArray(sValues);
      values = parser.strToDoubleArray(vsValues);
      sValues.clear(); vsValues.clear();
    }
  }
	
	// take filters params
  std::map<std::string, double> algoParams; 
 	for(auto& itP: _params) {
		std::string chamberName = parser.chamberName(itP.first); // first param before delim
		bool isAlgo = parser.findParamFromStr("algo", itP.first);
		bool isFilter = parser.findParamFromStr("filter", itP.first);
		if(chamberName == std::to_string(chamber) && (isFilter || isAlgo)) {
      double value = 0;
			bool isValue = parser.strToDouble(&value, itP.second);
			if(isValue)
    	 	algoParams.insert(std::make_pair(parser.histName(itP.first), value));
		}
		if((isAlgo || isFilter) && (chamberName == "filter" || chamberName == "algo") ) {
  		_log += " INFO: Param for chamber: " + std::to_string(chamber)  + " is " + itP.first + "." + _rowDelimiter;
  		double value = 0;
			bool isValue = parser.strToDouble(&value, itP.second);
			if(isValue)
    	 	algoParams.insert(std::make_pair(parser.histName(itP.first), value));
		}
	}
  analysis.find(chamber)->second.setRuns(runs, values);
  analysis.find(chamber)->second.setParams(algoParams);
  values.clear(); algoParams.clear();
  bool isAnalysis = analysis.find(chamber)->second.configure();
	if(!isAnalysis) {
  	_log += " ERROR: Can't configure analysis for chamber: " + std::to_string(chamber) + "!" + _rowDelimiter;
		return false;
	}
  // ---------------------------------------------------------------------------

  strips.clear(); trigCh.clear(); TDCs.clear(); chambers.clear(); 
  HR.clear(); timeOffSetHR.clear(); LR.clear(); timeOffSetLR.clear(); timeOffSet.clear(); inverse.clear(); // clear memory (map params) 
  // ---------------------------------------------------------------------------

  
  LyPetiRead stream; 
  bool isOpen = false;
  bool isClose = false;
  
  int r = 0;
  bool isOffSetRun = false; 
	for(unsigned int ir = 0; ir < runs.size(); ir++) {
    if(runs.at(ir) == offSetRun) {
      r = ir;
      isOffSetRun = true;
    }
  }
  if(!isOffSetRun) {
    _log += " WARNING! Can not find time offset run: " + std::to_string(runs.at(r)) + "!" + _rowDelimiter;
    return false;
  }
  // runs params
  std::string nameTGEs = "";    
  auto itTGEs = TGEs->find("");

  std::cout << std::endl << "OffSet Run[" << r << "]: " << runs.at(r);
  
  // HIST FOR FILL DATA
  std::string nameTH1s = "";
  std::map<std::string, DqTH1D> TH1s;
  auto itTH1s = TH1s.find("");
  
  std::string nameTH2s = "";
  std::map<std::string, DqTH2D> TH2s;
  auto itTH2s = TH2s.find("");
  //std::map<std::string, double> histsParams;

	//  CONFIGURATION LOOP -----------------------------------------------------
  //  ------------------------------------------------------------------------
  std::cout << std::endl << " OffSet: Configurations loop..." << std::endl;
  // check inputs files and take rigth for currect run 
  bool isFindRun = false;
	for(unsigned int i = 0; i < _inputs.size(); i++) {
		
    if(runs.at(r) == this->numRun(parser.fileName(_inputs.at(i))))
      isFindRun = true;
    if(runs.at(r) != this->numRun(parser.fileName(_inputs.at(i)))) {
      if(i < _inputs.size() - 1)
        continue;
      else if(!isFindRun) {
        _log += " WARNING! RUN: " 
					 + std::to_string(runs.at(r)) + " was skipped! Can't find files!" 
					 + _rowDelimiter;
        continue;
      }
      else
        continue;
    }
    
    std::cout << "    File[" << i << "]: " 
													 << parser.fileName(_inputs.at(i)) << std::endl;

    isOpen = stream.open(_inputs.at(i));
    if (!isOpen) {
      _log += " WARNING! file: " 
					 + parser.fileName(_inputs.at(i))  + " was skipped! Can't open!" 
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
      event.clear(); event.fill(&buffers); 


      // clear memory form old buffers
      for (int j = 0; j < (int)buffers.size(); j++)
        delete buffers.at(j);
      buffers.clear();
    
      bool isEvent = event.check();
      if(!isEvent) {
        _log += " WARNING! Event in pos: " + std::to_string(prev) + " was skipped! Can't open!" 
			       + _rowDelimiter;
        continue;
      }
	    // ---------------------------------------------------------------------
      // FIRST ANALYSIS BEGIN ------------------------------------------------
        analysis.find(chamber)->second.setStrips(event.strips(chamber));
        unsigned int numberOfTrigers = event.trigs(chamber)->size();
        bool isTrigers = analysis.find(chamber)->second.fillTrigers(numberOfTrigers);
				if(isTrigers) {
          for(unsigned int js = 0; js < event.strips(chamber)->size(); js++) {
            // Fill raw data with maping. 
            for(unsigned int jh = 0; jh < event.strips(chamber)->at(js).HR.size(); jh++) {
              bool isFilterHR = analysis.find(chamber)->second.filters("data&HR", js, jh); 
              if(isFilterHR) 
                analysis.find(chamber)->second.fillEvents("data&HR", js, jh); 
            analysis.find(chamber)->second.filters("deadTime&HR", js, jh); // skip if deadTime 
            }
            for(unsigned int jl = 0; jl < event.strips(chamber)->at(js).LR.size(); jl++) {
              bool isFilterLR = analysis.find(chamber)->second.filters("data&LR", js, jl); 
              if(isFilterLR)
                analysis.find(chamber)->second.fillEvents("data&LR", js, jl); 
            analysis.find(chamber)->second.filters("deadTime&LR", js, jl); // skip if deadTime 
       	  	}
				  }
        }
        if(!analysis.find(chamber)->second.skipEvent()) {  
          // ANALYSIS PART
          std::string filters = analysis.find(chamber)->second.filtersStr(); 
          for(unsigned int im = 0; im < mods.size(); im++) {
            analysis.find(chamber)->second.algos(mods.at(im));
            std::string modParams = analysis.find(chamber)->second.modParamsStr(mods.at(im)); 
            std::vector<std::pair<double, double>> data = analysis.find(chamber)->second.data(mods.at(im).c_str());
            std::map<int, std::vector<double>> offSet;
            
            // offset
            for(unsigned int d = 0; d < data.size(); d++) {
              std::vector<std::pair<double, double>> dataAND = analysis.find(chamber)->second.data("AND");
              bool isOffSet = analysis.find(chamber)->second.offSetTime(data.at(d).first, &data, &dataAND, &offSet, mods.at(im).c_str());
              if(isOffSet) {
                nameTH2s = Form("%d%s%s%s_%d_%s%stimeProfileStrip", chamber, filters.c_str(), modParams.c_str(), strDelim.c_str(),  (int)data.at(d).first, strDelim.c_str(),  mods.at(im).c_str()); 
                for(auto& itOffSet: offSet) {
                  itTH2s = dataRoot->addDqTH2D(nameTH2s, &TH2s); itTH2s->second.config(nameTH2s, &_params);
                  for(unsigned int iOs = 0; iOs < itOffSet.second.size(); iOs++) {
                    itTH2s->second.setting(itOffSet.second.at(iOs), itOffSet.first);
                  //  std::cout << itOffSet.first << " " << itOffSet.second.at(iOs) << std::endl;
                  }
                }
              }
            }
            // ---
          }
        }
      //
        analysis.find(chamber)->second.clear("EVENT");
      // FIRST ANALYSIS END --------------------------------------------------
    	if(codeRead > 0)
				processBar.print(pos, 0.02);
			else
				processBar.print(size, 0.02);
      prev = pos;
	}
    std::cout << std::endl;
    if (isOpen) {
      isClose = stream.close();
      if (!isClose) {
        _log += " ERROR! file: " + parser.fileName(_inputs.at(i))  
																 + " Can't close!" + _rowDelimiter;
      	return false;
			}
    }
	}
  analysis.find(chamber)->second.clear("RUN");
  
  // END SETTING PARAMETORS
  for(auto& it: TH1s)
    it.second.setting();
  for(auto& it: TH2s)
    it.second.setting();
  // RUN PARAMETORS AND HISTOGRAM
	
  // SECOND FILE LOOP (Analysis) READ DATA ------------------------------------
  std::cout << "\n  OffSet: Analysis loop..." << std::endl;
  for(unsigned int i = 0; i < _inputs.size(); i++) {
		
    if(runs.at(r) == this->numRun(parser.fileName(_inputs.at(i))))
      isFindRun = true;
    if(runs.at(r) != this->numRun(parser.fileName(_inputs.at(i)))) {
      if(i < _inputs.size() - 1)
        continue;
      else if(!isFindRun) {
        _log += " WARNING! RUN: " 
					 + std::to_string(runs.at(r)) + " was skipped! Can't find files!" 
					 + _rowDelimiter;
        continue;
      }
      else
        continue;
    }

    std::cout << "    File[" << i << "]: " 
													 << parser.fileName(_inputs.at(i)) << std::endl;

    isOpen = stream.open(_inputs.at(i));
    if (!isOpen) {
      _log += " WARNING! file: " 
					 + parser.fileName(_inputs.at(i))  + " was skipped! Can't open!" 
					 + _rowDelimiter;
      continue;
    }

    // read raw data
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
      event.clear(); event.fill(&buffers); 

      // clear memory form old buffers
      for (int j = 0; j < (int)buffers.size(); j++)
        delete buffers.at(j);
      buffers.clear();
    
      bool isEvent = event.check();
      if(!isEvent) {
        _log += " WARNING! Event in pos: " + std::to_string(prev) + " was skipped! Can't open!" 
			       + _rowDelimiter;
        continue;
      }
	   // ----------------------------------------------------------------------

      // ANALYSIS BEGIN
      analysis.find(chamber)->second.setStrips(event.strips(chamber));

      unsigned int numberOfTrigers = event.trigs(chamber)->size();
      bool isTrigers = analysis.find(chamber)->second.fillTrigers(numberOfTrigers);
			if(isTrigers) {
        for(unsigned int js = 0; js < event.strips(chamber)->size(); js++) {
          for(unsigned int jh = 0; jh < event.strips(chamber)->at(js).HR.size(); jh++) {
            bool isFilterHR = analysis.find(chamber)->second.filters("data&HR", js, jh); 
            if(isFilterHR)
              analysis.find(chamber)->second.fillEvents("data&HR", js, jh); 
            analysis.find(chamber)->second.filters("deadTime&HR", js, jh); // skip if deadTime 
          }
          // LR
      	  for(unsigned int jl = 0; jl < event.strips(chamber)->at(js).LR.size(); jl++) {
            bool isFilterLR = analysis.find(chamber)->second.filters("data&LR", js, jl); 
            if(isFilterLR)
              analysis.find(chamber)->second.fillEvents("data&LR", js, jl); 
            analysis.find(chamber)->second.filters("deadTime&LR", js, jl); // skip if deadTime 
      	  }
        }
			}
      if(!analysis.find(chamber)->second.skipEvent()) {  
        // ANALYSIS PART
        std::string filters = analysis.find(chamber)->second.filtersStr(); 
        for(unsigned int im = 0; im < mods.size(); im++) {
          analysis.find(chamber)->second.algos(mods.at(im));
          std::string modParams = analysis.find(chamber)->second.modParamsStr(mods.at(im)); 
          std::vector<std::pair<double, double>> data = analysis.find(chamber)->second.data(mods.at(im).c_str());
          
          // offset
          for(unsigned int d = 0; d < data.size(); d++) {
            std::map<int, std::vector<double>> offSet;
            std::vector<std::pair<double, double>> dataAND = analysis.find(chamber)->second.data("AND");
            bool isOffSet = analysis.find(chamber)->second.offSetTime(data.at(d).first, &data, &dataAND, &offSet, mods.at(im).c_str());
            if(isOffSet) {
              nameTH2s = Form("%d%s%s%s_%d_%s%stimeProfileStrip", chamber, filters.c_str(), modParams.c_str(), strDelim.c_str(),  (int)data.at(d).first, strDelim.c_str(),  mods.at(im).c_str()); 
              for(auto& itOffSet: offSet) {
                for(unsigned int iOs = 0; iOs < itOffSet.second.size(); iOs++) {
                  TH2s.find(nameTH2s.c_str())->second.Fill(itOffSet.second.at(iOs), itOffSet.first);
                  //std::cout << itOffSet.first << " " << itOffSet.second.at(iOs) << std::endl; 
                }
              }
            }
          }
        }
      }
          // ----
      // PART for analysis TRACING AND ETC STUDY CROSS chambers
      //--------------
      analysis.find(chamber)->second.clear("EVENT");
      // ANALYSIS END.
    	if(codeRead > 0)
				processBar.print(pos, 0.02);
			else
				processBar.print(size, 0.02);
      prev = pos;
	}
  //------
    std::cout << std::endl;
    if (isOpen) {
      isClose = stream.close();
      if (!isClose) {
        _log += " ERROR! file: " + parser.fileName(_inputs.at(i))  
																 + " Can't close!" + _rowDelimiter;
      	return false;
			}
    }
	}
  std::string filters = analysis.find(chamber)->second.filtersStr(); 
  for(unsigned int im = 0; im < mods.size(); im++) {
    std::string modParams = analysis.find(chamber)->second.modParamsStr(mods.at(im)); 
    std::string name = parser.last(mods.at(im));
    std::string param = parser.first(mods.at(im));
    if((name == "HR" || name == "LR")) {
      
      int strips = event.numberOfStrips(chamber);
      int thrEntries = 30;
      double refStrip = 0;
      //nameTH1s = Form("%d%sTimeWalk%s", chamber, strDelim.c_str(), mods.at(im).c_str());
      nameTH1s = Form("%d%sTimeWalk", chamber, strDelim.c_str());
      itTH1s = dataRoot->addDqTH1D(nameTH1s, &TH1s); itTH1s->second.config(nameTH1s, &_params);
      for(int s = 0; s < strips; s++) {
		    double value = analysis.find(chamber)->second.getValue(runs.at(r));
        if(s%2==0) {
            if(chamber == 1000)
              refStrip = 28;
            if(chamber == 0)
              refStrip = 20;
        }
        else {
            if(chamber == 1000)
              refStrip = 27;
            if(chamber == 0)
              refStrip = 19;
        }

        
        nameTH2s = Form("%d%s%s%s_%d_%s%stimeProfileStrip", chamber, filters.c_str(), modParams.c_str(), strDelim.c_str(),  s+chamber, strDelim.c_str(),  mods.at(im).c_str()); 
        auto itOffSetMean = TH2s.find(nameTH2s);
        if(!(itOffSetMean == TH2s.end())) {
          if(s < strips - 2 && s > 1 &&
              itOffSetMean->second.ProjectionX("hX", s-1, s-1, "")->GetEntries() >= thrEntries &&
             itOffSetMean->second.ProjectionX("hXX", s+1, s+1, "")->GetEntries() >= thrEntries ) {
            TH1D * s1 = itOffSetMean->second.ProjectionX("h1", s-1, s-1, "");
            double pMean  = s1->GetMean();
            double pRMS  = s1->GetRMS();
            TF1* sg1 = new TF1("sg", "gaus",  pMean - 3*pRMS, pMean + 3*pRMS);
            s1->Fit("sg", "S");
            TH1D * s2 = itOffSetMean->second.ProjectionX("h2", s+1, s+1, "");
            pMean  = s2->GetMean();
            pRMS  = s2->GetRMS();
            TF1* sg2 = new TF1("sg2", "gaus",  pMean - 3*pRMS, pMean + 3*pRMS);
            s2->Fit("sg2", "S");
            //nameTH1s = Form("%d%sTimeWalk%s", chamber, strDelim.c_str(), mods.at(im).c_str());
            nameTH1s = Form("%d%sTimeWalk", chamber, strDelim.c_str());
            TH1s.find(nameTH1s.c_str())->second.Fill(sg1->GetParameter(1));
            TH1s.find(nameTH1s.c_str())->second.Fill(sg2->GetParameter(1));
            delete sg2; delete sg1; delete s1; delete s2;
          }
        }
        
        double offSet = 0;
        double eOffSet = 0;
        for(int is = s+1; is < refStrip; is=is+2) {
          nameTH2s = Form("%d%s%s%s_%d_%s%stimeProfileStrip", chamber, filters.c_str(), modParams.c_str(), strDelim.c_str(),  is+chamber, strDelim.c_str(),  mods.at(im).c_str()); 
          auto itOffSet = TH2s.find(nameTH2s);
          if(!(itOffSet == TH2s.end())) {
            if(itOffSet->second.ProjectionX("h", is, is, "")->GetEntries() <= thrEntries) {
               offSet = 0; break;
            }
            else {
            TH1D * projh2X = itOffSet->second.ProjectionX("h", is, is, "");
            //double mean = 0;
            //int content = 0;
            //int nbinsx = projh2X->GetXaxis()->GetNbins();
            //for(int bin = 0; bin < nbinsx; bin++) {
            //  projh2X->GetBinContent(bin);
            // if(projh2X->GetBinContent(bin) > content) {
            //    content = projh2X->GetBinContent(bin);
            //    mean = projh2X->GetBinCenter(bin);
            //  }
            //}
            double pMean  = projh2X->GetMean();
            double pRMS  = projh2X->GetRMS();
            TF1* g1 = new TF1("g", "gaus",  pMean - 6*pRMS, pMean + 6*pRMS);
            projh2X->Fit("g", "S");
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
          nameTH2s = Form("%d%s%s%s_%d_%s%stimeProfileStrip", chamber, filters.c_str(), modParams.c_str(), strDelim.c_str(),  is+chamber, strDelim.c_str(),  mods.at(im).c_str()); 
          auto itOffSet = TH2s.find(nameTH2s);
          if(!(itOffSet == TH2s.end())) {
            if(itOffSet->second.ProjectionX("h", is, is, "")->GetEntries() <= thrEntries) {
               offSet = 0; break;
            }
            else {
            TH1D * projh2X = itOffSet->second.ProjectionX("h", is, is, "");
            //double mean = 0;
            //int content = 0;
            //int nbinsx = projh2X->GetXaxis()->GetNbins();
            //for(int bin = 0; bin < nbinsx; bin++) {
            //  projh2X->GetBinContent(bin);
            // if(projh2X->GetBinContent(bin) > content) {
            //    content = projh2X->GetBinContent(bin);
            //    mean = projh2X->GetBinCenter(bin);
            //  }
            //}
            double pMean  = projh2X->GetMean();
            double pRMS  = projh2X->GetRMS();
            TF1* g1 = new TF1("g", "gaus",  pMean - 6*pRMS, pMean + 6*pRMS);
            projh2X->Fit("g", "S");
            //offSet = offSet - projh2X->GetMean();
            offSet = offSet + g1->GetParameter(1);
            delete projh2X;
            delete g1;
            }
          }
          else
            break;
        }
        //std::cout << nameTH2s << " " << mean << std::endl;
        //int nbinsx = itOffSet->second.GetXaxis()->GetNbins();
        //double mean = 0;
        //double content = 0;
        //for(int bin = 0; bin < nbinsx; bin++) {
        //  itOffSet->second.GetBinContent(bin);
        //  if(itOffSet->second.GetBinContent(bin) > content) {
        //    content = itOffSet->second.GetBinContent(bin);
        //    mean = itOffSet->second.GetBinCenter(bin);
        //  }
        //}
        //TF1* g1 = new TF1("g", "gaus", mean-2, mean+2);
        
        // itOffSet->second.Fit("g", "S");
        
        // g1->Draw("same");
        //offSet = g1->GetParameter(1) - refTime;
        //offSet = mean - refTime;
        //eOffSet = g1->GetParameter(2)/std::sqrt(itOffSet->second.GetEntries());
        //delete g1;
        

        if(name == "HR")
          offsetsHR->insert(std::make_pair(s+chamber, offSet));
        if(name == "LR")
          offsetsLR->insert(std::make_pair(s+chamber, offSet));
        if(name == "AND")
          offsets->insert(std::make_pair(s+chamber, offSet));
		    nameTGEs = Form("%d%s%s_%d_%soffSet", chamber, strDelim.c_str(), mods.at(im).c_str(), (int)value, strDelim.c_str());   
        itTGEs = dataRoot->addDqTGE(nameTGEs, TGEs); itTGEs->second->config(nameTGEs, &_params);
        TGEs->find(nameTGEs.c_str())->second->setting(s + chamber, 0, offSet, eOffSet);
      }
      //}
    }
  }
  analysis.find(chamber)->second.clear("RUN");
  
  for(auto profiles: TH1s) {
    int chamberNum = -1;
    double value = runs.at(r);
    std::string chamber = parser.chamberName(profiles.first);
    bool isChamber = parser.strToInt(&chamberNum, chamber); 
    if(isChamber) { 
      auto it = analysis.find(chamberNum);
      if(!(it == analysis.end()))
        value = analysis.find(chamberNum)->second.getValue(runs.at(r));
    }

    std::string histName = parser.histName(profiles.first);
    std::string comment = this->findParam("comment", &_params);
    std::string path = Form("%s_%s/param_%.2f%srun_%d/chamber_%d", comment.c_str(), logRunNumber.c_str(), value, strDelim.c_str(), runs.at(r), chamberNum); 
    std::string folder = "";
    bool isFolder = this->findParam((histName + "_path"), &folder, &_params);
    if(isFolder)
      path += "/" + folder;
    else 
      path += "/withoutPath";
    dataRoot->writeTH1toCanvasCMS(path, &profiles.second, false);
  }
  TH1s.clear();
  
  for(auto profiles: TH2s) {
    int chamberNum = -1;
    double value = runs.at(r);
    std::string chamber = parser.chamberName(profiles.first);
    bool isChamber = parser.strToInt(&chamberNum, chamber); 
    if(isChamber) { 
      auto it = analysis.find(chamberNum);
      if(!(it == analysis.end()))
        value = analysis.find(chamberNum)->second.getValue(runs.at(r));
    }
    std::string histName = parser.histName(profiles.first);
    std::string comment = this->findParam("comment", &_params);
    std::string path = Form("%s_%s/param_%.2f%srun_%d/chamber_%d", comment.c_str(), logRunNumber.c_str(), value, strDelim.c_str(), runs.at(r), chamberNum); 
    std::string folder = "";
    bool isFolder = this->findParam((histName + "_path"), &folder, &_params);
    if(isFolder)
      path += "/" + folder;
    else 
      path += "/withoutPath";
    dataRoot->writeTH2toCanvasCMS(path, &profiles.second, false);
  }
  TH2s.clear();
  return true;
}

bool LyPeti::analysis() 
{
  std::cout << "#---\n" << std::endl;
	std::cout  << "Getting ingredients for cookies..." << std::endl;
  // GENERAL ANALYSIS PARAMETORS -----------------------------------------------
  std::string logRunNumber; 
  std::string plusOutDir; 
  std::string outName;
  std::string rootFilePath;
  LyPetiEvent event;
  std::vector<std::string> mods;
  std::map<int, LyPetiAnalysis> analysis;
  std::vector<int> runs;
  // ---------------------------------------------------------------------------
  // PARSING CARD PARAMETORS ---------------------------------------------------
  
  // system parametors
  std::vector<std::string> parserArrayDelimiters;
  parserArrayDelimiters.push_back(this->findParam("sys_DqParser_AD_1", &_params));
  parserArrayDelimiters.push_back(this->findParam("sys_DqParser_AD_2", &_params));
  parserArrayDelimiters.push_back(this->findParam("sys_DqParser_AD_3", &_params));
  parserArrayDelimiters.push_back(this->findParam("sys_DqParser_AD_4", &_params));
  parserArrayDelimiters.push_back(this->findParam("sys_DqParser_AD_5", &_params));
  DqParser parser; parser.setArrayDelimiter(parserArrayDelimiters);

  std::string strDelim = this->findParam("sys_DqParser_AD_3", &_params);
 
  logRunNumber = this->findParam("logNumber", &_params);  
  
  plusOutDir = this->findParam("dir", &_params);  
  outName = this->findParam("fN", &_params);
  std::string defaultPath = parser.filePath(_outputs.at(0));
  rootFilePath = defaultPath + plusOutDir + "/" + outName;

  std::string sMods = this->findParam("mods", &_params);
  mods = parser.parsArray(sMods);

	// Runs info
  std::string sRuns = this->findParam("runs", &_params);
  std::vector<std::string> vsRuns = parser.parsArray(sRuns);
  runs = parser.strToIntArray(vsRuns);
  sRuns.clear(); vsRuns.clear();
  
  std::string sStrips = this->findParam("strips", &_params);
  std::vector<std::string> vsStrips = parser.parsArray(sStrips);
  std::vector<int> strips = parser.strToIntArray(vsStrips);
  sStrips.clear(); vsStrips.clear();
  
  std::string sTrigCh = this->findParam("trigCh", &_params);
  std::vector<std::string> vsTrigCh = parser.parsArray(sTrigCh);
  std::vector<double> trigCh = parser.strToDoubleArray(vsTrigCh);
  sTrigCh.clear(); vsTrigCh.clear();
  
  std::string sHR = this->findParam("HR", &_params);
  std::vector<std::string> vsHR = parser.parsArray(sHR);
  std::vector<double> HR = parser.strToDoubleArray(vsHR);
  sHR.clear(); vsHR.clear();
  
  std::string sLR = this->findParam("LR", &_params);
  std::vector<std::string> vsLR = parser.parsArray(sLR);
  std::vector<double> LR = parser.strToDoubleArray(vsLR);
  sLR.clear(); vsLR.clear();
  
  std::string sTDCs = this->findParam("TDCs", &_params);
  std::vector<std::string> vsTDCs = parser.parsArray(sTDCs);
  std::vector<double> TDCs = parser.strToDoubleArray(vsTDCs);
  sTDCs.clear(); vsTDCs.clear();
  
  std::string sTimeOffSet = this->findParam("timeOffSet", &_params);
  std::vector<std::string> vsTimeOffSet = parser.parsArray(sTimeOffSet);
  std::vector<double> timeOffSet = parser.strToDoubleArray(vsTimeOffSet);
  sTimeOffSet.clear(); vsTimeOffSet.clear();
  
  std::string sTimeOffSetHR = this->findParam("timeOffSetHR", &_params);
  std::vector<std::string> vsTimeOffSetHR = parser.parsArray(sTimeOffSetHR);
  std::vector<double> timeOffSetHR = parser.strToDoubleArray(vsTimeOffSetHR);
  sTimeOffSetHR.clear(); vsTimeOffSetHR.clear();
 
  std::string sTimeOffSetLR = this->findParam("timeOffSetLR", &_params);
  std::vector<std::string> vsTimeOffSetLR = parser.parsArray(sTimeOffSetLR);
  std::vector<double> timeOffSetLR = parser.strToDoubleArray(vsTimeOffSetLR);
  sTimeOffSetLR.clear(); vsTimeOffSetLR.clear();
  
  std::string sInverse = this->findParam("inverse", &_params);
  std::vector<std::string> vsInverse = parser.parsArray(sInverse);
  std::vector<double> inverse = parser.strToDoubleArray(vsInverse);
  sInverse.clear(); vsInverse.clear();
  
  std::string sChambers = this->findParam("chambers", &_params);
  std::vector<std::string> vsChambers = parser.parsArray(sChambers);
  std::vector<double> chambers = parser.strToDoubleArray(vsChambers);
  sChambers.clear(); vsChambers.clear(); 
  
  // ---------------------------------------------------------------------------
	
  // ANALYSIS INPUT PARAMETORS -------------------------------------------------
	// fill maping 
  bool isMap = false;
  isMap = event.mapping(strips, trigCh, TDCs, 
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
  
  std::cout << "\n#---" << std::endl;
  // chambers in processing  
	std::cout  << "N: " << event.chambers()->size() << " chamber(s) in Processing:";
	for(unsigned int i = 0; i < event.chambers()->size(); i++)
		std::cout << " index=" << event.chambers()->at(i);
	std::cout << std::endl;
	
  // COMAND LINE ---------------------------------------------------------------
  if(runs.size() > 0) {
    int processingFiles = 0;

    for(unsigned int r = 0; r < runs.size(); r++) {
	    for(unsigned int i = 0; i < _inputs.size(); i++) {
        if(runs.at(r) == this->numRun(parser.fileName(_inputs.at(i))))
          processingFiles += 1;
      }
    }
    if(processingFiles > 0)
      std::cout <<"N: " << processingFiles << " file(s) in Processing... / ";
    else {
      _log += " ERROR! Processing file(s): " 
	  			 + std::to_string(processingFiles)  + "!" + _rowDelimiter;
      return false;
    }
  }
  else {  
    runs.clear();
  	runs = this->parsNumRuns(_inputs);
    std::cout <<"N: " << _inputs.size() << " file(s) in Processing... / ";
  }
  std::cout <<"N: " << runs.size() << " run(s) in Processing..." << std::endl;
  // ---------------------------------------------------------------------------  
  
	DqRoot dataRoot; dataRoot.setOutputFile(rootFilePath, "default");
  LyPetiRead stream; 
  bool isOpen = false;
  bool isClose = false;
  
  // runs params
  std::string nameTGEs = "";    
  std::map<std::string, DqTGE*> TGEs;
  auto itTGEs = TGEs.find("");
  std::cout << "#---" << std::endl;
	std::cout  << "\nStill Cooking the cookies..." << std::endl;
	
	// ANALYSIS CLASS SETTING ----------------------------------------------------	
  for(unsigned int i = 0; i < event.chambers()->size(); i++) {
    int chamber = event.chambers()->at(i); 
    auto itA = analysis.find(chamber);
    if(itA == analysis.end())
      analysis.insert(std::make_pair(chamber, LyPetiAnalysis{}));

    analysis.find(chamber)->second.setParser(&parser);
    
    // take values for runs 
    std::vector<double> values; 
    std::string sValues; 
    bool isValues = this->findParam((std::to_string(chamber) + strDelim + "values"), &sValues, &_params);
    if(isValues) {
      std::vector<std::string> vsValues = parser.parsArray(sValues);
      values = parser.strToDoubleArray(vsValues);
      sValues.clear(); vsValues.clear();
    }
    else {
      bool isValuesDef = this->findParam("values", &sValues, &_params);
      if(isValuesDef) {
    	  _log += " INFO: Chamber:  " + std::to_string(chamber) + " is using values !" + _rowDelimiter;
        std::vector<std::string> vsValues = parser.parsArray(sValues);
        values = parser.strToDoubleArray(vsValues);
        sValues.clear(); vsValues.clear();
      }
    }
	 	
		// take filters params
    std::map<std::string, double> algoParams; 
 		for(auto& itP: _params) {
			std::string chamberName = parser.chamberName(itP.first); // first param before delim
			bool isAlgo = parser.findParamFromStr("algo", itP.first);
			bool isFilter = parser.findParamFromStr("filter", itP.first);
			if(chamberName == std::to_string(chamber) && (isFilter || isAlgo)) {
        double value = 0;
				bool isValue = parser.strToDouble(&value, itP.second);
				if(isValue)
      	 	algoParams.insert(std::make_pair(parser.histName(itP.first), value));
			}
			if((isAlgo || isFilter) && (chamberName == "filter" || chamberName == "algo") ) {
    		_log += " INFO: Param for chamber: " + std::to_string(chamber)  + " is " + itP.first + "." + _rowDelimiter;
  			double value = 0;
				bool isValue = parser.strToDouble(&value, itP.second);
				if(isValue)
      	 	algoParams.insert(std::make_pair(parser.histName(itP.first), value));
			}
		}
    analysis.find(chamber)->second.setRuns(runs, values);
    analysis.find(chamber)->second.setParams(algoParams);
    values.clear(); algoParams.clear();
    bool isAnalysis = analysis.find(chamber)->second.configure();
    
    std::string sOffsetRun;
    int offsetRun = 0;
    bool isOffsetRun = this->findParam((std::to_string(chamber) + strDelim + "offsetRun"), &sOffsetRun, &_params);
    if(isOffsetRun)
      offsetRun = std::atoi(sOffsetRun.c_str());
    else {
      bool isOffsetRunDef = this->findParam("offsetRun", &sOffsetRun, &_params);
      offsetRun = std::atoi(sOffsetRun.c_str());
    std::map<int, double> offsets; std::map<int, double> offsetsHR; std::map<int, double> offsetsLR;
     this->offSet(chamber, offsetRun, &offsets, &offsetsHR, &offsetsLR, &TGEs, &dataRoot);
     event.setTimeOffSetHR(&offsetsHR); 
     event.setTimeOffSetLR(&offsetsLR);
     event.setTimeOffSet(&offsets);
    }
    if(!isAnalysis) {
    	_log += " ERROR: Can't configure analysis for chamber: " + std::to_string(chamber) + "!" + _rowDelimiter;
			return false;
		}
  }
  // ---------------------------------------------------------------------------

  strips.clear(); trigCh.clear(); TDCs.clear(); chambers.clear(); 
  HR.clear(); timeOffSetHR.clear(); LR.clear(); timeOffSetLR.clear(); timeOffSet.clear(); inverse.clear(); // clear memory (map params) 
  // ---------------------------------------------------------------------------
  std::cout  << "\nCookies are ready, invite you to enjoy the taste!" << std::endl;
  std::cout << "#-------------------------------------------------------------------------------" << std::endl;

  for(unsigned int r = 0; r < runs.size(); r++) {
    std::cout << std::endl << "Run[" << r << "]: " << runs.at(r);
    
    // HIST FOR FILL DATA
    std::string nameTH1s = "";
    std::map<std::string, DqTH1D> TH1s;
    auto itTH1s = TH1s.find("");
    
    std::string nameTH2s = "";
    std::map<std::string, DqTH2D> TH2s;
    auto itTH2s = TH2s.find("");
    //std::map<std::string, double> histsParams;

	  //  CONFIGURATION LOOP -----------------------------------------------------
    //  ------------------------------------------------------------------------
    std::cout << std::endl << "  Configurations loop..." << std::endl;
    // check inputs files and take rigth for currect run 
    bool isFindRun = false;
		for(unsigned int i = 0; i < _inputs.size(); i++) {
			
      if(runs.at(r) == this->numRun(parser.fileName(_inputs.at(i))))
        isFindRun = true;
      if(runs.at(r) != this->numRun(parser.fileName(_inputs.at(i)))) {
        if(i < _inputs.size() - 1)
          continue;
        else if(!isFindRun) {
          _log += " WARNING! RUN: " 
	  				 + std::to_string(runs.at(r)) + " was skipped! Can't find files!" 
	  				 + _rowDelimiter;
          continue;
        }
        else
          continue;
      }
  	  
      std::cout << "    File[" << i << "]: " 
														 << parser.fileName(_inputs.at(i)) << std::endl;

  	  isOpen = stream.open(_inputs.at(i));
  	  if (!isOpen) {
  	    _log += " WARNING! file: " 
						 + parser.fileName(_inputs.at(i))  + " was skipped! Can't open!" 
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
  	    event.clear(); event.fill(&buffers); 


  	    // clear memory form old buffers
  	    for (int j = 0; j < (int)buffers.size(); j++)
  	      delete buffers.at(j);
  	    buffers.clear();
  	  
  	    bool isEvent = event.check();
  	    if(!isEvent) {
  	      _log += " WARNING! Event in pos: " + std::to_string(prev) + " was skipped! Can't open!" 
				       + _rowDelimiter;
  	      continue;
  	    }
	      // ---------------------------------------------------------------------
  	    // FIRST ANALYSIS BEGIN ------------------------------------------------
        for(unsigned int ic = 0; ic < event.chambers()->size(); ic++) {
          int chamber = event.chambers()->at(ic);
          analysis.find(chamber)->second.setStrips(event.strips(chamber));

          // TRIGS
          for(unsigned int jt = 0; jt < event.trigs(chamber)->size(); jt++) {
            int boardID = event.trigs(chamber)->at(jt)->head->boardID; 
            nameTH1s = Form("%d%snum%d%sbuffer", chamber, strDelim.c_str(), boardID, strDelim.c_str());
            itTH1s = dataRoot.addDqTH1D(nameTH1s, &TH1s); itTH1s->second.config(nameTH1s, &_params);
            itTH1s->second.setting(event.trigs(chamber)->at(jt)->head->numOfCh);
            //---
            nameTH1s = Form("%d%snum%d%strigBCID", chamber, strDelim.c_str(), boardID,strDelim.c_str());
            itTH1s = dataRoot.addDqTH1D(nameTH1s, &TH1s); itTH1s->second.config(nameTH1s, &_params);
            itTH1s->second.setting(event.trigs(chamber)->at(jt)->BCID);
          }

          unsigned int numberOfTrigers = event.trigs(chamber)->size();
          nameTH1s = Form("%d%sNumberOfTrigers", chamber, strDelim.c_str()); 
          itTH1s = dataRoot.addDqTH1D(nameTH1s, &TH1s); itTH1s->second.config(nameTH1s, &_params);
          itTH1s->second.setting(numberOfTrigers);
          //--- 
				  
          bool isTrigers = analysis.find(chamber)->second.fillTrigers(numberOfTrigers);
					if(isTrigers) {
            for(unsigned int js = 0; js < event.strips(chamber)->size(); js++) {
             
              // HR
              double numberOfHitsHR = event.strips(chamber)->at(js).HR.size(); 
              nameTH1s = Form("%d%sNumberOfHitsHR", chamber, strDelim.c_str()); 
              itTH1s = dataRoot.addDqTH1D(nameTH1s, &TH1s); itTH1s->second.config(nameTH1s, &_params);
              itTH1s->second.setting(numberOfHitsHR);
              // LR
            	double numberOfHitsLR = event.strips(chamber)->at(js).LR.size(); 
              nameTH1s = Form("%d%sNumberOfHitsLR", chamber, strDelim.c_str()); 
              itTH1s = dataRoot.addDqTH1D(nameTH1s, &TH1s); itTH1s->second.config(nameTH1s, &_params);
              itTH1s->second.setting(numberOfHitsLR);
              //--- 
             
              // Fill raw data with maping. 
              for(unsigned int jh = 0; jh < event.strips(chamber)->at(js).HR.size(); jh++) {
                bool isFilterHR = analysis.find(chamber)->second.filters("data&HR", js, jh); 
                if(isFilterHR) 
                  analysis.find(chamber)->second.fillEvents("data&HR", js, jh); 
              bool isFilterNoiseHR = analysis.find(chamber)->second.filters("noise&HR", js, jh); 
              if(isFilterNoiseHR) 
                analysis.find(chamber)->second.fillEvents("noise&HR", js, jh); 
              analysis.find(chamber)->second.filters("deadTime&HR", js, jh); // skip if deadTime 
              }
              
              for(unsigned int jl = 0; jl < event.strips(chamber)->at(js).LR.size(); jl++) {
                bool isFilterLR = analysis.find(chamber)->second.filters("data&LR", js, jl); 
                if(isFilterLR)
                  analysis.find(chamber)->second.fillEvents("data&LR", js, jl); 
                bool isFilterNoiseLR = analysis.find(chamber)->second.filters("noise&LR", js, jl); 
                if(isFilterNoiseLR) 
                  analysis.find(chamber)->second.fillEvents("noise&LR", js, jl); 
              analysis.find(chamber)->second.filters("deadTime&LR", js, jl); // skip if deadTime 
         	  	}
					  }
          }
          if(!analysis.find(chamber)->second.skipEvent()) {  
            // ANALYSIS PART
            std::string filters = analysis.find(chamber)->second.filtersStr(); 
            for(unsigned int im = 0; im < mods.size(); im++) {
              analysis.find(chamber)->second.algos(mods.at(im));
              std::string modParams = analysis.find(chamber)->second.modParamsStr(mods.at(im)); 
              std::vector<std::pair<double, double>> data = analysis.find(chamber)->second.data(mods.at(im).c_str());
              for(unsigned int jd = 0; jd < data.size(); jd++) { 
                nameTH1s = Form("%d%s%s%s%sstripProfile", chamber, filters.c_str(), modParams.c_str(), strDelim.c_str(), mods.at(im).c_str()); 
                itTH1s = dataRoot.addDqTH1D(nameTH1s, &TH1s); itTH1s->second.config(nameTH1s, &_params);
                itTH1s->second.setting(data.at(jd).first);
                //---
                nameTH1s = Form("%d%s%s%s%stimeProfile", chamber, filters.c_str(), modParams.c_str(), strDelim.c_str(), mods.at(im).c_str()); 
                itTH1s = dataRoot.addDqTH1D(nameTH1s, &TH1s); itTH1s->second.config(nameTH1s, &_params);
                itTH1s->second.setting(data.at(jd).second);
              
                nameTH2s = Form("%d%s%s%s%sstripTimeProfile", chamber, filters.c_str(), modParams.c_str(), strDelim.c_str(), mods.at(im).c_str()); 
                itTH2s = dataRoot.addDqTH2D(nameTH2s, &TH2s); itTH2s->second.config(nameTH2s, &_params);
                itTH2s->second.setting(data.at(jd).second, data.at(jd).first);
              }
              
              double minTime = 0; bool isMinTime = analysis.find(chamber)->second.minTime(&minTime, mods.at(im).c_str());
              if(isMinTime) {
                nameTH1s = Form("%d%s%s%s%sminTime", chamber, filters.c_str(), modParams.c_str(), strDelim.c_str(), mods.at(im).c_str()); 
                itTH1s = dataRoot.addDqTH1D(nameTH1s, &TH1s); itTH1s->second.config(nameTH1s, &_params);
                itTH1s->second.setting(minTime);
              }
              double maxTime = 0; bool isMaxTime = analysis.find(chamber)->second.maxTime(&maxTime, mods.at(im).c_str());
              if(isMaxTime) {
                nameTH1s = Form("%d%s%s%s%smaxTime", chamber, filters.c_str(), modParams.c_str(), strDelim.c_str(), mods.at(im).c_str()); 
                itTH1s = dataRoot.addDqTH1D(nameTH1s, &TH1s); itTH1s->second.config(nameTH1s, &_params);
                itTH1s->second.setting(maxTime);
              }
              int mult = 0; bool isMult = analysis.find(chamber)->second.multiplicity(&mult, mods.at(im).c_str());
              if(isMult) {
                nameTH1s = Form("%d%s%s%s%smultiplicity", chamber, filters.c_str(), modParams.c_str(), strDelim.c_str(), mods.at(im).c_str()); 
                itTH1s = dataRoot.addDqTH1D(nameTH1s, &TH1s); itTH1s->second.config(nameTH1s, &_params);
                itTH1s->second.setting(mult);
              }
              std::vector<int> clusterSize; bool isClusterSize = analysis.find(chamber)->second.clusterSize(&clusterSize, mods.at(im).c_str());
              if(isClusterSize) {
                nameTH1s = Form("%d%s%s%s%sclusterSize", chamber, filters.c_str(), modParams.c_str(), strDelim.c_str(), mods.at(im).c_str()); 
                itTH1s = dataRoot.addDqTH1D(nameTH1s, &TH1s); itTH1s->second.config(nameTH1s, &_params);
                for( unsigned int i = 0; i < clusterSize.size(); i++)
                  itTH1s->second.setting(clusterSize.at(i));
                clusterSize.clear();
              }
              // --
            }
          }
        }
        //
        //
        for(unsigned int ic = 0; ic < event.chambers()->size(); ic++)
          analysis.find(event.chambers()->at(ic))->second.clear("CONFIG EVENT");
  	    // FIRST ANALYSIS END --------------------------------------------------
  	  	if(codeRead > 0)
					processBar.print(pos, 0.02);
				else
					processBar.print(size, 0.02);
  	    prev = pos;
		}
  	  std::cout << std::endl;
  	  if (isOpen) {
  	    isClose = stream.close();
  	    if (!isClose) {
  	      _log += " ERROR! file: " + parser.fileName(_inputs.at(i))  
																	 + " Can't close!" + _rowDelimiter;
  	    	return false;
				}
  	  }
	  }
    for(unsigned int ic = 0; ic < event.chambers()->size(); ic++) {
      for(unsigned int im = 0; im < mods.size(); im++) {
      }
      analysis.find(event.chambers()->at(ic))->second.clear("CONFIG RUN");
    }
    
    // END SETTING PARAMETORS
    for(auto& it: TH1s)
      it.second.setting();
    for(auto& it: TH2s)
      it.second.setting();
    // RUN PARAMETORS AND HISTOGRAM
	  
    // SECOND FILE LOOP (Analysis) READ DATA ------------------------------------
    std::cout << "\n  Analysis loop..." << std::endl;
    for(unsigned int i = 0; i < _inputs.size(); i++) {
			
      if(runs.at(r) == this->numRun(parser.fileName(_inputs.at(i))))
        isFindRun = true;
      if(runs.at(r) != this->numRun(parser.fileName(_inputs.at(i)))) {
        if(i < _inputs.size() - 1)
          continue;
        else if(!isFindRun) {
          _log += " WARNING! RUN: " 
	  				 + std::to_string(runs.at(r)) + " was skipped! Can't find files!" 
	  				 + _rowDelimiter;
          continue;
        }
        else
          continue;
      }


  	  std::cout << "    File[" << i << "]: " 
														 << parser.fileName(_inputs.at(i)) << std::endl;

  	  isOpen = stream.open(_inputs.at(i));
  	  if (!isOpen) {
  	    _log += " WARNING! file: " 
						 + parser.fileName(_inputs.at(i))  + " was skipped! Can't open!" 
						 + _rowDelimiter;
  	    continue;
  	  }

      // read raw data
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
  	    event.clear(); event.fill(&buffers); 

  	    // clear memory form old buffers
  	    for (int j = 0; j < (int)buffers.size(); j++)
  	      delete buffers.at(j);
  	    buffers.clear();
  	  
  	    bool isEvent = event.check();
  	    if(!isEvent) {
  	      _log += " WARNING! Event in pos: " + std::to_string(prev) + " was skipped! Can't open!" 
				       + _rowDelimiter;
  	      continue;
  	    }
	     // ----------------------------------------------------------------------

  	    // ANALYSIS BEGIN
				for(unsigned int ic = 0; ic < event.chambers()->size(); ic++) {
          int chamber = event.chambers()->at(ic); 
          analysis.find(chamber)->second.setStrips(event.strips(chamber));

          // TRIGS
          for(unsigned int jt = 0; jt < event.trigs(chamber)->size(); jt++) {
            int boardID = event.trigs(chamber)->at(jt)->head->boardID; 
            nameTH1s = Form("%d%snum%d%sbuffer", chamber, strDelim.c_str(), boardID, strDelim.c_str());
            TH1s.find(nameTH1s.c_str())->second.Fill(event.trigs(chamber)->at(jt)->head->numOfCh);
            //---
            nameTH1s = Form("%d%snum%d%strigBCID", chamber, strDelim.c_str(), boardID,strDelim.c_str());
            TH1s.find(nameTH1s.c_str())->second.Fill(event.trigs(chamber)->at(jt)->BCID);
          }
          unsigned int numberOfTrigers = event.trigs(chamber)->size();
          nameTH1s = Form("%d%sNumberOfTrigers", chamber, strDelim.c_str()); 
          TH1s.find(nameTH1s.c_str())->second.Fill(numberOfTrigers);
				 
          bool isTrigers = analysis.find(chamber)->second.fillTrigers(numberOfTrigers);
					if(isTrigers) {
            for(unsigned int js = 0; js < event.strips(chamber)->size(); js++) {
              // HR
              double numberOfHitsHR = event.strips(chamber)->at(js).HR.size(); 
              nameTH1s = Form("%d%sNumberOfHitsHR", chamber, strDelim.c_str()); 
              TH1s.find(nameTH1s.c_str())->second.Fill(numberOfHitsHR);
              double numberOfHitsLR = event.strips(chamber)->at(js).LR.size();
              nameTH1s = Form("%d%sNumberOfHitsLR", chamber, strDelim.c_str()); 
              TH1s.find(nameTH1s.c_str())->second.Fill(numberOfHitsLR);
              for(unsigned int jh = 0; jh < event.strips(chamber)->at(js).HR.size(); jh++) {
                bool isFilterHR = analysis.find(chamber)->second.filters("data&HR", js, jh); 
                if(isFilterHR)
                  analysis.find(chamber)->second.fillEvents("data&HR", js, jh); 
                bool isFilterNoiseHR = analysis.find(chamber)->second.filters("noise&HR", js, jh); 
                if(isFilterNoiseHR) 
                  analysis.find(chamber)->second.fillEvents("noise&HR", js, jh); 
                
                analysis.find(chamber)->second.filters("deadTime&HR", js, jh); // skip if deadTime 
              }
              // LR
         		  for(unsigned int jl = 0; jl < event.strips(chamber)->at(js).LR.size(); jl++) {
                bool isFilterLR = analysis.find(chamber)->second.filters("data&LR", js, jl); 
                if(isFilterLR)
                  analysis.find(chamber)->second.fillEvents("data&LR", js, jl); 
                bool isFilterNoiseLR = analysis.find(chamber)->second.filters("noise&LR", js, jl); 
                if(isFilterNoiseLR) 
                  analysis.find(chamber)->second.fillEvents("noise&LR", js, jl); 
                
                analysis.find(chamber)->second.filters("deadTime&LR", js, jl); // skip if deadTime 
         		  }
            }
					}
          if(!analysis.find(chamber)->second.skipEvent()) {  
            // ANALYSIS PART
            std::string filters = analysis.find(chamber)->second.filtersStr(); 
            for(unsigned int im = 0; im < mods.size(); im++) {
              analysis.find(chamber)->second.algos(mods.at(im));
              std::string modParams = analysis.find(chamber)->second.modParamsStr(mods.at(im)); 
              std::vector<std::pair<double, double>> data = analysis.find(chamber)->second.data(mods.at(im).c_str());
              for(unsigned int jd = 0; jd < data.size(); jd++) { 
                nameTH1s = Form("%d%s%s%s%sstripProfile", chamber, filters.c_str(), modParams.c_str(), strDelim.c_str(), mods.at(im).c_str()); 
                TH1s.find(nameTH1s.c_str())->second.Fill(data.at(jd).first);
                //--- 
                nameTH1s = Form("%d%s%s%s%stimeProfile", chamber, filters.c_str(), modParams.c_str(), strDelim.c_str(), mods.at(im).c_str()); 
                TH1s.find(nameTH1s.c_str())->second.Fill(data.at(jd).second);
                
                nameTH2s = Form("%d%s%s%s%sstripTimeProfile", chamber, filters.c_str(), modParams.c_str(), strDelim.c_str(), mods.at(im).c_str()); 
                TH2s.find(nameTH2s.c_str())->second.Fill(data.at(jd).second, data.at(jd).first);
              }
              double minTime = 0; bool isMinTime = analysis.find(chamber)->second.minTime(&minTime, mods.at(im).c_str());
              if(isMinTime) {
                nameTH1s = Form("%d%s%s%s%sminTime", chamber, filters.c_str(), modParams.c_str(), strDelim.c_str(), mods.at(im).c_str()); 
                TH1s.find(nameTH1s.c_str())->second.Fill(minTime);
              }
              double maxTime = 0; bool isMaxTime = analysis.find(chamber)->second.maxTime(&maxTime, mods.at(im).c_str());
              if(isMaxTime) {
                nameTH1s = Form("%d%s%s%s%smaxTime", chamber, filters.c_str(), modParams.c_str(), strDelim.c_str(), mods.at(im).c_str()); 
                TH1s.find(nameTH1s.c_str())->second.Fill(maxTime);
              }
              int mult = 0; bool isMult = analysis.find(chamber)->second.multiplicity(&mult, mods.at(im).c_str());
              if(isMult) {
                nameTH1s = Form("%d%s%s%s%smultiplicity", chamber, filters.c_str(), modParams.c_str(), strDelim.c_str(), mods.at(im).c_str()); 
                TH1s.find(nameTH1s.c_str())->second.Fill(mult);
              }
              std::vector<int> clusterSize; bool isClusterSize = analysis.find(chamber)->second.clusterSize(&clusterSize, mods.at(im).c_str());
              if(isClusterSize) {
                nameTH1s = Form("%d%s%s%s%sclusterSize", chamber, filters.c_str(), modParams.c_str(), strDelim.c_str(), mods.at(im).c_str()); 
                auto itCS = TH1s.find(nameTH1s.c_str());
                for(unsigned int i = 0; i < clusterSize.size(); i++)
                  itCS->second.Fill(clusterSize.at(i));
                clusterSize.clear();
              }
					  	bool isEff = analysis.find(chamber)->second.efficiency(mods.at(im).c_str()); // count events
              if(isEff) {
					  		nameTGEs = Form("%d%s%sefficiency", chamber, strDelim.c_str(), mods.at(im).c_str());   
              	itTGEs = dataRoot.addDqTGE(nameTGEs, &TGEs); itTGEs->second->config(nameTGEs, &_params);
					  	}
            }
          }
        }
        //
        // PART for analysis TRACING AND ETC STUDY CROSS chambers
        //--------------
        for(unsigned int ic = 0; ic < event.chambers()->size(); ic++)
          analysis.find(event.chambers()->at(ic))->second.clear("EVENT");
        // ANALYSIS END.
  	  	if(codeRead > 0)
					processBar.print(pos, 0.02);
				else
					processBar.print(size, 0.02);
  	    prev = pos;
		}
    //------
  	//------  
      std::cout << std::endl;
  	  if (isOpen) {
  	    isClose = stream.close();
  	    if (!isClose) {
  	      _log += " ERROR! file: " + parser.fileName(_inputs.at(i))  
																	 + " Can't close!" + _rowDelimiter;
  	    	return false;
				}
  	  }
	  }
    for(unsigned int ic = 0; ic < event.chambers()->size(); ic++) {
      int chamber = event.chambers()->at(ic);
      std::string filters = analysis.find(chamber)->second.filtersStr(); 
			double numSkipedTrigers = 0; double eNumSkipedTrigers = 0; bool isNumSkipedTrigers = analysis.find(chamber)->second.getNumSkipedTrigers(&numSkipedTrigers); 
			if(isNumSkipedTrigers) {
			  double value = analysis.find(chamber)->second.getValue(runs.at(r)); double eValue = 10;
				nameTGEs = Form("%d%snumberOfSkipedEventsWithTriger", chamber,  strDelim.c_str());   
        itTGEs = dataRoot.addDqTGE(nameTGEs, &TGEs); itTGEs->second->config(nameTGEs, &_params);
        TGEs.find(nameTGEs.c_str())->second->setting(value, eValue, numSkipedTrigers, eNumSkipedTrigers);
			}
			double numTrigers = 0; double eNumTrigers = 0; bool isNumTrigers = analysis.find(chamber)->second.getNumTrigers(&numTrigers); 
			if(isNumTrigers) {
			  double value = analysis.find(chamber)->second.getValue(runs.at(r)); double eValue = 10;
				nameTGEs = Form("%d%snumberOfEventsWithTriger", chamber,  strDelim.c_str());   
        itTGEs = dataRoot.addDqTGE(nameTGEs, &TGEs); itTGEs->second->config(nameTGEs, &_params);
        TGEs.find(nameTGEs.c_str())->second->setting(value, eValue, numTrigers, eNumTrigers);
			}
			int numEvents = analysis.find(chamber)->second.getNumberEvents();
			double value = analysis.find(chamber)->second.getValue(runs.at(r)); double eValue = 10;
			nameTGEs = Form("%d%snumberOfEvents", chamber, strDelim.c_str());   
      itTGEs = dataRoot.addDqTGE(nameTGEs, &TGEs); itTGEs->second->config(nameTGEs, &_params);
      TGEs.find(nameTGEs.c_str())->second->setting(value, eValue, numEvents, 0);
      for(unsigned int im = 0; im < mods.size(); im++) {
         std::string modParams = analysis.find(chamber)->second.modParamsStr(mods.at(im)); 
				double eff = 0; double eEff = 0; bool isEff = analysis.find(chamber)->second.efficiency(&eff, &eEff, mods.at(im).c_str()); 
				if(isEff) {
					double value = analysis.find(chamber)->second.getValue(runs.at(r)); double eValue = 10;
					nameTGEs = Form("%d%s%sefficiency", chamber, strDelim.c_str(), mods.at(im).c_str());   
          TGEs.find(nameTGEs.c_str())->second->setting(value, eValue, eff, eEff);
				}
				double fraction = 0; bool isFraction = analysis.find(chamber)->second.eventFraction(&fraction, mods.at(im).c_str());
				if(isFraction) {
					double value = analysis.find(chamber)->second.getValue(runs.at(r)); double eValue = 10;
					nameTGEs = Form("%d%s%sfraction", chamber, strDelim.c_str(), mods.at(im).c_str());   
          itTGEs = dataRoot.addDqTGE(nameTGEs, &TGEs); itTGEs->second->config(nameTGEs, &_params);
          TGEs.find(nameTGEs.c_str())->second->setting(value, eValue, fraction, 0);
				}
				double noise = 0; double eNoise = 0; bool isNoise = analysis.find(chamber)->second.noise(&noise, &eNoise, mods.at(im).c_str());
				eNoise = 0;
				if(isNoise) {
					double value = analysis.find(chamber)->second.getValue(runs.at(r)); double eValue = 10;
					nameTGEs = Form("%d%s%srate", chamber, strDelim.c_str(), mods.at(im).c_str());   
          itTGEs = dataRoot.addDqTGE(nameTGEs, &TGEs); itTGEs->second->config(nameTGEs, &_params);
          TGEs.find(nameTGEs.c_str())->second->setting(value, eValue, noise, eNoise);
				}
				nameTH1s = Form("%d%s%s%s%smultiplicity", chamber, filters.c_str(), modParams.c_str(), strDelim.c_str(), mods.at(im).c_str());
        auto itMult = TH1s.find(nameTH1s);
        if(!(itMult == TH1s.end())) {
          double mult = itMult->second.GetMean(); double eMult = 0;
				  double value = analysis.find(chamber)->second.getValue(runs.at(r)); double eValue = 10;
				  nameTGEs = Form("%d%s%smultiplicity", chamber, strDelim.c_str(), mods.at(im).c_str());   
          itTGEs = dataRoot.addDqTGE(nameTGEs, &TGEs); itTGEs->second->config(nameTGEs, &_params);
          TGEs.find(nameTGEs.c_str())->second->setting(value, eValue, mult, eMult);
        }
				nameTH1s = Form("%d%s%s%s%sclusterSize", chamber, filters.c_str(), modParams.c_str(), strDelim.c_str(), mods.at(im).c_str());
        auto itClusterSize = TH1s.find(nameTH1s);
        if(!(itClusterSize == TH1s.end())) {
          double clusterSize = itClusterSize->second.GetMean(); double eClusterSize = 0;
				  double value = analysis.find(chamber)->second.getValue(runs.at(r)); double eValue = 10;
				  nameTGEs = Form("%d%s%sclusterSize", chamber, strDelim.c_str(), mods.at(im).c_str());   
          itTGEs = dataRoot.addDqTGE(nameTGEs, &TGEs); itTGEs->second->config(nameTGEs, &_params);
          TGEs.find(nameTGEs.c_str())->second->setting(value, eValue, clusterSize, eClusterSize);
        }
				nameTH1s = Form("%d%sNumberOfHits%s", chamber, strDelim.c_str(), mods.at(im).c_str());
        auto itMHR = TH1s.find(nameTH1s);
        if(!(itMHR == TH1s.end())) {
          double mult = itMHR->second.GetEntries(); double eMult = 0;
				  double value = analysis.find(chamber)->second.getValue(runs.at(r)); double eValue = 10;
				  nameTGEs = Form("%d%s%sNumberOfHits", chamber, strDelim.c_str(), mods.at(im).c_str());   
          itTGEs = dataRoot.addDqTGE(nameTGEs, &TGEs); itTGEs->second->config(nameTGEs, &_params);
          TGEs.find(nameTGEs.c_str())->second->setting(value, eValue, mult, eMult);
        }
        nameTH2s = Form("%d%s%s%s%sstripTimeProfile", chamber, filters.c_str(), modParams.c_str(), strDelim.c_str(), mods.at(im).c_str()); 
        auto itFitST = TH2s.find(nameTH2s);
        if(!(itFitST == TH2s.end())) {
          double value = analysis.find(chamber)->second.getValue(runs.at(r)); 
          nameTGEs = Form("%d%s%d%s%sgStripTimeProfile", chamber, strDelim.c_str(), (int)value, strDelim.c_str(), mods.at(im).c_str());   
          itTGEs = dataRoot.addDqTGE(nameTGEs, &TGEs); itTGEs->second->config(nameTGEs, &_params);
          std::cout << TH2s.find(nameTH2s.c_str())->second.GetNbinsX() << std::endl;
            int strips = event.numberOfStrips(chamber);
            for(unsigned int c = 0; c < strips; c++) {
            if(TH2s.find(nameTH2s.c_str())->second.ProjectionX("h", c, c)->GetEntries() > 100) {
              TH1D *h = TH2s.find(nameTH2s.c_str())->second.ProjectionX("h", c, c);
              double mean = h->GetMean(); double rms = h->GetRMS();
              TF1 *f1 = new TF1("f1", "gaus", mean-3*rms, mean+3*rms);
              h->Fit("f1", "R");
              TGEs.find(nameTGEs.c_str())->second->setting(c, 0, f1->GetParameter(1),  f1->GetParameter(2)/std::sqrt(h->GetEntries()));
            }
          }   
          //------   
        }
      }
      analysis.find(event.chambers()->at(ic))->second.clear("RUN");
    }

    for(auto profiles: TH1s) {
      int chamberNum = -1;
      double value = runs.at(r);
      std::string chamber = parser.chamberName(profiles.first);
      bool isChamber = parser.strToInt(&chamberNum, chamber); 
      if(isChamber) { 
        auto it = analysis.find(chamberNum);
        if(!(it == analysis.end()))
          value = analysis.find(chamberNum)->second.getValue(runs.at(r));
      }

      std::string histName = parser.histName(profiles.first);
      std::string comment = this->findParam("comment", &_params);
        std::string path = Form("%s_%s/param_%.2f%srun_%d/chamber_%d", comment.c_str(), logRunNumber.c_str(), value, strDelim.c_str(), runs.at(r), chamberNum); 
      if(comment == "webdcs")
        path = Form("param_%.2f%srun_%d/chamber_%d", value, strDelim.c_str(), runs.at(r), chamberNum); 
      std::string folder = "";
      bool isFolder = this->findParam((histName + "_path"), &folder, &_params);
      if(isFolder)
        path += "/" + folder;
      else 
        path += "/withoutPath";
      dataRoot.writeTH1toCanvasCMS(path, &profiles.second, false);
    }
    TH1s.clear();
    
    for(auto profiles: TH2s) {
      int chamberNum = -1;
      double value = runs.at(r);
      std::string chamber = parser.chamberName(profiles.first);
      bool isChamber = parser.strToInt(&chamberNum, chamber); 
      if(isChamber) { 
        auto it = analysis.find(chamberNum);
        if(!(it == analysis.end()))
          value = analysis.find(chamberNum)->second.getValue(runs.at(r));
      }
      std::string histName = parser.histName(profiles.first);
      std::string comment = this->findParam("comment", &_params);
      std::string path = Form("%s_%s/param_%.2f%srun_%d/chamber_%d", comment.c_str(), logRunNumber.c_str(), value, strDelim.c_str(), runs.at(r), chamberNum); 
      if(comment == "webdcs")
        path = Form("param_%.2f%srun_%d/chamber_%d", value, strDelim.c_str(), runs.at(r), chamberNum); 
      std::string folder = "";
      bool isFolder = this->findParam((histName + "_path"), &folder, &_params);
      if(isFolder)
        path += "/" + folder;
      else 
        path += "/withoutPath";
      dataRoot.writeTH2toCanvasCMS(path, &profiles.second, false);
    }
    TH2s.clear();
  }
  std::cout << "End of Processing!" << std::endl;
  std::cout << "#-------------------------------------------------------------------------------\n" << std::endl;
	std::cout  << "Comparing cookies and Cleaning the kitchen..." << std::endl;
  for(auto profiles: TGEs) {
 		profiles.second->sortByX(); 
    profiles.second->plot();
    std::string histName = parser.histName(profiles.first);
    std::string comment = this->findParam("comment", &_params);
    std::string path = comment + "_" + logRunNumber + "/analysis/chamber_" + parser.chamberName(profiles.first); 
    if(comment == "webdcs")
      path = "analysis/chamber_" + parser.chamberName(profiles.first); 
    std::string folder = "";
    bool isFolder = this->findParam((histName + "_path"), &folder, &_params);
    if(folder == "print") {
      std::string line;
      std::vector<std::string> lines;
      lines.push_back(profiles.first);
      profiles.second->print(&lines);
          for(unsigned int i = 0; i < lines.size(); i++)
            std::cout << lines.at(i) << std::endl;
      std::string pathFile = "rate_"+ profiles.first + "_" + comment + ".txt";
       ifstream myfile(pathFile.c_str());
      if(myfile.is_open()) {
        int ss = 0;
        while(getline(myfile, line)) {
          std::cout << line;
          lines[ss] = lines[ss] + " " + line;
          ss+=1;
        }
        myfile.close();
      }
      else std::cout << "Unable to open file";
      ofstream myfile2 (pathFile.c_str(), std::ios_base::out | std::ios_base::trunc);
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
    dataRoot.writeDqTGEtoCanvasCMS(path, profiles.second, false);
    delete profiles.second;
  }
  TGEs.clear();
  
	std::cout << "You're welcome!" << std::endl;
  std::cout << "#---\n" << std::endl;
  std::string comment = this->findParam("comment", &_params);
  std::cout << "#COMMENT: " << comment << "_" << logRunNumber <<  std::endl;
  std::cout << "#OUTPUT FILE: " << rootFilePath <<  std::endl;
  return true;
}

bool LyPeti::binToROOT() 
{
  std::cout << "#---\n" << std::endl;
	std::cout  << "Getting ingredients for cookies..." << std::endl;
  // GENERAL ANALYSIS PARAMETORS -----------------------------------------------
  std::string logRunNumber; 
  std::string plusOutDir; 
  std::string rootFilePath;
  LyPetiEvent event;
  std::vector<std::string> mods;
  std::map<int, LyPetiAnalysis> analysis;
  std::vector<int> runs;
  // ---------------------------------------------------------------------------

  // PARSING CARD PARAMETORS ---------------------------------------------------
  
  // system parametors
  std::vector<std::string> parserArrayDelimiters;
  parserArrayDelimiters.push_back(this->findParam("sys_DqParser_AD_1", &_params));
  parserArrayDelimiters.push_back(this->findParam("sys_DqParser_AD_2", &_params));
  parserArrayDelimiters.push_back(this->findParam("sys_DqParser_AD_3", &_params));
  parserArrayDelimiters.push_back(this->findParam("sys_DqParser_AD_4", &_params));
  parserArrayDelimiters.push_back(this->findParam("sys_DqParser_AD_5", &_params));
  DqParser parser; parser.setArrayDelimiter(parserArrayDelimiters);

  std::string strDelim = this->findParam("sys_DqParser_AD_3", &_params);
  
  logRunNumber = this->findParam("logNumber", &_params);  
  
  plusOutDir = this->findParam("dir", &_params);  
  std::string defaultPath = parser.filePath(_outputs.at(0));
  rootFilePath = defaultPath + plusOutDir;

  std::string sMods = this->findParam("mods", &_params);
  mods = parser.parsArray(sMods);

	// Runs info
  std::string sRuns = this->findParam("runs", &_params);
  std::vector<std::string> vsRuns = parser.parsArray(sRuns);
  runs = parser.strToIntArray(vsRuns);
  sRuns.clear(); vsRuns.clear();
  
  std::string sStrips = this->findParam("strips", &_params);
  std::vector<std::string> vsStrips = parser.parsArray(sStrips);
  std::vector<int> strips = parser.strToIntArray(vsStrips);
  sStrips.clear(); vsStrips.clear();
  
  std::string sTrigCh = this->findParam("trigCh", &_params);
  std::vector<std::string> vsTrigCh = parser.parsArray(sTrigCh);
  std::vector<double> trigCh = parser.strToDoubleArray(vsTrigCh);
  sTrigCh.clear(); vsTrigCh.clear();
  
  std::string sHR = this->findParam("HR", &_params);
  std::vector<std::string> vsHR = parser.parsArray(sHR);
  std::vector<double> HR = parser.strToDoubleArray(vsHR);
  sHR.clear(); vsHR.clear();
  
  std::string sLR = this->findParam("LR", &_params);
  std::vector<std::string> vsLR = parser.parsArray(sLR);
  std::vector<double> LR = parser.strToDoubleArray(vsLR);
  sLR.clear(); vsLR.clear();
  
  std::string sTDCs = this->findParam("TDCs", &_params);
  std::vector<std::string> vsTDCs = parser.parsArray(sTDCs);
  std::vector<double> TDCs = parser.strToDoubleArray(vsTDCs);
  sTDCs.clear(); vsTDCs.clear();
  
  std::string sTimeOffSet = this->findParam("timeOffSet", &_params);
  std::vector<std::string> vsTimeOffSet = parser.parsArray(sTimeOffSet);
  std::vector<double> timeOffSet = parser.strToDoubleArray(vsTimeOffSet);
  sTimeOffSet.clear(); vsTimeOffSet.clear();
  
  std::string sTimeOffSetHR = this->findParam("timeOffSetHR", &_params);
  std::vector<std::string> vsTimeOffSetHR = parser.parsArray(sTimeOffSetHR);
  std::vector<double> timeOffSetHR = parser.strToDoubleArray(vsTimeOffSetHR);
  sTimeOffSetHR.clear(); vsTimeOffSetHR.clear();
 
  std::string sTimeOffSetLR = this->findParam("timeOffSetLR", &_params);
  std::vector<std::string> vsTimeOffSetLR = parser.parsArray(sTimeOffSetLR);
  std::vector<double> timeOffSetLR = parser.strToDoubleArray(vsTimeOffSetLR);
  sTimeOffSetLR.clear(); vsTimeOffSetLR.clear();
  
  std::string sInverse = this->findParam("inverse", &_params);
  std::vector<std::string> vsInverse = parser.parsArray(sInverse);
  std::vector<double> inverse = parser.strToDoubleArray(vsInverse);
  sInverse.clear(); vsInverse.clear();
  
  std::string sChambers = this->findParam("chambers", &_params);
  std::vector<std::string> vsChambers = parser.parsArray(sChambers);
  std::vector<double> chambers = parser.strToDoubleArray(vsChambers);
  sChambers.clear(); vsChambers.clear(); 
  
  // ---------------------------------------------------------------------------
	
  // ANALYSIS INPUT PARAMETORS -------------------------------------------------
	// fill maping 
  bool isMap = false;
  isMap = event.mapping(strips, trigCh, TDCs, 
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
  
  std::cout << "\n#---" << std::endl;
  // chambers in processing  
	std::cout  << "N: " << event.chambers()->size() << " chamber(s) in Processing:";
	for(unsigned int i = 0; i < event.chambers()->size(); i++)
		std::cout << " index=" << event.chambers()->at(i);
	std::cout << std::endl;
	
  // COMAND LINE ---------------------------------------------------------------
  if(runs.size() > 0) {
    int processingFiles = 0;

    for(unsigned int r = 0; r < runs.size(); r++) {
	    for(unsigned int i = 0; i < _inputs.size(); i++) {
        if(runs.at(r) == this->numRun(parser.fileName(_inputs.at(i))))
          processingFiles += 1;
      }
    }
    if(processingFiles > 0)
      std::cout <<"N: " << processingFiles << " file(s) in Processing... / ";
    else {
      _log += " ERROR! Processing file(s): " 
	  			 + std::to_string(processingFiles)  + "!" + _rowDelimiter;
      return false;
    }
  }
  else {  
    runs.clear();
  	runs = this->parsNumRuns(_inputs);
    std::cout <<"N: " << _inputs.size() << " file(s) in Processing... / ";
  }
  std::cout <<"N: " << runs.size() << " run(s) in Processing..." << std::endl;
  // ---------------------------------------------------------------------------  
  
  std::cout << "#---" << std::endl;
	std::cout  << "\nStill Cooking the cookies..." << std::endl;
	
	// ANALYSIS CLASS SETTING ----------------------------------------------------	
  for(unsigned int i = 0; i < event.chambers()->size(); i++) {
    int chamber = event.chambers()->at(i); 
    auto itA = analysis.find(chamber);
    if(itA == analysis.end())
      analysis.insert(std::make_pair(chamber, LyPetiAnalysis{}));

    analysis.find(chamber)->second.setParser(&parser);
    
    // take values for runs 
    std::vector<double> values; 
    std::string sValues; 
    bool isValues = this->findParam((std::to_string(chamber) + strDelim + "values"), &sValues, &_params);
    if(isValues) {
      std::vector<std::string> vsValues = parser.parsArray(sValues);
      values = parser.strToDoubleArray(vsValues);
      sValues.clear(); vsValues.clear();
    }
    else {
      bool isValuesDef = this->findParam("values", &sValues, &_params);
      if(isValuesDef) {
    	  _log += " Chamber:  " + std::to_string(chamber) + " is using values !" + _rowDelimiter;
        std::vector<std::string> vsValues = parser.parsArray(sValues);
        values = parser.strToDoubleArray(vsValues);
        sValues.clear(); vsValues.clear();
      }
    }
	 	
		// take filters params
    std::map<std::string, double> algoParams; 
 		for(auto& itP: _params) {
			std::string chamberName = parser.chamberName(itP.first); // first param before delim
			bool isAlgo = parser.findParamFromStr("algo", itP.first);
			bool isFilter = parser.findParamFromStr("filter", itP.first);
			if(chamberName == std::to_string(chamber) && (isFilter || isAlgo)) {
        double value = 0;
				bool isValue = parser.strToDouble(&value, itP.second);
				if(isValue)
      	 	algoParams.insert(std::make_pair(parser.histName(itP.first), value));
			}
			if((isAlgo || isFilter) && (chamberName == "filter" || chamberName == "algo") ) {
    		_log += " WARNING: Param for chamber: " + std::to_string(chamber)  + " is " + itP.first + "." + _rowDelimiter;
  			double value = 0;
				bool isValue = parser.strToDouble(&value, itP.second);
				if(isValue)
      	 	algoParams.insert(std::make_pair(parser.histName(itP.first), value));
			}
		}
    analysis.find(chamber)->second.setRuns(runs, values);
    analysis.find(chamber)->second.setParams(algoParams);
    values.clear(); algoParams.clear();
    bool isAnalysis = analysis.find(chamber)->second.configure();
		if(!isAnalysis) {
    	_log += " ERROR: Can't configure analysis for chamber: " + std::to_string(chamber) + "!" + _rowDelimiter;
			return false;
		}
  }
  // ---------------------------------------------------------------------------
  
  strips.clear(); trigCh.clear(); TDCs.clear(); chambers.clear(); 
  HR.clear(); timeOffSetHR.clear(); LR.clear(); timeOffSetLR.clear(); timeOffSet.clear(); inverse.clear(); // clear memory (map params) 
  // ---------------------------------------------------------------------------

  
  LyPetiRead stream; 
  bool isOpen = false;
  bool isClose = false;
  
  // runs params
  std::cout  << "Cookies are ready, invite you to enjoy the taste!" << std::endl;
  std::cout << "#-------------------------------------------------------------------------------" << std::endl;
	for(unsigned int r = 0; r < runs.size(); r++) {
	  DqRoot dataRoot; dataRoot.setOutputFile(Form("%s/RF_%d.root", rootFilePath.c_str(), runs.at(r)), "default");
    //TFile file(Form("%s/RF_%d.root", parser.filePath(_outputs.at(0)).c_str(), r),"RECREATE");
        Int_t number;
        Float_t time; 
        Float_t ch;
        TTree T("RAWData", "RAWData");
        T.Branch("EventNumber", &number,"EventNumber/I");
        T.Branch("TDC_channel", &ch,"TDC_channel/F");
        T.Branch("TDC_TimeStamp", &time,"TDC_TimeStamp/F");
    std::cout << std::endl << "Run[" << r << "]: " << runs.at(r);
    
    // LOOP (Analysis) READ DATA ------------------------------------
    bool isFindRun = false;
    std::cout << "\n  Converting loop..." << std::endl;
    for(unsigned int i = 0; i < _inputs.size(); i++) {
			
      if(runs.at(r) == this->numRun(parser.fileName(_inputs.at(i))))
        isFindRun = true;
      if(runs.at(r) != this->numRun(parser.fileName(_inputs.at(i)))) {
        if(i < _inputs.size() - 1)
          continue;
        else if(!isFindRun) {
          _log += " WARNING! RUN: " 
	  				 + std::to_string(runs.at(r)) + " was skipped! Can't find files!" 
	  				 + _rowDelimiter;
          continue;
        }
        else
          continue;
      }

  	  std::cout << "    File[" << i << "]: " 
														 << parser.fileName(_inputs.at(i)) << std::endl;

  	  isOpen = stream.open(_inputs.at(i));
  	  if (!isOpen) {
  	    _log += " WARNING! file: " 
						 + parser.fileName(_inputs.at(i))  + " was skipped! Can't open!" 
						 + _rowDelimiter;
  	    continue;
  	  }

      // read raw data
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
  	    event.clear(); event.fill(&buffers); 

  	    // clear memory form old buffers
  	    for (int j = 0; j < (int)buffers.size(); j++)
  	      delete buffers.at(j);
  	    buffers.clear();
  	  
  	    bool isEvent = event.check();
  	    if(!isEvent) {
  	      _log += " WARNING! Event in pos: " + std::to_string(prev) + " was skipped! Can't open!" 
				       + _rowDelimiter;
  	      continue;
  	    }
	     // ----------------------------------------------------------------------

  	    // ANALYSIS BEGIN
				for(unsigned int ic = 0; ic < event.chambers()->size(); ic++) {
          int chamber = event.chambers()->at(ic); 
         // std::cout << "chamber=" << chamber << std::endl;
          unsigned int numberOfTrigers = event.trigs(chamber)->size();
          bool isTrigers = analysis.find(chamber)->second.fillTrigers(numberOfTrigers);
					if(isTrigers) {
            for(unsigned int js = 0; js < event.strips(chamber)->size(); js++) {
              if(event.strips(chamber)->at(js).trigs.size() == 1) {
                for(unsigned int jh = 0; jh < event.strips(chamber)->at(js).HR.size(); jh++) {
              number = event.strips(chamber)->at(js).trigs.at(0)->head->number;
                  ch = event.strips(chamber)->at(js).number;
                  time = event.strips(chamber)->at(js).HR.at(jh)->time -  event.strips(chamber)->at(js).trigs.at(0)->time;
                //  std::cout << std::setprecision(3) << "HR:" << ch << "|" << time << std::endl;
                  T.Fill();
                }
         		    for(unsigned int jl = 0; jl < event.strips(chamber)->at(js).LR.size(); jl++) {
                  ch = event.strips(chamber)->at(js).number + 500;
                  time = event.strips(chamber)->at(js).LR.at(jl)->time -  event.strips(chamber)->at(js).trigs.at(0)->time;
                //  std::cout << std::setprecision(3) << "LR:" << ch << "|" << time << std::endl; 
                  T.Fill();
         		    }
              }
            }
					}
		    }
        for(unsigned int ic = 0; ic < event.chambers()->size(); ic++)
          analysis.find(event.chambers()->at(ic))->second.clear("EVENT");
        // ANALYSIS END.
  	  	if(codeRead > 0)
					processBar.print(pos, 0.02);
				else
					processBar.print(size, 0.02);
  	    prev = pos;
  	//------  
    }
        std::cout << std::endl;
  	    if(isOpen) {
  	      isClose = stream.close();
  	      if(!isClose) {
  	        _log += " ERROR! file: " + parser.fileName(_inputs.at(i))  
																	 + " Can't close!" + _rowDelimiter;
  	    	  return false;
				  }
  	    }
	    }
//        T.Write();
        dataRoot.writeObject("", &T);
      for(unsigned int ic = 0; ic < event.chambers()->size(); ic++)
        analysis.find(event.chambers()->at(ic))->second.clear("RUN");
  }
  std::cout << "End of Processing!" << std::endl;
  std::cout << "#-------------------------------------------------------------------------------\n" << std::endl;
	std::cout  << "Comparing cookies and Cleaning the kitchen..." << std::endl;
  
	std::cout << "You're welcome!" << std::endl;
  std::cout << "#---\n" << std::endl;
  std::string comment = this->findParam("comment", &_params);
  std::cout << "#COMMENT: " << comment << "_" << logRunNumber <<  std::endl;
  std::cout << "#OUTPUT FILE: " << rootFilePath <<  std::endl;
  return true;
}

//bool LyPeti::binToLyROOT() 
//{
//  std::cout << "#---\n" << std::endl;
//	std::cout  << "Getting ingredients for cookies..." << std::endl;
//  // GENERAL ANALYSIS PARAMETORS -----------------------------------------------
//  std::string logRunNumber; 
//  std::string plusOutDir; 
//  std::string rootFilePath;
//  std::string outName;
//  LyPetiEvent event;
//  std::vector<std::string> mods;
//  std::map<int, LyPetiAnalysis> analysis;
//  std::vector<int> runs;
//  // ---------------------------------------------------------------------------
//
//  // PARSING CARD PARAMETORS ---------------------------------------------------
//  
//  // system parametors
//  std::vector<std::string> parserArrayDelimiters;
//  parserArrayDelimiters.push_back(this->findParam("sys_DqParser_AD_1", &_params));
//  parserArrayDelimiters.push_back(this->findParam("sys_DqParser_AD_2", &_params));
//  parserArrayDelimiters.push_back(this->findParam("sys_DqParser_AD_3", &_params));
//  parserArrayDelimiters.push_back(this->findParam("sys_DqParser_AD_4", &_params));
//  parserArrayDelimiters.push_back(this->findParam("sys_DqParser_AD_5", &_params));
//  DqParser parser; parser.setArrayDelimiter(parserArrayDelimiters);
//
//  std::string strDelim = this->findParam("sys_DqParser_AD_3", &_params);
//  
//  logRunNumber = this->findParam("logNumber", &_params);  
//  
//  plusOutDir = this->findParam("dir", &_params);  
//  outName = this->findParam("fN", &_params);
//  std::string defaultPath = parser.filePath(_outputs.at(0));
//  rootFilePath = defaultPath + plusOutDir + "/" + outName;
//
//  std::string sMods = this->findParam("mods", &_params);
//  mods = parser.parsArray(sMods);
//
//	// Runs info
//  std::string sRuns = this->findParam("runs", &_params);
//  std::vector<std::string> vsRuns = parser.parsArray(sRuns);
//  runs = parser.strToIntArray(vsRuns);
//  sRuns.clear(); vsRuns.clear();
//  
//  std::string sStrips = this->findParam("strips", &_params);
//  std::vector<std::string> vsStrips = parser.parsArray(sStrips);
//  std::vector<int> strips = parser.strToIntArray(vsStrips);
//  sStrips.clear(); vsStrips.clear();
//  
//  std::string sTrigCh = this->findParam("trigCh", &_params);
//  std::vector<std::string> vsTrigCh = parser.parsArray(sTrigCh);
//  std::vector<double> trigCh = parser.strToDoubleArray(vsTrigCh);
//  sTrigCh.clear(); vsTrigCh.clear();
//  
//  std::string sHR = this->findParam("HR", &_params);
//  std::vector<std::string> vsHR = parser.parsArray(sHR);
//  std::vector<double> HR = parser.strToDoubleArray(vsHR);
//  sHR.clear(); vsHR.clear();
//  
//  std::string sLR = this->findParam("LR", &_params);
//  std::vector<std::string> vsLR = parser.parsArray(sLR);
//  std::vector<double> LR = parser.strToDoubleArray(vsLR);
//  sLR.clear(); vsLR.clear();
//  
//  std::string sTDCs = this->findParam("TDCs", &_params);
//  std::vector<std::string> vsTDCs = parser.parsArray(sTDCs);
//  std::vector<double> TDCs = parser.strToDoubleArray(vsTDCs);
//  sTDCs.clear(); vsTDCs.clear();
//  
//  std::string sTimeOffSet = this->findParam("timeOffSet", &_params);
//  std::vector<std::string> vsTimeOffSet = parser.parsArray(sTimeOffSet);
//  std::vector<double> timeOffSet = parser.strToDoubleArray(vsTimeOffSet);
//  sTimeOffSet.clear(); vsTimeOffSet.clear();
//  
//  std::string sTimeOffSetHR = this->findParam("timeOffSetHR", &_params);
//  std::vector<std::string> vsTimeOffSetHR = parser.parsArray(sTimeOffSetHR);
//  std::vector<double> timeOffSetHR = parser.strToDoubleArray(vsTimeOffSetHR);
//  sTimeOffSetHR.clear(); vsTimeOffSetHR.clear();
// 
//  std::string sTimeOffSetLR = this->findParam("timeOffSetLR", &_params);
//  std::vector<std::string> vsTimeOffSetLR = parser.parsArray(sTimeOffSetLR);
//  std::vector<double> timeOffSetLR = parser.strToDoubleArray(vsTimeOffSetLR);
//  sTimeOffSetLR.clear(); vsTimeOffSetLR.clear();
//  
//  std::string sInverse = this->findParam("inverse", &_params);
//  std::vector<std::string> vsInverse = parser.parsArray(sInverse);
//  std::vector<double> inverse = parser.strToDoubleArray(vsInverse);
//  sInverse.clear(); vsInverse.clear();
//  
//  std::string sChambers = this->findParam("chambers", &_params);
//  std::vector<std::string> vsChambers = parser.parsArray(sChambers);
//  std::vector<double> chambers = parser.strToDoubleArray(vsChambers);
//  sChambers.clear(); vsChambers.clear(); 
//  
//  // ---------------------------------------------------------------------------
//	
//  // ANALYSIS INPUT PARAMETORS -------------------------------------------------
//	// fill maping 
//  bool isMap = false;
//  isMap = event.mapping(strips, trigCh, TDCs, 
//                HR, timeOffSetHR, 
//                LR, timeOffSetLR,  
//                timeOffSet,
//                inverse,
//                chambers); // inside check map arrays.
//  if(!isMap) {
//      _log += " ERROR! Map isn't correct, Size of arrays are not equal!" + _rowDelimiter
//					 + "strips=" + std::to_string(strips.size()) + parserArrayDelimiters.at(0) 
//					 + "TDCs=" + std::to_string(TDCs.size()) + parserArrayDelimiters.at(0) 
//					 + "trigCh=" + std::to_string(trigCh.size()) + parserArrayDelimiters.at(0)
//					 + "HR=" + std::to_string(HR.size()) + parserArrayDelimiters.at(0)
//					 + "LR=" + std::to_string(LR.size()) + parserArrayDelimiters.at(0)
//					 + "timeOffSetHR=" + std::to_string(timeOffSetHR.size()) + parserArrayDelimiters.at(0)
//					 + "timeOffSetLR=" + std::to_string(timeOffSetLR.size()) + parserArrayDelimiters.at(0)
//					 + "timeOffSet=" + std::to_string(timeOffSet.size()) + parserArrayDelimiters.at(0)
//					 + "inverse=" + std::to_string(inverse.size()) + parserArrayDelimiters.at(0)
//					 + "asciiChambers=" + std::to_string(chambers.size()) + _rowDelimiter;
//      return false;
//  }
//  
//  std::cout << "\n#---" << std::endl;
//  // chambers in processing  
//	std::cout  << "N: " << event.chambers()->size() << " chamber(s) in Processing:";
//	for(unsigned int i = 0; i < event.chambers()->size(); i++)
//		std::cout << " index=" << event.chambers()->at(i);
//	std::cout << std::endl;
//	
//  // COMAND LINE ---------------------------------------------------------------
//  if(runs.size() > 0) {
//    int processingFiles = 0;
//
//    for(unsigned int r = 0; r < runs.size(); r++) {
//	    for(unsigned int i = 0; i < _inputs.size(); i++) {
//        if(runs.at(r) == this->numRun(parser.fileName(_inputs.at(i))))
//          processingFiles += 1;
//      }
//    }
//    if(processingFiles > 0)
//      std::cout <<"N: " << processingFiles << " file(s) in Processing... / ";
//    else {
//      _log += " ERROR! Processing file(s): " 
//	  			 + std::to_string(processingFiles)  + "!" + _rowDelimiter;
//      return false;
//    }
//  }
//  else {  
//    runs.clear();
//  	runs = this->parsNumRuns(_inputs);
//    std::cout <<"N: " << _inputs.size() << " file(s) in Processing... / ";
//  }
//  std::cout <<"N: " << runs.size() << " run(s) in Processing..." << std::endl;
//  // ---------------------------------------------------------------------------  
//  
//  std::cout << "#---" << std::endl;
//	std::cout  << "\nStill Cooking the cookies..." << std::endl;
//	
//	// ANALYSIS CLASS SETTING ----------------------------------------------------	
//  for(unsigned int i = 0; i < event.chambers()->size(); i++) {
//    int chamber = event.chambers()->at(i); 
//    auto itA = analysis.find(chamber);
//    if(itA == analysis.end())
//      analysis.insert(std::make_pair(chamber, LyPetiAnalysis{}));
//
//    analysis.find(chamber)->second.setParser(&parser);
//    
//    // take values for runs 
//    std::vector<double> values; 
//    std::string sValues; 
//    bool isValues = this->findParam((std::to_string(chamber) + strDelim + "values"), &sValues, &_params);
//    if(isValues) {
//      std::vector<std::string> vsValues = parser.parsArray(sValues);
//      values = parser.strToDoubleArray(vsValues);
//      sValues.clear(); vsValues.clear();
//    }
//    else {
//      bool isValuesDef = this->findParam("values", &sValues, &_params);
//      if(isValuesDef) {
//    	  _log += " Chamber:  " + std::to_string(chamber) + " is using values !" + _rowDelimiter;
//        std::vector<std::string> vsValues = parser.parsArray(sValues);
//        values = parser.strToDoubleArray(vsValues);
//        sValues.clear(); vsValues.clear();
//      }
//    }
//	 	
//		// take filters params
//    std::map<std::string, double> algoParams; 
// 		for(auto& itP: _params) {
//			std::string chamberName = parser.chamberName(itP.first); // first param before delim
//			bool isAlgo = parser.findParamFromStr("algo", itP.first);
//			bool isFilter = parser.findParamFromStr("filter", itP.first);
//			if(chamberName == std::to_string(chamber) && (isFilter || isAlgo)) {
//        double value = 0;
//				bool isValue = parser.strToDouble(&value, itP.second);
//				if(isValue)
//      	 	algoParams.insert(std::make_pair(parser.histName(itP.first), value));
//			}
//			if((isAlgo || isFilter) && (chamberName == "filter" || chamberName == "algo") ) {
//    		_log += " WARNING: Param for chamber: " + std::to_string(chamber)  + " is " + itP.first + "." + _rowDelimiter;
//  			double value = 0;
//				bool isValue = parser.strToDouble(&value, itP.second);
//				if(isValue)
//      	 	algoParams.insert(std::make_pair(parser.histName(itP.first), value));
//			}
//		}
//    analysis.find(chamber)->second.setRuns(runs, values);
//    analysis.find(chamber)->second.setParams(algoParams);
//    values.clear(); algoParams.clear();
//    bool isAnalysis = analysis.find(chamber)->second.configure();
//		if(!isAnalysis) {
//    	_log += " ERROR: Can't configure analysis for chamber: " + std::to_string(chamber) + "!" + _rowDelimiter;
//			return false;
//		}
//  }
//  // ---------------------------------------------------------------------------
//  
//  strips.clear(); trigCh.clear(); TDCs.clear(); chambers.clear(); 
//  HR.clear(); timeOffSetHR.clear(); LR.clear(); timeOffSetLR.clear(); timeOffSet.clear(); inverse.clear(); // clear memory (map params) 
//  // ---------------------------------------------------------------------------
//
//  
//  LyPetiRead stream; 
//  bool isOpen = false;
//  bool isClose = false;
//  
//  // runs params
//  std::cout  << "Cookies are ready, invite you to enjoy the taste!" << std::endl;
//  std::cout << "#-------------------------------------------------------------------------------" << std::endl;
//	DqRoot dataRoot; dataRoot.setOutputFile(Form("%s", rootFilePath.c_str()), "default");
//	for(unsigned int r = 0; r < runs.size(); r++) {
//    //TFile file(Form("%s/RF_%d.root", parser.filePath(_outputs.at(0)).c_str(), r),"RECREATE");
//    std::cout << std::endl << "Run[" << r << "]: " << runs.at(r);
//    
//    // LOOP (Analysis) READ DATA ------------------------------------
//    bool isFindRun = false;
//    std::cout << "\n  Converting loop..." << std::endl;
//    for(unsigned int i = 0; i < _inputs.size(); i++) {
//			
//      if(runs.at(r) == this->numRun(parser.fileName(_inputs.at(i))))
//        isFindRun = true;
//      if(runs.at(r) != this->numRun(parser.fileName(_inputs.at(i)))) {
//        if(i < _inputs.size() - 1)
//          continue;
//        else if(!isFindRun) {
//          _log += " WARNING! RUN: " 
//	  				 + std::to_string(runs.at(r)) + " was skipped! Can't find files!" 
//	  				 + _rowDelimiter;
//          continue;
//        }
//        else
//          continue;
//      }
//
//  	  std::cout << "    File[" << i << "]: " 
//														 << parser.fileName(_inputs.at(i)) << std::endl;
//
//  	  isOpen = stream.open(_inputs.at(i));
//  	  if (!isOpen) {
//  	    _log += " WARNING! file: " 
//						 + parser.fileName(_inputs.at(i))  + " was skipped! Can't open!" 
//						 + _rowDelimiter;
//  	    continue;
//  	  }
//
//      // read raw data
//  	  int codeRead = 1;
//			int size = stream.size();
//  	  int pos = 0, prev = 0;
//  	  std::vector<zdaq::buffer*> buffers; // raw data (buffers)
//      
//      DqProcessBar processBar;
//  	  processBar.setEndIndex(size);
//  	  processBar.setCurrentStep(0);
//  	  while(codeRead > 0) {
//  	    codeRead = stream.read(&pos, &buffers); // read file
//      
//  	    // fill event class with raw data from buffers
//  	    event.clear(); event.fill(&buffers); 
//
//  	    // clear memory form old buffers
//  	    for (int j = 0; j < (int)buffers.size(); j++)
//  	      delete buffers.at(j);
//  	    buffers.clear();
//  	  
//  	    bool isEvent = event.check();
//  	    if(!isEvent) {
//  	      _log += " WARNING! Event in pos: " + std::to_string(prev) + " was skipped! Can't open!" 
//				       + _rowDelimiter;
//  	      continue;
//  	    }
//	     // ----------------------------------------------------------------------
//
//  	    // ANALYSIS BEGIN
//				for(unsigned int ic = 0; ic < event.chambers()->size(); ic++) {
//          int chamber = event.chambers()->at(ic); 
//          Int_t number;
//          Float_t time; 
//          Float_t ch;
//          TTree T(Form("run_%d_chamber_%d", runs.at(r), chamber), Form("run_%d_chamber_%d", runs.at(r), chamber));
//          T.Branch("EventNumber", &number,"EventNumber/I");
//          T.Branch("TDC_channel", &ch,"TDC_channel/F");
//          T.Branch("TDC_TimeStamp", &time,"TDC_TimeStamp/F");
//         // std::cout << "chamber=" << chamber << std::endl;
//          unsigned int numberOfTrigers = event.trigs(chamber)->size();
//          bool isTrigers = analysis.find(chamber)->second.fillTrigers(numberOfTrigers);
//					if(isTrigers) {
//            for(unsigned int js = 0; js < event.strips(chamber)->size(); js++) {
//              if(event.strips(chamber)->at(js).trigs.size() == 1) {
//                number = event.strips(chamber)->at(js).trigs.at(0)->head->number;
//                for(unsigned int jh = 0; jh < event.strips(chamber)->at(js).HR.size(); jh++) {
//                  ch = event.strips(chamber)->at(js).number - chamber;
//                  time = event.strips(chamber)->at(js).HR.at(jh)->time -  event.strips(chamber)->at(js).trigs.at(0)->time;
//                //  std::cout << std::setprecision(3) << "HR:" << ch << "|" << time << std::endl;
//                  T.Fill();
//                }
//         		    for(unsigned int jl = 0; jl < event.strips(chamber)->at(js).LR.size(); jl++) {
//                  ch = event.strips(chamber)->at(js).number + 500 - chamber;
//                  time = event.strips(chamber)->at(js).LR.at(jl)->time -  event.strips(chamber)->at(js).trigs.at(0)->time;
//                //  std::cout << std::setprecision(3) << "LR:" << ch << "|" << time << std::endl; 
//                  T.Fill();
//         		    }
//              }
//            }
//					}
//        dataRoot.writeObject("", &T);
//		    }
//        for(unsigned int ic = 0; ic < event.chambers()->size(); ic++)
//          analysis.find(event.chambers()->at(ic))->second.clear("EVENT");
//        // ANALYSIS END.
//  	  	if(codeRead > 0)
//					processBar.print(pos, 0.02);
//				else
//					processBar.print(size, 0.02);
//  	    prev = pos;
//  	//------  
//    }
//        std::cout << std::endl;
//  	    if(isOpen) {
//  	      isClose = stream.close();
//  	      if(!isClose) {
//  	        _log += " ERROR! file: " + parser.fileName(_inputs.at(i))  
//																	 + " Can't close!" + _rowDelimiter;
//  	    	  return false;
//				  }
//  	    }
//	    }
//      for(unsigned int ic = 0; ic < event.chambers()->size(); ic++)
//        analysis.find(event.chambers()->at(ic))->second.clear("RUN");
//  }
//  std::cout << "End of Processing!" << std::endl;
//  std::cout << "#-------------------------------------------------------------------------------\n" << std::endl;
//	std::cout  << "Comparing cookies and Cleaning the kitchen..." << std::endl;
//  
//	std::cout << "You're welcome!" << std::endl;
//  std::cout << "#---\n" << std::endl;
//  std::string comment = this->findParam("comment", &_params);
//  std::cout << "#COMMENT: " << comment << "_" << logRunNumber <<  std::endl;
//  std::cout << "#OUTPUT FILE: " << rootFilePath <<  std::endl;
//  return true;
//}

bool LyPeti::run() 
{
  bool isRun = false;
  if (_mod == "analysis") isRun = analysis();
  if (_mod == "binToROOT") isRun = binToROOT();
  return isRun;
}
//------------------------------------------------------------------------------
