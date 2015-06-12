
#include "ParameterParsing.h"



void RaiseCommandLineSyntaxErrorException()
{
	throw std::invalid_argument( "Syntax error in command line." );
}




void RaiseCommandLineMissingOperandException()
{
	throw std::invalid_argument( "Command line parameter is missing an operand." );
}



