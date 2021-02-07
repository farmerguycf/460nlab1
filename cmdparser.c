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

struct table_element{
    char * label;
    int address;
};
struct table_element symbol_table[MAX_LINE_LENGTH];

void isReg(char * ptr);
int isOpcode(char * ptr);
int toNum(char *pStr);
int getRegNumber(char * pStr);
int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char ** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4);
int get_label_offset(char *label){
    for(int i = 0; i<Table_Counter;i++){
        if(strcmp(symbol_table[i].label, label)==0){
            return (symbol_table[i].address - Program_Counter)/2;
        }
    }
    exit(1);
    return -1;
}// branch instruction 
//1 Operand Expected
int inst0(char *opcode, char *arg1, char *arg2, char *arg3, char *arg4){
    if((strcmp(arg1, "")==0) || (strcmp(arg2, "")!=0) || (strcmp(arg3, "")!=0) || (strcmp(arg4, "")!=0)){
      exit(4);
    }
    int decoded_inst = 0x0000;

    if(strchr(opcode,'n')!=NULL){
        int n = 1 << 11;
        decoded_inst = decoded_inst | n;
    }if(strchr(opcode,'z')!=NULL){
        int z = 1 << 10;
        decoded_inst = decoded_inst | z;
    }
    if(strchr(opcode,'p')!=NULL){
        int p = 1 << 9;
        decoded_inst = decoded_inst | p;
    }
    if(strchr(opcode,'n')==NULL && strchr(opcode,'z')==NULL && strchr(opcode,'p')==NULL){
      int n = 1 << 11;
        decoded_inst = decoded_inst | n;
        int z = 1 << 10;
        decoded_inst = decoded_inst | z;
        int p = 1 << 9;
        decoded_inst = decoded_inst | p;
    }

    int offset = get_label_offset(arg1);
    if(offset < -256 || offset > 255){
      exit(4);
      return -1;
      }
    offset = offset & 0x01FF;
    
    return decoded_inst | offset;
    

}// add instruction
//3 Operands Expected
int inst1(char *opcode, char *arg1, char *arg2, char *arg3, char *arg4){

    if((strcmp(arg1, "")==0) || (strcmp(arg2, "")==0) || (strcmp(arg3, "")==0) || (strcmp(arg4, "")!=0)){
      exit(4);
    }

    int decoded_inst = 0x1000;
    isReg(arg1);
    int dr = getRegNumber(&arg1[1]);
    if(dr>7||dr<0){
      exit(4);
    }
    dr = dr << 9;
    decoded_inst = decoded_inst | dr;

    isReg(arg2);
    int sr1 = getRegNumber(&arg2[1]);
    if(sr1>7||sr1<0){
      exit(4);
    }
    sr1 = sr1 << 6;
    
    decoded_inst = decoded_inst | sr1;

    
    if(arg3[0] == 'r'){
        isReg(arg3);
        int sr2 = getRegNumber(&arg3[1]);
        if(sr2>7||sr2<0){
          exit(4);
        }
        decoded_inst = decoded_inst | sr2;
    }else{
        
        decoded_inst = decoded_inst | (1<<5);
        int amount5 = toNum(arg3);
        if(amount5 > 15 || amount5 < -16){
          exit(3);
        }
        amount5 = amount5  & 0x001F;
        decoded_inst = decoded_inst | amount5;
    }
    return decoded_inst;
}// ldb instruction
// 3 Operands Expected
int inst2(char *opcode, char *arg1, char *arg2, char *arg3, char *arg4){

    if((strcmp(arg1, "")==0) || (strcmp(arg2, "")==0) || (strcmp(arg3, "")==0) || (strcmp(arg4, "")!=0)){
      exit(4);
    }

    int decoded_inst = 0x2000;
    isReg(arg1);
    int dr = getRegNumber(&arg1[1]);
    if(dr>7||dr<0){
      exit(4);
    }
    dr = dr << 9;
    decoded_inst = decoded_inst | dr;

    isReg(arg2);
    int basereg = getRegNumber(&arg2[1]);
    if(basereg>7||basereg<0){
      exit(4);
    }
    basereg = basereg << 6;
    decoded_inst = decoded_inst | basereg;


    int offset = toNum(arg3);
    if(offset > 31 || offset < -32){
          exit(3);
    }

    offset = offset & 0x003F;
    decoded_inst = decoded_inst | offset;
    return decoded_inst;
}// stb instruction
// 3 Operands Expected
int inst3(char *opcode, char *arg1, char *arg2, char *arg3, char *arg4){

    if((strcmp(arg1, "")==0) || (strcmp(arg2, "")==0) || (strcmp(arg3, "")==0) || (strcmp(arg4, "")!=0)){
      exit(4);
    }

    int decoded_inst = 0x3000;
    isReg(arg1);
    int dr = getRegNumber(&arg1[1]);
    if(dr>7||dr<0){
      exit(4);
    }
    dr = dr << 9;
    decoded_inst = decoded_inst | dr;

    isReg(arg2);
    int basereg = getRegNumber(&arg2[1]);
    if(basereg>7||basereg<0){
      exit(4);
    }
    basereg = basereg << 6;
    decoded_inst = decoded_inst | basereg;

    int offset = toNum(arg3);
    if(offset > 31 || offset < -32){
          exit(3);
    }

    offset = offset & 0x003F;

    decoded_inst = decoded_inst | offset;
    return decoded_inst;

}// jsr / jsrr instruction
//1 Operand
int inst4(char *opcode, char *arg1, char *arg2, char *arg3, char *arg4){
    int decoded_inst = 0x4000;

    if((strcmp(arg1, "")==0) || (strcmp(arg2, "")!=0) || (strcmp(arg3, "")!=0) || (strcmp(arg4, "")!=0)){
      exit(4);
    }

    if(strcmp(opcode, "jsr")==0){
        decoded_inst = decoded_inst | (1<<11);
        int offset = get_label_offset(arg1);
        if(offset < -1024 || offset > 1023){
          exit(4);
        }
        offset = offset & 0x07FF;
        return decoded_inst | offset;
        


    }else{
        decoded_inst = decoded_inst | (getRegNumber(&arg1[1])<<6);
    }
    return decoded_inst;
} // and instruction
// 3 Operands Expected
int inst5(char *opcode, char *arg1, char *arg2, char *arg3, char *arg4){

    if((strcmp(arg1, "")==0) || (strcmp(arg2, "")==0) || (strcmp(arg3, "")==0) || (strcmp(arg4, "")!=0)){
      exit(4);
    }

    int decoded_inst = 0x5000;

    isReg(arg1);
    int dr = getRegNumber(&arg1[1]);
    if(dr>7||dr<0){
      exit(4);
    }
    dr = dr << 9;
    decoded_inst = decoded_inst | dr;

    isReg(arg2);
    int sr1 = getRegNumber(&arg2[1]);
    if(sr1>7||sr1<0){
      exit(4);
    }
    sr1 = sr1 << 6;
    decoded_inst = decoded_inst | sr1;

    if(arg3[0] == 'r'){
        isReg(arg3);        
        int sr2 = getRegNumber(&arg3[1]);
        if(sr2>7||sr2<0){
      exit(4);
    }
        decoded_inst = decoded_inst | sr2;
    }else{
        
        decoded_inst = decoded_inst | (1<<5);
        int amount5 = toNum(arg3);
        if(amount5 > 15 || amount5 < -16){
          exit(3);
        }
        amount5 = amount5  & 0x001F;
        decoded_inst = decoded_inst | amount5;
    }
    return decoded_inst;
}// ldw instruction
// 3 Operands Expected
int inst6(char *opcode, char *arg1, char *arg2, char *arg3, char *arg4){

    if((strcmp(arg1, "")==0) || (strcmp(arg2, "")==0) || (strcmp(arg3, "")==0) || (strcmp(arg4, "")!=0)){
      exit(4);
    }

    int decoded_inst = 0x6000;
    isReg(arg1);
    int dr = getRegNumber(&arg1[1]);
    if(dr>7||dr<0){
      exit(4);
    }
    dr = dr << 9;
    decoded_inst = decoded_inst | dr;

    isReg(arg2);
    int basereg = getRegNumber(&arg2[1]);
    if(basereg>7||basereg<0){
      exit(4);
    }
    basereg = basereg << 6;
    decoded_inst = decoded_inst | basereg;

    int offset = toNum(arg3);
    if(offset > 31 || offset < -32){
          exit(3);
    }

    offset = offset & 0x003F;
    decoded_inst = decoded_inst | offset;
    return decoded_inst;

}// stw instruction
// 3 Operands Expected
int inst7(char *opcode, char *arg1, char *arg2, char *arg3, char *arg4){

    if((strcmp(arg1, "")==0) || (strcmp(arg2, "")==0) || (strcmp(arg3, "")==0) || (strcmp(arg4, "")!=0)){
      exit(4);
    }

    int decoded_inst = 0x7000;
    isReg(arg1);
    int dr = getRegNumber(&arg1[1]);
    if(dr>7||dr<0){
      exit(4);
    }
    dr = dr << 9;
    decoded_inst = decoded_inst | dr;

    isReg(arg2);
    int basereg = getRegNumber(&arg2[1]);
    if(basereg>7||basereg<0){
      exit(4);
    }
    basereg = basereg << 6;
    decoded_inst = decoded_inst | basereg;

    int offset = toNum(arg3);
    if(offset > 31 || offset < -32){
          exit(3);
    }

    offset = offset & 0x003F;

    decoded_inst = decoded_inst | offset;
    return decoded_inst;

}// rti instruction
//no operands
int inst8(char *opcode, char *arg1, char *arg2, char *arg3, char *arg4){
    if((strcmp(arg1, "")!=0) || (strcmp(arg2, "")!=0) || (strcmp(arg3, "")!=0) || (strcmp(arg4, "")!=0)){
      exit(4);
    }

    return 0x8000;
}// xor / nor instruction
//3 operands
int inst9(char *opcode, char *arg1, char *arg2, char *arg3, char *arg4){

    int decoded_inst = 0x9000;

    isReg(arg1);
    int dr = getRegNumber(&arg1[1]);
    if(dr>7||dr<0){
      exit(4);
    }
    dr = dr << 9;
    decoded_inst = decoded_inst | dr;

    isReg(arg2);
    int sr1 = getRegNumber(&arg2[1]);
    if(sr1>7||sr1<0){
      exit(4);
    }
    sr1 = sr1 << 6;
    decoded_inst = decoded_inst | sr1;

    if(strcmp(opcode, "not") == 0){

        if((strcmp(arg1, "")==0) || (strcmp(arg2, "")==0) || (strcmp(arg3, "")!=0) || (strcmp(arg4, "")!=0)){
          exit(4);
        }

        decoded_inst = decoded_inst | 0x003F;
    }else if(arg3[0] == 'r'){

        if((strcmp(arg1, "")==0) || (strcmp(arg2, "")==0) || (strcmp(arg3, "")==0) || (strcmp(arg4, "")!=0)){
          exit(4);
        }
        
        isReg(arg3);
        int sr3 = getRegNumber(&arg3[1]);
        if(sr3>7||sr3<0){
          exit(4);
        }
        decoded_inst = decoded_inst | sr3;
    }else{
        
        if((strcmp(arg1, "")==0) || (strcmp(arg2, "")==0) || (strcmp(arg3, "")==0) || (strcmp(arg4, "")!=0)){
          exit(4);
        }

        decoded_inst = decoded_inst | (1<<5);
        int amount5 = toNum(arg3);
        if(amount5 > 15 || amount5 < -16){
          exit(3);
        }
        amount5 = amount5 & 0x001F;
        decoded_inst = decoded_inst | amount5;
    }
    return decoded_inst;
}//jmp/ret instruction
//1 operand unless ret
int inst12(char *opcode, char *arg1, char *arg2, char *arg3, char *arg4){
    int decoded_inst = 0xC000;

    if(strcmp(opcode, "ret")==0){
        if((strcmp(arg1, "")!=0) || (strcmp(arg2, "")!=0) || (strcmp(arg3, "")!=0) || (strcmp(arg4, "")!=0)){
          exit(4);
        }

        decoded_inst = decoded_inst | (7<<6);
    }else{

        if((strcmp(arg1, "")==0) || (strcmp(arg2, "")!=0) || (strcmp(arg3, "")!=0) || (strcmp(arg4, "")!=0)){
          exit(4);
        }

        isReg(arg1);
        int base_reg = getRegNumber(&arg1[1]);
        if(base_reg>7||base_reg<0){
          exit(4);
        }
        base_reg = base_reg << 6;
        decoded_inst = decoded_inst | base_reg;
    }
    return decoded_inst;

}//shft inst
//Error for Negative number in shift amount
// 3 Operands Expected
int inst13(char *opcode, char *arg1, char *arg2, char *arg3, char *arg4){

    if((strcmp(arg1, "")==0) || (strcmp(arg2, "")==0) || (strcmp(arg3, "")==0) || (strcmp(arg4, "")!=0)){
      exit(4);
    }

    int decoded_inst = 0xD000;

    isReg(arg1);
    int dr = getRegNumber(&arg1[1]);
    if(dr>7||dr<0){
      exit(4);
    }
    dr = dr << 9;
    decoded_inst = decoded_inst | dr;

    isReg(arg2);
    int sr = getRegNumber(&arg2[1]);
    if(sr>7||sr<0){
      exit(4);
    }
    sr = sr << 6;
    decoded_inst = decoded_inst | sr;

    if(strcmp(opcode, "lshf")==0){
        int amount4 = toNum(arg3);
        if(amount4 < 0){
          exit(4);
        }
        if(amount4 > 15){
          exit(3);
        }
        decoded_inst = decoded_inst | amount4;
    }else if(strcmp(opcode, "rshfl")==0){
        decoded_inst = decoded_inst | (1<<4);
        int amount4 = toNum(arg3);
        if(amount4 < 0){
          exit(4);
        }
        if(amount4 > 15){
          exit(3);
        }
        decoded_inst = decoded_inst | amount4;
    }else{
        decoded_inst = decoded_inst | (3<<4);
        int amount4 = toNum(arg3);
        if(amount4 < 0){
          exit(4);
        }
        if(amount4 > 15){
          exit(3);
        }
        decoded_inst = decoded_inst | amount4;
    }

    return decoded_inst;
}// lea instruction
//2 Operands
int inst14(char *opcode, char *arg1, char *arg2, char *arg3, char *arg4){

    if((strcmp(arg1, "")==0) || (strcmp(arg2, "")==0) || (strcmp(arg3, "")!=0) || (strcmp(arg4, "")!=0)){
      exit(4);
    }

    int decoded_inst = 0xE000;

    isReg(arg1);
    int dr = getRegNumber(&arg1[1]);
    if(dr>7||dr<0){
      exit(4);
    }
    dr = dr << 9;
    decoded_inst = decoded_inst | dr;

    int offset = get_label_offset(arg2);
    if(offset < -256 || offset > 255){
      exit(4);
      return -1;
    }
    offset = offset & 0x01FF;
    return decoded_inst | offset;
    

}//trap instruction
//1 Operand
int inst15(char *opcode, char *arg1, char *arg2, char *arg3, char *arg4){
  int decoded_inst = 0xF000;

  if((strcmp(arg1, "")==0) || (strcmp(arg2, "")!=0) || (strcmp(arg3, "")!=0) || (strcmp(arg4, "")!=0)){
    exit(4);
  }

  decoded_inst = decoded_inst | toNum(arg1);

  return decoded_inst;
}

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
        // else add to the program counter  )        /* hex     */
  
        if(strcmp(lOpcode, ".orig")==0){
          Orig = toNum(lArg1);
          if(Orig % 2 != 0){
            exit(3);
          }
          Program_Counter = Orig;
        }else{

          if(strcmp(lLabel,"")!=0){
            for(int x = 0; x <Table_Counter;x++){
              if(strcmp(lLabel,symbol_table[x].label)==0 || isOpcode(lLabel)==1){
                exit(4);
              }
            }
            struct table_element elem;
            elem.label = malloc(sizeof(char) * (strlen(lLabel)));
            strcpy(elem.label, lLabel);
            elem.label[strlen(lLabel)] = '\0';
            elem.address = Program_Counter;
            symbol_table[Table_Counter] = elem;
	    Table_Counter++;
          }

          Program_Counter++;
          Program_Counter++;
        }



	      
      }
    } while( lRet != DONE );


    rewind(infile);
    
    /* Do stuff with files */
    do{
       lRet = readAndParse( infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
       if( lRet != DONE && lRet != EMPTY_LINE ){
           Program_Counter++;
           Program_Counter++;
           int num_to_file;
           if(isOpcode(lOpcode)==1){
             if(strcmp(lOpcode,"add")==0){
               num_to_file = inst1(lOpcode, lArg1, lArg2, lArg3, lArg4);

          }else if(strcmp(lOpcode, "and")==0){
            num_to_file = inst5(lOpcode, lArg1, lArg2, lArg3, lArg4);
            
          }else if((strcmp(lOpcode, "br")==0)||(strcmp(lOpcode, "brp")==0)
          ||(strcmp(lOpcode, "brn")==0)||(strcmp(lOpcode, "brz")==0)
          ||(strcmp(lOpcode, "brnz")==0)||(strcmp(lOpcode, "brnp")==0)
          ||(strcmp(lOpcode, "brzp")==0)||(strcmp(lOpcode, "brnzp")==0)){
            num_to_file = inst0(lOpcode, lArg1, lArg2, lArg3, lArg4);

          }else if(strcmp(lOpcode, "jmp")==0){
            num_to_file = inst12(lOpcode, lArg1, lArg2, lArg3, lArg4);

          }else if(strcmp(lOpcode, "jsr")==0){
            num_to_file = inst4(lOpcode, lArg1, lArg2, lArg3, lArg4);

          }else if(strcmp(lOpcode, "jsrr")==0){
            num_to_file = inst4(lOpcode, lArg1, lArg2, lArg3, lArg4);

          }else if(strcmp(lOpcode, "ldb")==0){
            num_to_file = inst2(lOpcode, lArg1, lArg2, lArg3, lArg4);

          }else if(strcmp(lOpcode, "ldw")==0){
            num_to_file = inst6(lOpcode, lArg1, lArg2, lArg3, lArg4);

          }else if(strcmp(lOpcode, "lea")==0){
            num_to_file = inst14(lOpcode, lArg1, lArg2, lArg3, lArg4);

          }else if(strcmp(lOpcode, "not")==0){
            num_to_file = inst9(lOpcode, lArg1, lArg2, lArg3, lArg4);

          }else if(strcmp(lOpcode, "ret")==0){
            num_to_file = inst12(lOpcode, lArg1, lArg2, lArg3, lArg4);

          }else if(strcmp(lOpcode, "lshf")==0){
            num_to_file = inst13(lOpcode, lArg1, lArg2, lArg3, lArg4);

          }else if(strcmp(lOpcode, "rshfl")==0){
            num_to_file = inst13(lOpcode, lArg1, lArg2, lArg3, lArg4);

          }else if(strcmp(lOpcode, "rshfa")==0){
            num_to_file = inst13(lOpcode, lArg1, lArg2, lArg3, lArg4);

          }else if(strcmp(lOpcode, "stb")==0){
            num_to_file = inst3(lOpcode, lArg1, lArg2, lArg3, lArg4);

          }else if(strcmp(lOpcode, "stw")==0){
            num_to_file = inst7(lOpcode, lArg1, lArg2, lArg3, lArg4);

          }else if(strcmp(lOpcode, "trap")==0){
            num_to_file = inst15(lOpcode, lArg1, lArg2, lArg3, lArg4);

          }else if(strcmp(lOpcode, "xor")==0){
            num_to_file = inst9(lOpcode, lArg1, lArg2, lArg3, lArg4);

          }else if(strcmp(lOpcode,"nop")==0){
            num_to_file = 0;
          }
          else{
            // invalid opcode error
            exit(2);
          }
        }else{
          if(strcmp(lOpcode, ".orig")==0){
            Orig = toNum(lArg1);
            Program_Counter = Orig;
            num_to_file = toNum(lArg1);
          }else if(strcmp(lOpcode, ".end")==0){
            // do nothing
            num_to_file = -1;
            lRet = DONE;
          }else if(strcmp(lOpcode, ".fill")==0){
            if((strcmp(lArg1, "")==0) || (strcmp(lArg2, "")!=0) || (strcmp(lArg3, "")!=0) || (strcmp(lArg4, "")!=0)){
              exit(4);
            }
            num_to_file = toNum(lArg1);
          }else{
            //error
            exit(4);
          }
        }
        if(num_to_file != -1){
          fprintf( outfile, "0x%.4X\n", num_to_file);
        }
      }
    } while( lRet != DONE );



    fclose(infile);
    fclose(outfile);
}

