#ifndef _QA_H
#define _QA_H

#include "hdhC.h"
#include "date.h"
#include "annotation.h"
#include "qa_data.h"
#include "qa_time.h"
#include "qa_PT.h"

//! Quality Control Program Unit for CORDEX.
/*! All the QA considerations are covered by this class.\n
The QA_CORDEX.cpp and qa_CORDEX.h files have to be linked to
QA.cpp and qa.h, respectively.\n
Requested properties from the cordex_archive_specifications.pdf
from http://cordex.dmi.dk/joomla/images/CORDEX are checked as well
as specifications in CORDEX_variables_requirement_table.pdf.\n
The netCDF data-file is linked by a pointer to the InFile class
instance. Results of the QA are written to a netCDF file
(to the directory where the main program was started) with filename
qa_<data-filename>.nc. Outlier test and such for replicated records
are performed. Annotations are supplied via the Annotation class
linked by a pointer.
*/

//! Struct containing dimensional properties to cross-check with table information.
struct DimensionMetaData
{
  // first item is set by 'time'
  std::string  cmor_name;
  std::string  outname;
  std::string  stndname;
  std::string  longname;
  std::string  type;
  std::string  units;
  bool         isUnitsDefined;
  std::string  index_axis;
  std::string  axis;
  std::string  coordsAtt;
  std::string  bounds;
  uint32_t     checksum;  //fletcher32
  size_t       size;
};

//! Properties of the variable to cross-check with table information.
class VariableMetaData
{
  public:
  VariableMetaData(QA*, Variable *var=0);
  ~VariableMetaData();

  // only used for reading tables
  std::string cellMethods;
  std::string cellMethodsOpt;
  std::string standardName;
  std::string longName;
  std::string positive;
  std::string units;

  bool        isUnitsDefined;
  bool        isForkedAnnotation;

  std::string name;

  //! store results temporarily
  DataOutputBuffer dataOutputBuffer;

  Annotation*  notes;
  Variable*    var;
  QA*          pQA;
  QA_Data      qaData;

  //! Verify units % or 1 by data range
  void verifyPercent(void);

  int  finally(int errCode=0);
  void forkAnnotation(Annotation *p);
  void setAnnotation(Annotation *p);
  void setParent(QA *p){pQA=p;}
};

class QA : public IObj
{
  public:

  //! Default constructor.
  QA();
  ~QA();

  //! coresponding to virtual methods in IObj

  //! Check field properties of the variable.
  /*! Designed for multiple usage for sub-layers of a data block.
      Each multiple-set must be invoked by calling method 'clearStatistics'.*/
  bool   entry(void);

  //! Initialisation of the QA object.
  /*! Open the qa-result.nc file, when available or create
   it from scratch. Meta data checks are performed.
   Initialise time testing, time boundary testing, and cycles
   within a time step. At the end  entry() is called to test
   the data of fields.*/
  bool   init(void) ;
  void   linkObject(IObj *);

  // special: from InFile with path and period stripped.
  void   setFilename(hdhC::FileSplit&);
  void   setTablePath(std::string p){tablePath=p;}

  void   applyOptions(bool isPost=false);

  //! Comparison of dimensions between file and standard table
  /*! Cross-checks with the standard table are performed only once for
   each variable at first time encounter in the CORDEX Project ensuring
   conformance.*/

  bool   checkDataBody(std::string vName="");

  void   checkDimStandardTable(ReadLine &tbl, InFile &in,
            VariableMetaData &var,
            std::vector<struct DimensionMetaData>&,
            std::map<std::string, size_t> &col,
            std::string dName, size_t colMax);

  //! Check dimensions
  void   checkDimTableEntry(InFile &in,
            VariableMetaData &var,
            struct DimensionMetaData &nc,
            struct DimensionMetaData &tbl) ;

  void   checkDimAxis(InFile &in,
           VariableMetaData &vMD,
           struct DimensionMetaData &nc_entry,
           struct DimensionMetaData &tbl_entry);

  void   checkDimChecksum(InFile &in,
           VariableMetaData &vMD,
           struct DimensionMetaData &nc_entry,
           struct DimensionMetaData &tbl_entry);

