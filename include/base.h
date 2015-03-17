#ifndef _BASE_ASDF_H
#define _BASE_ASDF_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "hdhC.h"

#include "geo_meta.h"
#include "nc_api.h"

#include "fd_interface.h"
#include "annotation.h"
#include "time_control.h"
#include "variable.h"

/*! \file base.h
 \brief Base of derived classes InFile, CellStatistics, Oper, OutFile.
*/

class InFile;
class QC ;

//! Basic structs and methods of derived classes

/*! Derived classes: InFile, CellStatistics, Oper, OutFile.
Management of netCDF properties beyond the application interface to netCDF files (Nc_API).
Reading netCDF data and storing these in class GeoMeta objects.\n
Note: many members are not relevant for the Quality Control.*/

class Base : public IObj
{
  public:

  //  Helper for writing error/warning messages.
  //! Deprecated error/warning procedure.
  /*! Each derived class streams warnings and error messages,
      respectively, to a file of its own. Obsolete by the Annotation procedure.*/
  struct ExceptionStruct
  {
     std::ofstream *ofsError;
     std::ofstream *ofsWarning;

     std::string strError;
     std::string strWarning;
  } ;

  //! Default contructor
  Base();

  //! Copy constructor
  Base(const Base&);

  //! Assignment operator
  Base& operator=( const Base&);

//  virtual void setTablePath(std::string p) = 0;

  //! Setting of start-time options.
  /*! Start-time options are put to string 'optStr' which is evaluated.*/
  void applyOptions(void);

  void clearVariable(Variable &);
  void copy(const Base &b);

  //! Error messages are written to a file.
  bool exceptionHandling(std::string key,
            std::string capt, std::string text,
            std::vector<std::string> &checkType,
            std::string vName="") ;
  void exceptionError(std::string );
  void exceptionWarning(std::string );

  //! Final actions depending on the derived class.
  /*! If errCode>0 is provided, the program shall exit with this number.*/
  void finally(int errCode=0, std::string s="");

  //! Return the time value of record 'rec' out of the nc-file.
  static double
       getTime(NcAPI &, size_t rec, std::string, double offset=0.);

  //! Put all the time values of record 'rec' to the Matval obj.
  /*! This method gives access to time bound values.*/
  static bool
       getTime(NcAPI &, size_t rec, std::string, MtrxArr<double>&, double offset=0.);

  //!  Get the name of the variable indicated by 'varname=...', 'vname=...',  'variable=...', or 'v=...'
  std::vector<std::string>
       getVarname( std::string &s);
  std::vector<std::string>
       getVarname( std::string &s, std::vector<std::string> &alias);
  //! Description of options.
  static void
       help(void);
  void initDefaults(void);

  bool isVarname( std::string &);
  void makeVariable (NcAPI *, std::string name, int id=-1);

  void setFilename(std::string s){filename=s;}
  void setFilePath(std::string p){filePath=p;}

  //! Connect a frequency distribution object.
  void setFreqDistI(FD_interface *p){ fDI=p; return;}

//  void setGDwithSrcProps(std::string vName);
  //! Connect a QC object.
  void setQualityControl(QC *p){ qC=p; return;}
//  void setRec(size_t rec)
//         {currRec=rec;}
  void setSrcStr(std::string s)
         {srcStr.push_back(s); return;}
  void setSrcVariable(Variable &var, Base *p);

  void setTimeControl(TimeControl *p){ tC=p; return;}
//  void setVarnameAndGD(std::string s);
  void setVarProps(void);
  void setVarPropsForOperation( void );
  void setVarPropsNoOperation( void );

  std::string filename;
  std::string filePath;
  std::string linkObjName;
  bool        isAllocate;
  std::vector<std::string> srcStr;
  std::vector<std::string> operandStr;
  std::map<std::string, Variable *> opMap;
  std::map<std::string, size_t> varNameMap;

  //see rules for varName and srcName in attach()
  std::vector<Variable> variable;

  std::vector<Variable> srcVariable;
  std::vector<std::string> obsolete_srcVarName;
  std::vector<std::string> varName;
  std::map<std::string, int> mapIndex;
  std::string unlimitedDimName;  // name of time dimension
  std::string timeName;          // name of time variable

  bool    isArithmeticMean;
  bool    isBackRotation;
  bool    isMissValSet;
  double  explicitFillingValue;

//  size_t currRec;
  MtrxArr<double> tmp_mv;

  //! Obsolete
  std::string regionFile;
  std::vector<std::string> regioStr;

  // index mapped to varName for the next vectors
  std::vector<double>  fillingValue;

  // Receives an id for each specified region; may be multiple
  // for OutFile objects.
  std::vector<int> regionID;

  //point to those source-gM(s) that will be used
  std::vector<GeoMetaBase*> pSrcGD;

  // point to the source base
  std::vector<Base*> pSrcBase;
  NcAPI        *pNc;  // only set in InFile objects

  // messaging in case of exceptions.
  struct ExceptionStruct xcptn;
};

#endif