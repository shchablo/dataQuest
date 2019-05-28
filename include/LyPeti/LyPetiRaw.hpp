//------------------------------------------------------------------------------
/* INFORMATION: */
//------------------------------------------------------------------------------
#ifndef LYPETIRAW_h
#define LYPETIRAW_h

#include <DqRoot.hpp>

namespace lyRaw {
struct head
{
	Float_t run; Float_t source;
	Float_t HV_top; Float_t HVapp_top; Float_t HVeff_top;
	Float_t HV_bot; Float_t HVapp_bot; Float_t HVeff_bot;
};      
struct data
{
};
}

class LyPetiRaw
{

  public:

  /* Constructor, destructor. */ 
  //----------------------------------------------------------------------------
  LyPetiRaw();
  virtual ~LyPetiRaw();
  //----------------------------------------------------------------------------
  lyRaw::data* data();

  protected:
	
  lyRaw::data _head;  // runInformation
  lyRaw::data _data;  // rawdata
};

#endif
//------------------------------------------------------------------------------
