#include <stdbool.h>
#include <inttypes.h>
#include "var.h"
#include "pseudo.h"

void removeChars(char* str, char c);
void removeLeadingSpaces(char** line); 

int getGadgetOperands(char** operandList, char* operandString);
int getOperands(char** operandList, char* operandString);
int64_t getVarValue(char* valStr);

int getProgLines(char** progList, char* progString);

bool used(char* reg, char** usedRegs, int count);

char** usedRegisters(Vars* vars);
void freeUsedRegs(char** usedRegs, int count);
void addRegToUsed(char** usedRegs, char* reg, int count);
void removeRegFromUsed(char** usedRegs, char* reg, int count);

bool checkArithOp(char* opcode);
bool checkArithOpGadget(char opcode, char* gadget);
void fillArithOp(ArithOp* arithOp, char* opcode);

bool checkSpecialOp(char* opcode);
bool checkSpecialOpGadget(char opcode, char* gadget);
void fillSpecialOp(Special* special, char* opcode);

void freePseudo(int progLines, Pseudo* pseudoInst);
