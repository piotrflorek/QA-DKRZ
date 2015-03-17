#ifndef _QC_H
#define _QC_H

#include "hdhC.h"
#include "date.h"
#include "annotation.h"
#include "qc_data.h"
#include "qc_time.h"
#include "qc_PT.h"

//! Quality Control Program Unit for CMIP5.
/*! All the QC considerations are covered by this class.\n
The QC_CMIP5.cpp and qc_CMIP5.h files have to be linked to
QC.cpp and qc.h, respectively.\n
Properties specified in standard_output.xlsx document
from http://cmip-pcmdi.llnl.gov/cmip5/docs
are checked as well as the Data Reference Syntax given in cmip5_data_reference_syntax.pdf.\n
The netCDF data-file is linked by a pointer to the InFile class
instance. Results of the QC are written to a netCDF file
(to the directory where the main program was started) with filename qc_<data-filename>.nc. Outlier test and such for replicated records
are performed. Annotations are supplied via the Annotation class
linked by a pointer.
*/


//! Main program unit of the Quality Control.
/*! All the QC considerations are covered by this class. QC of CMIP5
netCDF dimensions, variables and data are checked.
Atomic datasets, chunks and even chunks in the process of 'under-construction'
are taken into account by multi-session features. A resulting
netCDF file is written
(to the directory where the program was started) prefixing the data-filename by 'qc_'.
Zero-D to 3D data are checked, the latter layer-wise in a way that
there is a method 'entry' for the data and an eventual method 'closeEntry', which stores time series results in a file
qc_<varname>.nc .
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
  std::string  bnds_name;
  uint32_t     checksum;  //fletcher32
  size_t       size;
  std::string  value;
  std::string  requested ;
};

//! Properties of the variable to cross-check with table information.
class VariableMetaData
{
  public:
  VariableMetaData(QC*, Variable *v=0);
  ~VariableMetaData();

  std::vector<size_t>  dimVarRep;

  std::string name;
  std::string name_alt;  // Table column: output variable name
  std::string stdTable;
  std::string stdTableAlt;  // special case, cf. Omon, cf3hr, and cfSites
  std::string stdSubTable;
  std::string standardName;
  std::string longName;
  std::string units;

  bool        isUnitsDefined;
  bool        isForkedAnnotation;

  std::string cellMethods;
  std::string cellMeasures;
  std::string type;
  std::string dims;
  double validMin;
  double validMax;
  size_t priority;
  std::string variableName;
  std::string time_units;
  std::string unlimitedDim;

  //! store results temporarily
  DataOutputBuffer dataOutputBuffer;

  Annotation     *notes;
  Variable *var;
  QC             *pQC ;
  QC_Data         qcData;

  int  finally(int errCode=0);
  void forkAnnotation(Annotation *p);
  void setAnnotation(Annotation *p);
  void setParent(QC *p){pQC=p;}
};

class QC : public IObj
{
public:
  //! Default constructor.
  QC();
  ~QC();

  //! coresponding to virtual methods in IObj

  //! Check field properties of the variable.
  /*! Designed for multiple usage for sub-layers of a data block.
      Each multiple-set must be invoked by calling method 'clearStatistics'.*/
  bool   entry(void);

  //! Initialisation of the QC object.
  /*! Open the qc-result.nc file, when available or create
   it from scratch. Meta data checks are performed.
   Initialise time testing, time boundary testing, and cycles
   within a time step. At the end  entry() is called to test
   the data of fields.*/
  bool   init(void) ;
  void   linkObject(IObj *);
  void   setFilename(std::string n){filename=n;}
  void   setFilePath(std::string s){;}
  void   setTablePath(std::string p){ tablePath=p; }

  void   applyOptions(bool isPost=false);

  //! Check of dimensionless variables
  /*! These are represented by a single-value-dimension in the
      standard table and by a dimensionless variable in the file.*/
  bool   checkDimlessVar(InFile &in, Split &splt_line,
            VariableMetaData &var,
            struct DimensionMetaData *&p_dimFE,
            struct DimensionMetaData &dimFE_altern,
            struct DimensionMetaData &dimTE,
            std::map<std::string, size_t> &col);

  bool   checkDimSpecialValue(InFile &in, VariableMetaData &vMD,
            struct DimensionMetaData &, std::string &dimName) ;

  //! Comparison of dimensions between file and standard table
  /*! Cross-checks with the standard table are performed only once for
   each variable at first time encounter in the CMIP Project ensuring
   conformance.*/
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

  void   checkDimBndsName(InFile &in,
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

  bool   checkLonLatParamRep(InFile &in,
           VariableMetaData &vMD,
           std::string &nc_entry_name,
           std::string &tbl_entry_name);

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

  //! Match filename components and global attributes of the file.
  void   checkFilename(std::vector<std::string>&,
            std::string &stdSubTables_table);

  //! Checks meta-data
  void   checkMetaData(InFile &) ;

  //! Valid project name?
  void   check_ProjectName(InFile &) ;

  //! Prepare the comparison of dimensions between file and project table.
  /*! This is checked for each chunk or atomic data set in each
  experiment ensuring conformance.*/
  bool   checkProjectTable(std::ifstream &tbl, InFile &in,
            VariableMetaData &,
            std::vector<struct DimensionMetaData>&);

  //! Cross-check with standard table.
  /*! Prepare the check for dimensions and variable.*/
  bool   checkStandardTable(InFile &in,
            VariableMetaData &var,
            std::vector<struct DimensionMetaData>& );

  //! Check for the hybrid sigma pressure coordinates.*/
  /*! lev contains the added coefficients a(k) and b(k). A check
      for the sum does not make much sense, but, the existance of
      a(k) and b(k) does.*/
  void   checkSigmaPressureCoordinates(
            VariableMetaData &,
            std::vector<std::string> &);

  //! Check dimensional bounds: layout and size
  /*! Number of values and checksum of the bounds*/
  void   checkStandardTableDimBounds(InFile &in, Split &splt_line,
            VariableMetaData &var,
            struct DimensionMetaData &dimFE,
            struct DimensionMetaData &dimTE,
            std::map<std::string, size_t> &col) ;

  //! Check dimensional values: layout and size
  /*! Number of values and checksum*/
  void   checkStandardTableDimValues(InFile &in, Split &splt_line,
            VariableMetaData &var,
            struct DimensionMetaData &file,
            struct DimensionMetaData &table,
            std::map<std::string, size_t> &col) ;

  //! Starting function for all table cross-checks.
  void   checkTables(InFile &in, VariableMetaData &v);

  //! Apply the cross-check for the variable.
  void   checkVarTableEntry(
             VariableMetaData &,
             VariableMetaData &tbl_entry);

  //! Check time properties.
  /*! Close records for time and data.*/
  void   closeEntry(void);

  //! Variable objects for netcdf meta-data
  /*! Only the objects are created.*/
  void   createVarMetaData(void);

  //! The final operations.
  /*! An exit code is returned.*/
  int    finally(int errCode=0);

  //! The final qc data operations.
  /*! Called from finall(). An exit code is returned.*/
  int    finally_data(int errCode=0);

  //! Find occurrence of a heading line in string str0.
  /*! Returns true, if not found or the maximum number of columns needed.
      col maps required column titles to their index.*/
  bool   findNextVariableHeadline(ReadLine &, std::string &str0,
            VariableMetaData &var, std::vector<std::string> &);

  //! Find entry of a requested variable in the standard table.
  bool   findStandardEntry(ReadLine &, std::string &,
            VariableMetaData &var,
            std::map<std::string, size_t> &col, size_t col_max,
            std::vector<std::string> & );

  //! Find the name of requested sub-table in string str0.
  bool   findStdTables(ReadLine &, std::string &str0,
            VariableMetaData &var);

  void   findStdSubTables(std::string &str0,
            VariableMetaData &vMD, std::vector<std::string> &);

  std::string
       getCurrentTable(void){ return currTable ; }

  //! Store properties of a dimension in the struct.
  /*! Note: the name of the dimension is passed by the struct.*/
  void   getDimMetaData(InFile &in,
             VariableMetaData &,
             struct DimensionMetaData &, std::string dName);

  //! Get global attribute 'frequency'
  std::string
       getFrequency(void);

  //! Get the MIP table name from the global attributes
  std::string
       getGA_MIP_table(std::vector<std::string>&);

  //! get and check MIP table name
  void
       getMIP_table(VariableMetaData &);

  //! Return the name of the object.
  std::string
       getObjName(void) { return objName; }

  std::string
       getStandardTable(void){ return standardTable ; }

  std::string
       getTablePath(void){ return tablePath; }

  void getVarnameFromFilename(std::string &str);

  //! Brief description of options
  static void
       help(void);

  //! Check the path to the tables;
  void inqTables(void);

  //! Initialisation of flushing gathered results to netCDF file.
  /*! Parameter indicates the number of variables. */
  void initDataOutputBuffer(void);

  //! Set default values.
  void initDefaults(void);

  //! Global attributes of the qc-netCDF file.
  /*! Partly reflecting global attributes from the sources. */
  void initGlobalAtts(InFile &);

  //! Initialisiation of a resumed session.
  /*! Happens for non-atomic data sets and those that are yet incomplete. */
  void   initResumeSession(void);

  bool   isProgress(void){ return ! isNoProgress ; }

  //! Get coordinates of grid-cells where an error occurred
  /*! Does not work for tripolar coordinates */
//  bool   locate( GeoMetaT<float>*, double *lat, double *lon, const char* );

  //! Open a qc_result file for creation or appending data.
  /*! CopY time variable from input-nc file.
   Collect some properties of the in-netcdf-file in
   struct varMeDa. Also check properties against tables.
  */
  void   openQcNc(InFile&);

//  //! Print results to a text file.
//  /*! Not for CMIP5 or CORDEX */
//  void   print(GeoMetaT<float>*, hdhC::FieldData &fA);

  //! Perform only post-processing
  bool   postProc(void);
  bool   postProc_outlierTest(void);

  //! Read the headlines for dimensions and variable.
  /*! Read from the standard table; used to identify columns.*/
  bool   readHeadline(ReadLine &,
            VariableMetaData &,
            std::map<std::string, size_t> &v_col,
            std::map<std::string, size_t> &d_col,
            size_t &v_colMax, size_t &d_colMax);

  //! Connect this with the object to be checked
  void   setInFilePointer(InFile *p) { pIn = p; }

  //! get access to the global exception and annotation handling
  void setNotes(Annotation *n) {notes = n; }

  //! Unused.
  /*! Needed to be conform to a specific Base class functionality */
  void   setSrcStr(std::string s)
             {srcStr.push_back(s); return;}

  //! Set properties of variable from netcdf file
  void   setVarMetaData(VariableMetaData &);

  //! Store results in the internal buffer
  /*! The buffer is flushed to file from time to time.*/
  void   store(std::vector<hdhC::FieldData> &fA);
  void   storeData(std::vector<hdhC::FieldData> &fA);
  void   storeTime(void);

  //! Test the time-stamp of the input file.
  /*! If the end-date in the filename and the last time value
      match within the uncertainty of 0.75% of the time-step, then
      the file is assumed to be completely qc-processed.
      Syntax of date ranges as given in CMIP% DRS Syntax.*/
  bool   testPeriod(void);

  //! Name of the netCDF file with results of the quality control
  std::string qcFilename;
  std::string qcNcfileFlags;

  int exitCode;
  bool isExit;

  std::vector<VariableMetaData> varMeDa;

  NcAPI *nc;
  QC_Time qcTime;

  size_t currQcRec;
  size_t importedRecFromPrevQC; // initial num of recs in the write-to-nc-file
  MtrxArr<double> tmp_mv;

  // init for test about times
  bool enablePostProc;
  bool isUseStrict;
  bool enableVersionInHistory;
  bool isCaseInsensitiveVarName;
  bool isCheckParentExpID;
  bool isCheckParentExpRIP;
  bool isClearBits;
  bool isFileComplete;
  bool isForceStndTable;
  bool isNoProgress;
  bool isNotFirstRecord;
  bool isResumeSession;

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

  std::string dataPath;
  std::string maxDateRange;

  std::string cfStndNames;
  std::string currTable;
  std::string projectTableName;
  std::string standardTable;
  std::string tablePath;
  std::string MIP_tableName;
  std::string frequency;
  std::string totalPeriod;

  std::string parentExpID;
  std::string parentExpRIP;

  std::string experiment_id;

  int identNum;
  std::string filename;
  std::string fVarname;
  std::vector<std::string> srcStr;
  std::string svnVersion;

  std::string fail;
  std::string fileStr;
  std::string notAvailable;

  void        appendToHistory(size_t);
  std::string getCurrentTableSubst(void);
  std::string getSubjectsIntroDim(VariableMetaData &vMD,
                   struct DimensionMetaData &nc_entry,
                   struct DimensionMetaData &tbl_entry);
  bool        not_equal(double x1, double x2, double epsilon);
  void        pushBackVarMeDa(Variable*);
  void        setCheckMode(std::string);
  void        setExit(int);
  void        setTable(std::string, std::string acronym="");
};

#endif