//------------------------------------------------------------------------------
// Description: main file
// Authors:  Shchablo, Shchablo@gmail.com
//------------------------------------------------------------------------------

// C++ includes
#include <iostream>
#include <string>
#include <vector>

// Dq includes
#include "DqAnalysis.hpp"
#include "DqCard.hpp"
#include "DqConfigure.hpp"

// Mods
#include "MyRoot.hpp"
#include "Wee2UI.hpp"
#include "LyPeti.hpp"

int main(int argc, char* argv[]) {
  std::string types[3];
  types[0] = "type";
  types[1] = "card";
  types[2] = "args";

  // get arguments
  std::vector<std::string> arguments;
  for (int i = 0; i < argc; i++) arguments.push_back(argv[i]);

  // config program
  DqConfigure configure(arguments, types[0], "=", "--");
  configure.setTypeSymbol(types[0]);
  std::string type = configure.type();
  std::map<std::string, std::string> args = configure.getArgs();

  // card
  if (type == types[1]) {
    std::string cardDelimiter = ",";
    std::string cardRowDelimiter = "\r\n";

    DqCard card(types[1], cardDelimiter, cardRowDelimiter);

    std::vector<std::string> systems;
    systems.push_back("DqROOT:");
    systems.push_back("Wee2UI:");
    systems.push_back("LyPeti:");
    card.setSystems(systems);

    // card param setting
    bool isSetting = false;
    isSetting = card.setting(args);
    if (!isSetting) {
      std::cout << "#ERROR. Can't config card file." << std::endl;
      std::cout << "The End." << std::endl;
      return 0;
    }
    std::cout << card.getStringInfo() << std::endl;

    std::vector<std::string> mods = card.getMods();
    std::map<std::string, std::string> params = card.getParams();
    std::vector<std::string> inputs = card.getInput();
    std::vector<std::string> outputs = card.getOutput();
    std::vector<std::vector<std::string>> sysMods = card.getSysMods();

    DqAnalysis* analysis[mods.size()];
    std::string logRunNumber = configure.getNumRun();
    params.insert(std::pair<std::string, std::string>("logNumber", logRunNumber));
    unsigned int n = 0;
    for(unsigned int i = 0; i < systems.size(); i++) {
      for(unsigned int j = 0; j < sysMods.at(i).size(); j++) {
        if(n < mods.size() && mods.at(n) == sysMods.at(i).at(j) &&
            systems.at(i) == "Wee2UI:") {
          std::cout << "#MOD: " << sysMods.at(i).at(j) << std::endl;
          analysis[n] = new Wee2UI(sysMods.at(i).at(j), params, inputs, outputs);
          n += 1;
        }
        if(n < mods.size() && mods.at(n) == sysMods.at(i).at(j) &&
            systems.at(i) == "DqROOT:") {
          std::cout << "#MOD: " << sysMods.at(i).at(j) << std::endl;
          analysis[n] = new MyRoot(sysMods.at(i).at(j), params, inputs, outputs);
          n += 1;
        }
        if(n < mods.size() && mods.at(n) == sysMods.at(i).at(j) &&
            systems.at(i) == "LyPeti:") {
          std::cout << "#MOD: " << sysMods.at(i).at(j) << std::endl;
          analysis[n] = new LyPeti(sysMods.at(i).at(j), params, inputs, outputs);
          n += 1;
        }
      }
    }
    bool isRuns[mods.size()];
    for(unsigned int i = 0; i < n; i++) {
      isRuns[i] = false;
      isRuns[i] = analysis[i]->setRowDelimiter(cardRowDelimiter);
      isRuns[i] = analysis[i]->run();
      if (!isRuns[i]) {
        std::cout << "#ERROR: Can't run: " << mods.at(i) << "!" << std::endl;
        std::cout << analysis[i]->getLog();
      } else {

       // for(unsigned int j = 0; j < outputs.size(); j++)
       //   std::cout << "\n#OUTPUT FILE[" << j << "]: " << outputs.at(j) << "\n" << std::endl;
        std::vector<std::string> logs;
        logs.push_back(analysis[i]->getLog());
			  bool isDropLog = analysis[i]->dropLog(&params);
			  if(isDropLog) { 
			  	std::cout << configure.log(logs, card.getCardLineDelimiter());
        }
        logs.clear();
        logs.push_back(card.getLog());
        logs.push_back(analysis[i]->getLog());
        std::string logData = configure.log(logs, card.getCardLineDelimiter());
        DqParser parser;
        std::string logFilePath = parser.filePath(card.getCardPath());
        std::string logFileName = parser.fileName(card.getCardPath());
        std::string logFileFormat = parser.deleteFormat(&logFileName);
        std::string pathLogFile = logFilePath + "logs/" + logFileName + "_" +
                                  logRunNumber + logFileFormat;
        std::cout << "#LOG FILE: " << pathLogFile << std::endl;
        bool isLogFile = configure.logToFile(logData, pathLogFile);
        if (!isLogFile) {
          std::cout << "#ERROR: Can write log file!" << std::endl;
          std::cout << logData;
        }
      }
      delete analysis[i];
    }
  }

  // args
  else if (type == type[2]) {
    std::cout << "Args Run!" << std::endl;
  }
  else
    std::cout << "Please enter type of run. See README." << std::endl;
  
  std::cout << "#-------------------------------------------------------------------------------" << std::endl;
  std::cout << "THE END." << std::endl;
  return 1;
}
