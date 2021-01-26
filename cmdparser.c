#include <stdlib.h>
#include <stdio.h>
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */


#define MAX_LINE_LENGTH 255

int Program_Counter;
int Orig;
int Table_Counter = 0;

enum{
     DONE,
     OK,
     EMPTY_LINE
    };

FILE* infile = NULL;
FILE* outfile = NULL;

int isOpcode(char * ptr);
int toNum(char * pStr );
int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char ** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4);

struct table_element{
    char *label;
    int address;
};
struct table_element symbol_table[MAX_LINE_LENGTH];

int
main(int argc, char* argv[]) {

    char *prgName   = NULL;
    char *iFileName = NULL;
    char *oFileName = NULL;

    prgName   = argv[0];
    iFileName = argv[1];
    oFileName = argv[2];

    

    printf("program name = '%s'\n", prgName);
    printf("input file name = '%s'\n", iFileName);
    printf("output file name = '%s'\n", oFileName);

    char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1,
                *lArg2, *lArg3, *lArg4;
    int lRet;

    /* open the source file */
    infile = fopen(argv[1], "r");
    outfile = fopen(argv[2], "w");

    if (!infile) {
      printf("Error: Cannot open file %s\n", argv[1]);
      exit(4);
                 }
    if (!outfile) {
      printf("Error: Cannot open file %s\n", argv[2]);
      exit(4);
    }

    do{
      lRet = readAndParse( infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
      if( lRet != DONE && lRet != EMPTY_LINE ){
        // if .orig then record the starting address
        // else add to the program counter 
        if(strcmp(lOpcode, ".orig")==0){
          Orig = toNum(lArg1);
          Program_Counter = Orig;
        }else{

          if(lLabel != NULL){

            struct table_element elem;
            strcpy(elem.label, lLabel);
            elem.address = Program_Counter;
            symbol_table[Table_Counter] = elem;
          }

          Program_Counter++;
        }


        if(!*lArg2){

          int num_to_file = toNum(lArg1);
	        fprintf( outfile, "0x%.4X\n", num_to_file);
	      
        }
      }
    } while( lRet != DONE );



    /* Do stuff with files */
    do{
       lRet = readAndParse( infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
       if( lRet != DONE && lRet != EMPTY_LINE ){
        if(!isOpcode(lOpcode)){
          if(strcmp(lOpcode, "add")==0){


          }else if(strcmp(lOpcode, "and")==0){


          }else if(strcmp(lOpcode, "and")==0){


          }else if(strcmp(lOpcode, "br")==0){


          }else if(strcmp(lOpcode, "jmp")==0){


          }else if(strcmp(lOpcode, "jsr")==0){


          }else if(strcmp(lOpcode, "jsrr")==0){


          }else if(strcmp(lOpcode, "ldb")==0){


          }else if(strcmp(lOpcode, "ldw")==0){


          }else if(strcmp(lOpcode, "lea")==0){


          }else if(strcmp(lOpcode, "not")==0){


          }else if(strcmp(lOpcode, "ret")==0){


          }else if(strcmp(lOpcode, "lshf")==0){


          }else if(strcmp(lOpcode, "rshfl")==0){


          }else if(strcmp(lOpcode, "rshfa")==0){


          }else if(strcmp(lOpcode, "stb")==0){


          }else if(strcmp(lOpcode, "stw")==0){


          }else if(strcmp(lOpcode, "trap")==0){


          }else if(strcmp(lOpcode, "xor")==0){


          }else{
            //error
          }
        }else{
          if(strcmp(lOpcode, ".orig")==0){


          }else if(strcmp(lOpcode, ".end")==0){


          }else if(strcmp(lOpcode, ".fill")==0){

          }else{
            //error
          }
        }

        if(!*lArg2){

          int num_to_file = toNum(lArg1);
	        fprintf( outfile, "0x%.4X\n", num_to_file);
	      
        }
      }
    } while( lRet != DONE );



    fclose(infile);
    fclose(outfile);
}
int
toNum( char * pStr )
{
  char * t_ptr;
  char * orig_pStr;
  int t_length,k;
  int lNum, lNeg = 0;
  long int lNumLong;

  orig_pStr = pStr;
  if( *pStr == '#' )                                /* decimal */
  {
    pStr++;
    if( *pStr == '-' )                                /* dec is negative */
    {
      lNeg = 1;
      pStr++;
    }
    t_ptr = pStr;
    t_length = strlen(t_ptr);
    for(k=0;k < t_length;k++)
    {
      if (!isdigit(*t_ptr))
      {
         printf("Error: invalid decimal operand, %s\n",orig_pStr);
         exit(4);
      }
      t_ptr++;
    }
    lNum = atoi(pStr);
    if (lNeg)
      lNum = -lNum;

    return lNum;
  }
  else if( *pStr == 'x' )        /* hex     */
  {
    pStr++;
    if( *pStr == '-' )                                /* hex is negative */
    {
      lNeg = 1;
      pStr++;
    }
    t_ptr = pStr;
    t_length = strlen(t_ptr);
    for(k=0;k < t_length;k++)
    {
      if (!isxdigit(*t_ptr))
      {
         printf("Error: invalid hex operand, %s\n",orig_pStr);
         exit(4);
      }
      t_ptr++;
    }
    lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
    lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
    if( lNeg )
      lNum = -lNum;
    return lNum;
  }
  else
  {
        printf( "Error: invalid operand, %s\n", orig_pStr);
        exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
  }
}

int isOpcode(char * ptr){
  
  if(strcmp(ptr, "add")==0){
    return 1;

  }else if(strcmp(ptr, "and")==0){
    return 1;

  }else if(strcmp(ptr, "and")==0){
    return 1;

  }else if(strcmp(ptr, "br")==0){
    return 1;

  }else if(strcmp(ptr, "jmp")==0){
    return 1;

  }else if(strcmp(ptr, "jsr")==0){
    return 1;

  }else if(strcmp(ptr, "jsrr")==0){
    return 1;

  }else if(strcmp(ptr, "ldb")==0){
    return 1;

  }else if(strcmp(ptr, "ldw")==0){
    return 1;

  }else if(strcmp(ptr, "lea")==0){
    return 1;

  }else if(strcmp(ptr, "not")==0){
    return 1;

  }else if(strcmp(ptr, "ret")==0){
    return 1;

  }else if(strcmp(ptr, "lshf")==0){
    return 1;

  }else if(strcmp(ptr, "rshfl")==0){
    return 1;

  }else if(strcmp(ptr, "rshfa")==0){
    return 1;

  }else if(strcmp(ptr, "stb")==0){
    return 1;

  }else if(strcmp(ptr, "stw")==0){
    return 1;

  }else if(strcmp(ptr, "trap")==0){
    return 1;

  }else if(strcmp(ptr, "xor")==0){
    return 1;

  }
  
  return -1;
}


int
  readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char ** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4)
        {
           char * lRet, * lPtr;
           int i;
           if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
                return( DONE );
           for( i = 0; i < strlen( pLine ); i++ )
                pLine[i] = tolower( pLine[i] );

          /* convert entire line to lowercase */
           *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

           /* ignore the comments */
           lPtr = pLine;

           while( *lPtr != ';' && *lPtr != '\0' &&
           *lPtr != '\n' )
                lPtr++;

           *lPtr = '\0';
           if( !(lPtr = strtok( pLine, "\t\n ," ) ) )
                return( EMPTY_LINE );

           if( isOpcode( lPtr ) == -1 && lPtr[0] != '.' ) /* found a label */
           {
                *pLabel = lPtr;
                if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
           }

          *pOpcode = lPtr;

           if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

          *pArg1 = lPtr;

          if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

           *pArg2 = lPtr;
           if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

           *pArg3 = lPtr;

           if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

           *pArg4 = lPtr;

           return( OK );
        }

        /* Note: MAX_LINE_LENGTH, OK, EMPTY_LINE, and DONE are defined values */
