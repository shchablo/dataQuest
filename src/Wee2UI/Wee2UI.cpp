#include "Wee2UI.hpp"

/* Constructor, destructor. */ 
//------------------------------------------------------------------------------
Wee2UI::Wee2UI(std::string mod,
                           std::map<std::string, std::string> params,
                           std::vector<std::string> inputs,
                           std::vector<std::string> outputs)
{
  _mod = mod;
  _params = params;
  _inputs = inputs;
  _outputs = outputs;
}
Wee2UI::~Wee2UI()
{
}
bool Wee2UI::plot_SCscan() 
{
    std::vector<int> ch;
    for(unsigned int i = 0; i < _inputs.size(); i++) {
        
     // create stream and open file
     DqCSV inCSV(_inputs.at(i), " ", "\n");
     bool isOpenInCSV = inCSV.open(std::fstream::in);
     if(!isOpenInCSV) {
       std::cout << "Can't open file: " << _inputs.at(i) << std::endl;
       continue;
     }
     for(unsigned int j = 0; j < ch.size(); j++) {
     }
     // close file
     if(isOpenInCSV) {
       bool isCloseInCSV = false; 
       isCloseInCSV = inCSV.close();  
       if(!isCloseInCSV) return 0;
     }
    }

  return true;
}

bool Wee2UI::run() 
{
  bool isRun = false;
  if(_mod == "plot_SCscan")
    isRun = plot_SCscan();
   return isRun;
}
//------------------------------------------------------------------------------
