#ifndef VAR_H
#define VAR_H

#include <stdbool.h>

// Consider changing data structure
typedef struct Var {
    int value;
    int lifeSpan;  // from this line on don't need var
    bool loop;  // used inside loop so don't kill until loop end
    char reg[4];
    char name[];
} Var;

typedef struct Vars {
    int count;
    Var* vars[];
} Vars;

Var* findVar(char* name, Vars* vars);
Var* findVarByReg(char* reg, Vars* vars);

void updateLifespan(char* name, Vars* vars, int currLine, bool loop);
void updateLoopVars(Vars* vars, int currLine);

Vars* copyVars(Vars* vars);
void freeVars(Vars* vars);
void deleteStaleVars(int line, Vars* vars);

#endif /* VAR_H */