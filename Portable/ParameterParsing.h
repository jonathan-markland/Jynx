
#pragma once

#include <vector>
#include <string>
#include <stdexcept>


void RaiseCommandLineSyntaxErrorException();
void RaiseCommandLineMissingOperandException();



template<typename CHAR_TYPE>
std::vector<std::basic_string<CHAR_TYPE>>  MakeParamsListFromArgcAndArgv( int argc, CHAR_TYPE **argv, int startIndex )
{
    std::vector<std::basic_string<CHAR_TYPE>>  paramsList;

    for( int i = startIndex; i < argc; i++)
    {
        paramsList.push_back( argv[i] );
    }

    return paramsList;
}






template<typename STRING_CLASS>
bool ParseParamAndValue( const std::vector<STRING_CLASS> &paramList, size_t &i, const char *switchString, STRING_CLASS *out_variable )
{
	if( i < paramList.size() )
	{
		if( paramList[i] == switchString )
		{
			++i;
			if( i >= paramList.size() )  RaiseCommandLineMissingOperandException();
			if( paramList[i][0] == '-' ) RaiseCommandLineMissingOperandException();
			*out_variable = paramList[i];
			++i;
			return true;
		}
	}
	return false;
}



template<typename STRING_CLASS>
bool ParseParam( const std::vector<STRING_CLASS> &paramList, size_t &i, const char *switchString, bool *out_variable )
{
	if( i < paramList.size() )
	{
		if( paramList[i] == switchString )
		{
			++i;
			*out_variable = true;
			return true;
		}
	}
	return false;
}


