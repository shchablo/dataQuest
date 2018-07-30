#include "DqRoot.hpp"

/* Constructor, destructor. */ 
//------------------------------------------------------------------------------
DqRoot::DqRoot()
{
    isOutFile_ = false;
}

DqRoot::~DqRoot()
{
  if(isOutFile_) {
    outTree_->Delete();
    outFile_->Close();
    outFile_->Delete();
  }
}
//------------------------------------------------------------------------------

/* Set functions. */ 
//------------------------------------------------------------------------------
/*! \fn setOutputFile
* \brief Function for set output file.
*
* \param  char* outputFileName - path to the file and name of file
* \param  char* outputTreeName - name of tree
*
* \return bool - check tag
*/
bool DqRoot::setOutputFile(std::string outputFileName, std::string outputTreeName)
{
  outFile_ = new TFile(outputFileName.c_str(), "UPDATE");
  if(!outFile_)
    return false;
  outTree_ = new TTree(outputTreeName.c_str(), outputTreeName.c_str());
  if(!outTree_)
    return false;
  isOutFile_ = true;
  return true;
}

/*! \fn setBranch
* \brief Function for ser output branch for tree.
*
* \param  double *data - pointer of variable of data
* \param  string branchName - neame of branch
* \return bool - check tag
*/
bool DqRoot::setBranch(double *data, std::string branchName)
{
  if(!isOutFile_)
    return false;
  outTree_->Branch(branchName.c_str(), &data, branchName.c_str());
  return true;
}
//------------------------------------------------------------------------------

/* Functions */ 
//------------------------------------------------------------------------------
bool DqRoot::writeObject(std::string dirName, TObject *object)
{
  if(!outFile_->GetDirectory(dirName.c_str())) {
    outFile_->mkdir(dirName.c_str());
    outFile_->cd(dirName.c_str());
  }
  else
    outFile_->cd(dirName.c_str());

 // if(outFile_->FindObject(object->GetName()))
 //   return false;

  object->Write();
  return true;
}