  void   checkDimLongName(InFile &in,
           VariableMetaData &vMD,
           struct DimensionMetaData &nc_entry,
           struct DimensionMetaData &tbl_entry);

  void   checkDimOutName(InFile &in,
           VariableMetaData &vMD,
           struct DimensionMetaData &nc_entry,
           struct DimensionMetaData &tbl_entry);

  void   checkDimSize(InFile &in,
           VariableMetaData &vMD,
           struct DimensionMetaData &nc_entry,
           struct DimensionMetaData &tbl_entry);

  void   checkDimStndName(InFile &in,
           VariableMetaData &vMD,
           struct DimensionMetaData &nc_entry,
           struct DimensionMetaData &tbl_entry);

  //! Check the (transient) time dimension.
  void   checkDimULD(
            VariableMetaData &var,
            struct DimensionMetaData &nc,
            struct DimensionMetaData &tbl);

  //! Check the units of a dimension's variable representation.
  void   checkDimUnits(InFile &in,
            VariableMetaData &var,
            struct DimensionMetaData &nc,
            struct DimensionMetaData &tbl);

  void   checkDRS(InFile &);
  void   checkDRS_ModelName(InFile &, std::string &aN, std::string &aV,
            char model, std::string iN="", std::string iV="");

  //! Match filename components and global attributes of the file.
  void   checkFilename(InFile &) ;

  //! Test optional global attribute
  void   checkDrivingExperiment(InFile &);

  //! Check near-surface height value between 0 - 10m
  void   checkHeightValue(InFile &);

//! Checks meta-data
  void   checkMetaData(InFile &) ;

  //! Is it NetCDF-4, is it compressed?
  void   checkNetCDF(InFile &);

  //! Consistency between filename, plev variable and data value
  /*! For variables defined for a specific pressure level, eg.ta850*/
  void   checkPressureCoord(InFile&);

  void   checkProjectTable(InFile &in);
  
  void   checkVarTableEntry(
             VariableMetaData &,
             VariableMetaData &tbl_entry);

  //! Consistency test between file and table: cell_methods.
  void   checkVarTableEntry_cell_methods(
             VariableMetaData &,
             VariableMetaData &tbl_entry);

  //! Consistency test between file and table: long name.
  void   checkVarTableEntry_longName(
             VariableMetaData &,
             VariableMetaData &tbl_entry);

  //! Consistency test between file and table: (output) name.
  void   checkVarTableEntry_name(
             VariableMetaData &,
             VariableMetaData &tbl_entry);

  //! Consistency test between file and table: standard name.
  void   checkVarTableEntry_standardName(
             VariableMetaData &,
             VariableMetaData &tbl_entry);

  //! Consistency test between file and table: units.
  /*! If one of table or CF standard units matches, then the check will pass.*/
  void   checkVarTableEntry_units(
             VariableMetaData &,
             VariableMetaData &tbl_entry);

  /*! Close records for time and data.*/
  void   closeEntry(void);

  //! Make VarMetaData objects.
  void   createVarMetaData(void);

  //! Check time properties.
  void   domainCheck(ReadLine&);
  void   domainCheckData(std::string &var_lon, std::string &var_lat,
            std::vector<std::string>&, std::string tbl_id);
  void   domainCheckDims(std::string item, std::string &t_Nlon,
            std::string &f_Nlon, std::string tble_id);
  void   domainCheckPole(std::string item,
            std::string &t_Nlon, std::string &f_Nlon);
  void   domainFindTableType( std::vector<std::vector<std::string> > &tbl1,
            std::vector<std::vector<std::string> > &tbl2,
            int &ix1, int &ix2 );
  bool   domainFindTableTypeByRange(
            std::vector<std::vector<std::string> > &T1,
            std::vector<std::vector<std::string> > &T2,
           int &table_id, size_t &row );

  //! The final operations.
  /*! An exit code is returned.*/
  int    finally(int errCode=0);

  //! The final qa data operations.
  /*! Called from finall(). An exit code is returned.*/
  int    finally_data(int errCode=0);

  //! Find entry of a requested variable in the standard table.
  bool   findTableEntry(ReadLine &, std::string &,
            VariableMetaData &tble_entry);

