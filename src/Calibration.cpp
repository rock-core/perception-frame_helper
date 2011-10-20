#include <fstream>
#include "Calibration.h"
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>
#include <iterator>
#include <iostream>

using namespace frame_helper;
using namespace std;

StereoCalibration StereoCalibration::fromMatlabFile( const std::string& file_name )
{
    ifstream ifs( file_name.c_str() );
    string line;

    map<string, vector<double> > raw;

    boost::regex e( "(\\w+?) = \\[([^\\]]+?)\\]" );
    boost::smatch what;

    while( getline( ifs, line ) )
    {
	if( boost::regex_search( line, what, e ) )
	{
	    if( what.size() == 3 )
	    {
		raw[what[1]] = vector<double>();
		istringstream iss( what[2] );
		istream_iterator<string> is( iss );
		for(; is != istream_iterator<string>(); is++ )
		    raw[what[1]].push_back( boost::lexical_cast<double>( *is ) );
	    }
	}
    }

    StereoCalibration result = 
    {
	{ raw["fc_left"][0], raw["fc_left"][1], 
	  raw["cc_left"][0], raw["cc_left"][1], 
	  raw["kc_left"][0], raw["kc_left"][1], raw["kc_left"][2], raw["kc_left"][3] },
	{ raw["fc_right"][0], raw["fc_right"][1], 
	  raw["cc_right"][0], raw["cc_right"][1], 
	  raw["kc_right"][0], raw["kc_right"][1], raw["kc_right"][2], raw["kc_right"][3] },
	{
	  raw["T"][0], raw["T"][1], raw["T"][2],
	  raw["om"][0], raw["om"][1], raw["om"][2]
	}
    };

    return result;
}
