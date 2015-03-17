#ifndef HDH_C_H
#define HDH_C_H

#include <algorithm>
#include <cmath>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <typeinfo>
#include <vector>

#include <stddef.h>
#include <values.h>
#include <unistd.h>
#include <stdint.h>

#include "split.h"

/*! \file hdhC.h
 \brief Helper functions.
*/

//class Upper ;
//class Lower;
//! Namespace for a collection of functions.

/*! Note: many of these functions could be obsolete.\n
  Requires an instance of class Split.*/

namespace hdhC
{

//! Obsolete.
const double EQUALITY_TOLERANCE=1.e-12 ;

//! Epsilon tolerance in comparisons.
/*! Facilitates equality comparisons of floating numbers,
 e.g.: double x=1./10. ; if(fabs(x-0.1) < NUMERICAL_TOLERANCE)...*/
const double NUMERICAL_TOLERANCE=1.e-10 ;

//! Struct: simple statistics of a geographical area
struct FieldData
{
//! Is any valid average defined?
  bool   isValid;
  bool   isStndDevValid;

//! Minimum
  double      min;

//! Maximum
  double      max;

//! The number of grid-cells marked as missing|filling value.
  size_t fillingValueNum;
  size_t infCount;
  size_t nanCount;

//! Weighted average over the effective area.
/*! Cells marked as missing value are exclude from area.*/
  double areaWeightedAve;

//! Standard deviation (weighted)
  double stndDev;

//! Fraction of the effective area, i.e. excluding fillingValue grids.
  double areaFract;

//! Checksum of the data (inclusively filling values)
  uint32_t checksum;

//! Ensemble size
  size_t size;
};

//! Struct: simple statistics of a geographical area
struct FieldDataMeta
{
//! Is any valid average defined?
  bool   isValid;

//! Minimum
  double      min;

//! Maximum
  double      max;

//! The number of grid-cells marked as missing|filling value.
  size_t fillingValueNum;
  size_t infCount;
  size_t nanCount;

//! Ensemble size
  size_t size;
};

//! Struct defining a point coordinate
struct Point
{
  double x;
  double y;
};

template<typename T>
T clearLeastBits( T v, size_t shft=2);

//! Compare x and y within uncertainty ranges e(psilon)
/*! A factor of 2 is substituted in epsilon
 modes: op: "==" --> fabs(x - y) < e \n
        op: "<"  --> x + e < y \n
        op: ">"  --> x > y + e
*/
bool
compare(double x, double y,
           std::string op="eq", double epsilon=NUMERICAL_TOLERANCE);

/*! epsilon is 10^-DECS of the smaller value of the magnitude of x and y*/
bool
compare(double x, double y,
           std::string op="eq", int DECS=10);

//! Convert time
/*! Time given in a certain unit is converted to a target unit.
 A sub-string (year, month, day, hour, minute, second) may be
 appended to the time string or be given as separate parameter.
 Note: A distinctive number of characters is sufficient.
 Note: If both appended units and such in the parameter list are given,
 then the appended one prevails.
 Note: This function does not work, if the conversion
 crosses the border between year, months on the on hand and
 the smaller units on the other.
 Use in these cases the class-method Date.addTime().*/
double
convertTime(std::string targetUnit, std::string time,
            std::string unit="");

//! Convert a floating number into a string.
/*!Format: ?[w=int?][p=int[|adj]?][f=int?][s[ci]]?\n
   ?: separator (by default ','). The pre- and post-fix may
     be omitted for the default. If changed, then the prefix
     has to be formed by twice a character (not: w, p,f,s).
   w: width of the field,
   p: precision with 'adj' discarding tariling zeros and dec. point,
   f: filling character, sci: scientific notation.
   Note: assignment '=' may be omitted.*/

std::string
double2String( double z, std::string flag);

//! Convert a floating number into a string.
/*! Number 'z' is represented with precision of 'd' with rounding.
 Format of 'flag' is 'n_X with n leading positions filled by
 character X. The _-char may be omitted if X is not a number.
 Negative value of d indicates rounding with additional
 elimination of trailing zeros and a trailing decimal point.*/
std::string
double2String( double z, int d=-5, std::string flag="");

//! Convert decimal degree to degree-minute-second.
/*! Decimal degrees are converted to traditional coordinates
 (degree, minute, seconds).*/
double
deci2dgr(double degree, double minute=0., double second=0. );

//! Convert degree-minute-second to decimal degrees.
/*! Traditional coordinates (degree, minute, seconds) are converted
 to decimal degree.*/
double
dgr2deci(std::string );

//! Obsolete.
void
envelope( std::vector<double> &w, std::vector<double> &fct,
          std::vector<double> &wE, std::vector<double> &E,
          bool upper=true );

//! Test of equality even of extreme small/large values
template<typename T>
bool
equal(T x1, T x2, double epsilon);

//! Checksum of a vector of data applying the Fletcher 32-bit algorithm.
/*! Reference: Fletcher32 algorithm \n
 http://en.wikipedia.org/wiki/Fletcher%27s_checksum
 Ref.: Fletcher, J. G., ?An Arithmetic Checksum for Serial
 Transmissions?, IEEE Trans. on Comm., Vol. COM-30, pp 247-252.\n
The start for a new sequence is indicated by boolean reset=true,
otherwise continued. String data ends with '\0'. If the size of the string is even, then '\0' is discarded, otherwise included. The boolean pointer is set to false after the first call.\n
Note: *_cmip5 versions do not calculate the Fletcher32
checksum as expected for data arrays that are
sub-divided and processed in sequential calls,
because the second reduction step is done at the
end of each call.
The versions are kept for CMIP5 application in order
to get backward compatibility.*/

uint32_t
fletcher32_cmip5( std::string &data, bool *reset=0);

//! Building the [accumulated] checksum from single doubles.
/*! Each double is converted to a string representation with default
 rounding of the function hdhC::double2String and then
 back-converted; exept isConvert is set false.*/
uint32_t
fletcher32_cmip5( double, bool *reset=0);

uint32_t
fletcher32_cmip5( double*, size_t sz=1, bool *reset=0);

uint32_t
fletcher32_cmip5( const char *data, size_t byte_sz=1, bool *reset=0 );

uint32_t
fletcher32( std::string &data, bool *reset=0);

template<typename T>
uint32_t
fletcher32( std::vector<T> &data, bool *reset=0, size_t clear=0);

template<typename T>
uint32_t
fletcher32( T, bool *reset=0, size_t shift=0);

template<typename T>
uint32_t
fletcher32( T*, size_t sz=1, bool *reset=0, size_t clear=0);

template<typename T>
uint32_t
fletcher32( const T*, size_t sz=1, bool *reset=0, size_t clear=0);

uint32_t
fletcher32_BE( uint8_t *data, size_t sz, bool *reset);

template<typename T>
uint32_t
fletcher32_LE( T *data, size_t sz, bool *reset);

template<typename T>
uint32_t
fletcher32_BE_clear( T *data, size_t sz, bool *reset, size_t clear=0);

template<typename T>
uint32_t
fletcher32_LE_clear( T *data, size_t sz, bool *reset, size_t clear=0);

//! Extract path, discard the filename.
std::string
getPath(std::string&, bool isWithSlash=false );

//! Extract filename, discard the path.
std::string
getFilename(std::string& );

//! Get filename without extension
std::string
getBasename(std::string& );

//! Get extension.
std::string
getExtension(std::string& );

//! Get all path components
void
getPathComponents(std::string &str, std::string &path,
                  std::string &fName, std::string &base,
                  std::string &ext);

//! Invert the ordering of bits
/*! For any type of T*/
template <typename T>
T invertBits ( T );

//! Print bits of any type
template <typename T>
void printBits ( T val );

//! Get non-alpha characters.
/*! Return empty if no one was found.*/
std::string
getNonAlpha(std::string);

//! Get the non-alpha-numeric characters.
std::string
getNonAlphaNum(std::string);

//! Remove space-separated duplicates
std::string
getUniqueString(std::string&);

std::string
getUniqueString(std::vector<std::string>&);

std::vector<std::string>
getUniqueVector(std::string&);

std::vector<std::string>
getUniqueVector(std::vector<std::string>&);

//! Is unsigned char a alphabetic?
bool
isAlpha(unsigned char);

//! Is string composed of alphas?
/*! If 'isContains' is true, then a single alpha is sufficient.*/
bool
isAlpha(std::string, bool isContains=false);

bool
isAlphaNum(unsigned char);

template <typename T>
bool
isAmong(T&, std::vector<T>& set);

template <typename T>
bool
isAmong(T, std::vector<T>& set, bool);  // bool for the compiler

// all==true: the entire sub-set must be found in the set
template <typename T>
bool
isAmong(std::vector<T>& sub, std::vector<T>& set, bool all=false);

//! Is unsigned char a digit?
/*! If isNumber is true, then also sign and decimal point are accepted.*/
bool
isDigit(unsigned char, bool isNumber=false);

//! Is string composed of digits?
/*! If 'isContains' is true, then a single digit is sufficient.*/
bool
isDigit(std::string, bool isContains=false);

//! Does string contain any non-digit?
bool
isNonDigit(std::string);

//! Is string a number?
/*! The string may be e- or E-format*/
bool
isNumber( std::string & );

//! Is string only of white-spaces
bool
isWhiteString( std::string &str );

//double
//interpol(int ma, std::vector<double> &xV, std::vector<double> &yV, double x);

/*
void
interpolArray2Array( int idxa, int idxn , int idya, int idyn,
                     double spv, double **fa, double **fn);
*/

//! Convert int to string (based on K&R).
std::string
itoa(int n );

//! Convert int to string (K&R).
/*! For very long numbers, e.g. the year 2100 expressed in
 seconds since 1956.*/
std::string
itoa(unsigned long long int n, int sign);

//! Approximate the vertex of a curve by a parabola.
Point
parabolaSummit(double xm, double x, double xp, double ym,
               double y, double yp);

//! Planck function.
double
planck(double anue,double t);

//! Convert string to double (K&R).
/*! Extract a number from string. Multiple numbers in a string
 separated by any char that is not part of a number (i.e. not
 e, E, -,+) may be skipped ny parameter 'nr' counting from 1*/
bool
string2Double( std::string s, size_t nr, double* retVal) ;

//! Convert string to double
double
string2Double( std::string s, size_t nr=1,bool isRetNull=false) ;

//! Convert string to double
double
string2Double( std::string s, size_t nr, int &width,
               int &precision,  bool isRetNull=false) ;

//! Convert string to double
double
string2Double( std::string s, size_t nr ,int *restIndex,
               bool isRetNull=false) ;

//! Remove all characters from a str.
/*! By default, each character in 's' is removed. If isStr==true, then only
    every occurrence of the string 's'.*/
std::string
clearChars(std::string &str, std::string s="", bool isStr=false );

//! replace multiple internal spaces by a single blank
std::string
clearInternalMultipleSpaces(std::string &str );

//! rermove white spaces and newlines
std::string
clearSpaces(std::string &str );

std::string
sAssign(std::string left, std::string right, bool withSpaces=false) ;

std::string
sAssign(std::string right, bool withSpaces=false) ;

//! strip off surrounding characters.
/*! Surrounding ' ' and '\t' are removed, also any character provided
   in 'key'. Additionally, keyword 'left' and 'right' may be given
   causing stripping only from the left or right, respectively. */
std::string
stripSurrounding(std::string &str, std::string key="" );

//! Convert char to lower case
void
lowerCase(char &c);

//! Convert char to upper case
void
upperCase(char &c);

//! Convert string to upper case
class Upper
{
  public:
  std::string operator()(std::string s, bool self=false)
  {
    if( self )
    {
      std::for_each(s.begin(), s.end(), hdhC::upperCase);
      return s;
    }
    else
    {
      std::string t(s) ;
      std::for_each(t.begin(), t.end(), hdhC::upperCase);
      return t;
    }
  }
};

//! Convert string to lower case
class Lower
{
  public:
  std::string operator()(std::string s, bool self=false)
  {
    if( self )
    {
      std::for_each(s.begin(), s.end(), hdhC::lowerCase);
      return s;
    }
    else
    {
      std::string t(s) ;
      std::for_each(t.begin(), t.end(), hdhC::lowerCase);
      return t;
    }
  }
};

} // end of namespace


#endif