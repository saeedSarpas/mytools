#ifndef MATCHFINDER_H
#define MATCHFINDER_H


#define INTERNALMATCHINGTRUE 1
#define INTERNALMATCHINGFALSE 0


typedef struct _matchfinder_params
{
  char *priHalosAddr;
  char *secHalosAddr;
  char *priICAddr;
  char *secICAddr;
  int internalMatching;
  int loadBinMatches;
  char *binMatchesAddr;
  double massOffset;
  double maxDisplacement;
  int initVolResolution;
  int saveBinMatches;
  int saveSingleMatches;
  char *singleMatchesDir;
  int numSingleMatches;
  char *priSnapAddr;
  char *secSnapAddr;
  char *asciiOutputPath;
} matchfinder_params;


#endif /* MATCHFINDER_H */