  bool   findTableEntry(ReadLine &, std::string &name,
            size_t col_outName, std::string &str0);

  //! Find the name of requested sub-table in string str0.
//  bool   find_CORDEX_SubTable(ReadLine &, std::string &str0,
//            VariableMetaData &var);

  std::string
         getAttValue(size_t v_ix, size_t a_ix);

  std::string
         getCurrentTable(void){ return currTable ; }

  //! Store properties of a dimension in the struct.
  /*! Note: the name of the dimension is passed by the struct.*/
  bool   getDimMetaData(InFile &in,
             VariableMetaData &,
             struct DimensionMetaData &, std::string &);

  int    getExitCode(void){return exitCode;}

  //! Get path componenents.
  /*! mode: "total": filename with total path, "file": filename,
      "base": filename without extension, "ext": extension without '.',
      "path": the path component without trailing '/'.*/
  std::string
         getPath(std::string& f, std::string mode="total");

  std::string
         getFrequency(void);

//  std::string
//         getStandardTable(void){ return varReqTable.file ; }

  //! Get the Standard table name from the global attributes
  void   getSubTable(void);

//  std::string
//         getTablePath(void){ return tablePath; }

  //! Brief description of options
  static void
         help(void);

  //! Initialisation of flushing gathered results to netCDF file.
  /*! Parameter indicates the number of variables. */
  void   initDataOutputBuffer(void);

  //! Set default values.
  void   initDefaults(void);

  //! Global attributes of the qa-netCDF file.
  /*! Partly reflecting global attributes from the sources. */
  void   initGlobalAtts(InFile &);

  //! Initialisiation of a resumed session.
  /*! Happens for non-atomic data sets and those that are yet incomplete. */
  void   initResumeSession(void);

  //! Check the path to the tables;
  void   inqTables(void);

  bool   isProgress(void){ return ! qaTime.isNoProgress ; }

  //! Get coordinates of grid-cells where an error occurred
  /*! Does not work for tripolar coordinates */
//  bool   locate( GeoMetaT<float>*, double *lat, double *lon, const char* );

  //! Open a qa_result file for creation or appending data.
  /*! CopY time variable from input-nc file.
   Collect some properties of the in-netcdf-file in
   struct varMeDa. Also check properties against tables.
  */
  void   openQA_Nc(InFile&);

  //! Perform only post-processing
  bool   postProc(void);
  bool   postProc_outlierTest(void);

  //! Read the headlines for dimensions and variable.
  /*! Read from the standard table; used to identify columns.*/
  bool   readTableCaptions(ReadLine &, std::string freq,
            std::map<std::string, size_t> &col, std::string &str0);

  void   requiredAttributes_check(InFile &);
  void   requiredAttributes_checkCloudVariableValues(InFile &,
             std::string &aux, std::string &vals) ;
  void   requiredAttributes_checkGlobal(InFile &,
             std::vector<std::string> &atts);
  void   requiredAttributes_checkVariable(InFile &,
             Variable &, std::vector<std::string> &atts);
  void   requiredAttributes_readFile(
           std::vector<std::string> &,
           std::vector<std::vector<std::string> > &);

  void   resumeSession(void);

  //! Connect this with the object to be checked
  void   setInFilePointer(InFile *p) { pIn = p; }

  //! Unused.
  /*! Needed to be conform to a specific Base class functionality */
  void   setSrcStr(std::string s)
             {srcStr.push_back(s); return;}

  //! Cross-check with standard table.
  /*! Prepare the check for dimensions and variable.*/
  void   varReqTableCheck(InFile &in,
            VariableMetaData &var,
            std::vector<struct DimensionMetaData>& );

  //! Check dimensional bounds: layout and size
  /*! Number of values and checksum of the bounds*/
/*
  void   varReqTableCheckDimBounds(InFile &in, Split &splt_line,
            VariableMetaData &var,
            struct DimensionMetaData &dimFE,
            struct DimensionMetaData &dimTE,
            std::map<std::string, size_t> &col) ;
*/
  //! Check dimensional values: layout and size
  /*! Number of values and checksum*/
/*
  void   varReqTableCheckDimValues(InFile &in, Split &splt_line,
            VariableMetaData &var,
            struct DimensionMetaData &file,
            struct DimensionMetaData &table,
            std::map<std::string, size_t> &col) ;
*/

