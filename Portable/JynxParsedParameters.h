
#pragma once

#include "ParameterParsing.h"



template<typename STRING_CLASS>
class JynxParsedParameters
{
public:

    explicit JynxParsedParameters( const std::vector<STRING_CLASS> &paramList );

    STRING_CLASS  GetSettingsFilePath() const { return _settingsFilePath; }
    STRING_CLASS  GetSnapshotFilePath() const { return _snapshotFilePath; }
    STRING_CLASS  GetTapFilePath()      const { return _tapFilePath; }
    bool          GetGamesMode()        const { return _gamesMode; }

private:

    STRING_CLASS  _settingsFilePath;
    STRING_CLASS  _snapshotFilePath;
    STRING_CLASS  _tapFilePath;
    bool          _gamesMode = false;

};



template<typename STRING_CLASS>
JynxParsedParameters<STRING_CLASS>::JynxParsedParameters( const std::vector<STRING_CLASS> &paramList )
{
    size_t indexWithinList = 0;  // Advanced by the parsing functions when returning true.

    while( indexWithinList < paramList.size() )
    {
             if( ParseParamAndValue( paramList, indexWithinList, "--settings", &_settingsFilePath ) ) {}
        else if( ParseParamAndValue( paramList, indexWithinList, "--snapshot", &_snapshotFilePath ) ) {}
        else if( ParseParamAndValue( paramList, indexWithinList, "--run",      &_tapFilePath ) ) {}
        else if( ParseParam(         paramList, indexWithinList, "--games",    &_gamesMode ) ) {}
        else RaiseCommandLineSyntaxErrorException();
    }
}