bool DqRoot::writeTH1toCanvasCMS(std::string path, TH1* object, bool isCanvas)
{
  TCanvas *c1 = new TCanvas(Form("c_%s", object->GetName()), Form("%s", object->GetName()));
	object->GetXaxis()->SetTitleOffset(0.7);
	object->GetXaxis()->SetTitleSize(0.05);
	object->GetYaxis()->SetTitleOffset(0.7);
	object->GetYaxis()->SetTitleSize(0.05);
  object->SetFillColor(4);
  object->Draw();
  TString tsPath(path.c_str());
  const char* cDirName = tsPath.Data();
  if(isCanvas) {
	  c1->SetCanvasSize(900, 600);
	  c1->SetWindowSize(900, 600);
	  c1->SetGrid();
    TString c1Path((path + "/CMS/").c_str());
    const char* c1DirName = c1Path.Data();
	  this->writeObject(c1DirName, c1);
  }
	this->writeObject(cDirName, object);
	delete c1;
	return true;
}
bool DqRoot::writeDqTGEtoCanvasCMS(std::string path, DqTGE* object, bool isCanvas)
{
	TCanvas *c1 = new TCanvas(Form("c_%s", object->GetName()), Form("%s", object->GetName()));
	object->GetXaxis()->SetTitleOffset(0.7);
	object->GetXaxis()->SetTitleSize(0.05);
	object->GetXaxis()->SetTitle(object->getTitleX().c_str());
	object->GetYaxis()->SetTitleOffset(0.7);
	object->GetYaxis()->SetTitleSize(0.05);
	object->GetYaxis()->SetTitle(object->getTitleY().c_str());
  object->SetMarkerStyle(20);
  object->Draw();
  TString tsPath(path.c_str());
  const char* cDirName = tsPath.Data();
  if(isCanvas) {
	  c1->SetCanvasSize(700, 600);
	  c1->SetWindowSize(700, 600);
	  c1->SetGrid();
    TString c1Path((path + "/CMS/").c_str());
    const char* c1DirName = c1Path.Data();
	  this->writeObject(c1DirName, c1);
  }
	this->writeObject(cDirName, object);
  delete c1;
	return true;
}
bool DqRoot::writeTH2toCanvasCMS(std::string path, TH2* object, bool isCanvas)
{
	TCanvas *c1 = new TCanvas("c_", "");
	object->GetXaxis()->SetTitleOffset(0.7);
	object->GetXaxis()->SetTitleSize(0.05);
	object->GetYaxis()->SetTitleOffset(0.7);
	object->GetYaxis()->SetTitleSize(0.05);
	object->GetZaxis()->SetTitleOffset(0.7);
	object->GetZaxis()->SetTitleSize(0.05);
	object->Draw();
  TString tsPath(path.c_str());
  const char* cDirName = tsPath.Data();
  if(isCanvas) {
	  c1->SetCanvasSize(700, 600);
	  c1->SetWindowSize(700, 600);
	  c1->SetGrid();
    TString c1Path((path + "/CMS/").c_str());
    const char* c1DirName = c1Path.Data();
	  this->writeObject(c1DirName, c1);
  }
	this->writeObject(cDirName, object);
  delete c1;
	return true;
}
bool DqRoot::writeTMGtoCanvasCMS(std::string path, std::vector<TH1>* objects, bool isCanvas)
{
  
	TCanvas *c1 = new TCanvas("c_", "");
  TMultiGraph *mg = new TMultiGraph();
  for(unsigned int i = 0; i < objects->size(); i++) {
	  objects->at(i).GetXaxis()->SetTitleOffset(0.7);
	  objects->at(i).GetXaxis()->SetTitleSize(0.05);
	  objects->at(i).GetYaxis()->SetTitleOffset(0.7);
	  objects->at(i).GetYaxis()->SetTitleSize(0.05);
	  objects->at(i).Draw(); 
  } 
  mg->Draw();
  TString tsPath(path.c_str());
  const char* cDirName = tsPath.Data();
  if(isCanvas) {
	  c1->SetCanvasSize(700, 600);
	  c1->SetWindowSize(700, 600);
	  c1->SetGrid();
    TString c1Path((path + "/CMS/").c_str());
    const char* c1DirName = c1Path.Data();
	  this->writeObject(c1DirName, c1);
  }
	this->writeObject(cDirName, mg);
  delete mg;
	delete c1;
	return true;
}

std::map<std::string, DqTH2D>::iterator DqRoot::addDqTH2D(std::string name, std::map<std::string, DqTH2D>* dqTH2Ds)
{
    auto it = dqTH2Ds->find(name.c_str());
    if(it == dqTH2Ds->end()) {
      int sizeX = 1000; double begX = 1; double endX = -1;
      int sizeY = 1000; double begY = 1; double endY = -1;
      dqTH2Ds->insert(std::make_pair(name, 
              DqTH2D{Form("h_%s", name.c_str()), Form("%s", name.c_str()), sizeX, begX, endX, sizeY, begY, endY}));
      it = dqTH2Ds->find(name.c_str());
    }
    return it;
}
std::map<std::string, DqTH1D>::iterator DqRoot::addDqTH1D(std::string name, std::map<std::string, DqTH1D>* dqTH1Ds)
{
    auto it = dqTH1Ds->find(name.c_str());
    if(it == dqTH1Ds->end()) {
      int size = 1000; double beg = 1; double end = -1;
      dqTH1Ds->insert(std::make_pair(name, 
              DqTH1D{Form("h_%s", name.c_str()), Form("%s", name.c_str()), size, beg, end}));
      it = dqTH1Ds->find(name.c_str());
    }
    return it;
}
std::map<std::string, DqTGE*>::iterator DqRoot::addDqTGE(std::string name, std::map<std::string, DqTGE*>* dqTGEs)
{
    auto it = dqTGEs->find(name.c_str());
    if(it == dqTGEs->end()) {
      dqTGEs->insert(std::make_pair(name, new DqTGE{Form("gr_%s", name.c_str()), Form("%s", name.c_str())}));
      it = dqTGEs->find(name.c_str());
    }
    return it;
}

//-----
//------------------------------------------------------------------------------