int getRegNumber(char * pStr){
  if(strcmp(pStr, "0") == 0){
    return 0;
  }
  else if(strcmp(pStr, "1") == 0){
    return 1;
  }
  else if(strcmp(pStr, "2") == 0){
    return 2;
  }
  else if(strcmp(pStr, "3") == 0){
    return 3;
  }
  else if(strcmp(pStr, "4") == 0){
    return 4;
  }
  else if(strcmp(pStr, "5") == 0){
    return 5;
  }
  else if(strcmp(pStr, "6") == 0){
    return 6;
  }
  else if(strcmp(pStr, "7") == 0){
    return 7;
  }
  else{
    return -1;
  }
  
}

int
toNum( char * pStr )
{
  char * t_ptr;
  char * orig_pStr;
  int t_length,k;
  int lNum, lNeg = 0;          /* hex     */
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

  }else if((strcmp(ptr, "br")==0)||(strcmp(ptr, "brp")==0)
          ||(strcmp(ptr, "brn")==0)||(strcmp(ptr, "brz")==0)
          ||(strcmp(ptr, "brnz")==0)||(strcmp(ptr, "brnp")==0)
          ||(strcmp(ptr, "brzp")==0)||(strcmp(ptr, "brnzp")==0)){
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

void isReg(char * ptr){
  if(ptr[0]!= 'r'){
    exit(4);
  }
  if(strlen(ptr)>2){
    exit(4);
  }
  if(ptr[1]<'0' || ptr[1]>'7'){
    exit(4);
  }
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
