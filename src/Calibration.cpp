#include <fstream>
#include "Calibration.h"
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>
#include <iterator>
#include <iostream>

using namespace frame_helper;
using namespace std;

Eigen::Affine3d ExtrinsicCalibration::getTransform() const
{
    Eigen::Vector3d t( tx, ty, tz ), r( rx, ry, rz );
    Eigen::Affine3d result = Eigen::Affine3d::Identity();
    if( r.norm() > 0 )
	result.linear() = Eigen::AngleAxisd( r.norm(), r.normalized() ).toRotationMatrix();

    result.translation() = t;

    return result;
}

StereoCalibration StereoCalibration::fromMatlabFile( const std::string& file_name )
{
    ifstream ifs( file_name.c_str() );
    string line;

    // strategy is to read the input matlab file into 
    // a map, which contains the key (matlab variable)
    // and stores the vector as a vector of floats
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

    // check if all fields are there
    assert( raw["fc_left"].size() >= 2 );
    assert( raw["cc_left"].size() >= 2 );
    assert( raw["kc_left"].size() >= 4 );

    assert( raw["fc_right"].size() >= 2 );
    assert( raw["cc_right"].size() >= 2 );
    assert( raw["kc_right"].size() >= 4 );

    assert( raw["T"].size() >= 3 );
    assert( raw["om"].size() >= 3 );

    // now we can fill the calibration based on the matlab
    // vectors we have obtained
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