  //! Store results in the internal buffer
  /*! The buffer is flushed to file every 'flushCountMax' time steps.*/
  void   store(std::vector<hdhC::FieldData> &fA);
  void   storeData(VariableMetaData&, hdhC::FieldData& );
  void   storeTime(void);

  //! Test the time-period of the input file.
  /*! If the end-date in the filename and the last time value
      match within the uncertainty of 0.75% of the time-step, then
      the file is assumed to be completely qa-processed.
      Syntax of date ranges as given in CORDEX  DRS Syntax.*/
  bool   testPeriod(void);
  void   testPeriodCut(std::vector<std::string> &sd) ;
  bool   testPeriodFormat(std::vector<std::string> &sd) ;

  //! Name of the netCDF file with results of the quality control
  std::string tablePath;
  struct hdhC::FileSplit qaFile;
  struct hdhC::FileSplit archiveDesignTable;
  struct hdhC::FileSplit GCM_ModelnameTable;
  struct hdhC::FileSplit projectTableFile;
  struct hdhC::FileSplit RCM_ModelnameTable;
  struct hdhC::FileSplit varReqTable;

  std::string qaNcfileFlags;

  int exitCode;
  bool isExit;

  std::vector<VariableMetaData> varMeDa;

  NcAPI *nc;
  QA_Time qaTime;

  int thisId;

  size_t currQARec;
  size_t importedRecFromPrevQA; // initial num of recs in the write-to-nc-file
  MtrxArr<double> tmp_mv;

  // the same buf-size for all buffer is required for testing replicated records
  size_t bufferSize;

  // init for test about times
  bool enablePostProc;
  bool enableVersionInHistory;
  bool isUseStrict;
  bool isCaseInsensitiveVarName;
  bool isCheckParentExpID;
  bool isCheckParentExpRIP;
  bool isClearBits;
  bool isFileComplete;
  bool isFirstFile;
  bool isNotFirstRecord;
  bool isPrintTimeBoundDates;
  bool isResumeSession;
  bool isRotated;

  size_t nextRecords;

  bool isCheckMeta;
  bool isCheckTime;
  bool isCheckData;

  std::vector<std::string> excludedAttribute;
  std::vector<std::string> overruleAllFlagsOption;

  std::vector<std::string> constValueOption;
  std::vector<std::string> fillValueOption;
  std::vector<std::string> outlierOpts;
  std::vector<std::string> replicationOpts;
  std::vector<std::string> requiredAttributesOption;

  std::string cfStndNames;
  std::string currTable;

  std::string frequency;
  std::string parentExpID;
  std::string parentExpRIP;
  std::string experiment_id;
  std::string subTable ;

  int identNum;
  std::string fVarname;
  char        fileSequenceState;
  std::string prevVersionFile;
  std::vector<std::string> srcStr;
  std::string revision;

  std::string fail;
  std::string fileStr;
  std::string notAvailable;
  std::string blank;
  std::string no_blank;
  std::string s_colon;
  std::string s_empty;
  std::string s_mismatch;
  std::string s_upper;
  std::string s_lower;

  std::string n_axis;
  std::string n_cell_methods;
  std::string n_long_name;
  std::string n_outputVarName;
  std::string n_positive;
  std::string n_standard_name;
  std::string n_units;

  std::string getCaptIntroDim(VariableMetaData &vMD,
                   struct DimensionMetaData &nc_entry,
                   struct DimensionMetaData &tbl_entry, std::string att="");
  void        appendToHistory();
  bool        getExit(void);
  std::string getSubjectsIntroDim(VariableMetaData &vMD,
                   struct DimensionMetaData &nc_entry,
                   struct DimensionMetaData &tbl_entry, bool isColon=true);
  std::string getVarnameFromFilename(std::string str);
  bool        not_equal(double x1, double x2, double epsilon);
  void        pushBackVarMeDa(Variable*);
  void        setExit(int);
  void        setCheckMode(std::string);
  void        setTable(std::string, std::string acronym="");
};

#endif
