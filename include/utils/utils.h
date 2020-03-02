#ifndef _UTILS_H
#define _UTILS_H

// #include "all.h"

//-------------------------- MACRO UTILITIES BEGIN ---------------------
// A custom assert block for assertions
#define ASSERT( condition, statement, ... ) \
   if( !(condition) ) { \
      fprintf( stderr, "[ASSERT] In File: %s, Line: %d => " #statement "\n", __FILE__, __LINE__, ##__VA_ARGS__ ); \
      abort(); \
   }
//--------------------------- MACRO UTILITIES END ----------------------

std::string trim(const std::string& str);
std::string str2Upper(const std::string& str_in, bool trim_input=false) {return {};}
double interpolate( std::vector <std::vector<double>> data, double x, bool extrapolate ) {return 0;}
double normalCDF(double x) {return 0;} // Phi(-∞, x) aka N(x)
double normalCDF ( double x, double mu, double sigma ) {return 0;}
double lnCDF ( double x, double mu, double sigma ) {return 0;}

#endif // _UTILS_H
