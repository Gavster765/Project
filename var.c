#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "var.h"
#include "utils.h"

// Find variable by name
Var* findVar(char* name, Vars* vars) {
    if (name != NULL) {
        for (int i = 0 ; i < vars->count ; i++) {
            if (strcmp(vars->vars[i]->name, name) == 0) {
                return vars->vars[i];
            }
        }
    }
    return NULL;
}

// Find variable which currently occupies a register
Var* findVarByReg(char* reg, Vars* vars) {
    for (int i = 0 ; i < vars->count ; i++) {
        if (strcmp(vars->vars[i]->reg, reg) == 0) {
            return vars->vars[i];
        }
    }
    return NULL;
}

// Adds new var to vars
int addNewVar(Var* newVar, Vars* vars) {
    if (vars->count < vars->maxSize) {
        vars->vars[vars->count] = newVar;
        vars->count++;
        return 0;
    }
    else {
        printf("Ran out of variable space - allocate more\n");
        return -1;
    }
}

// Creates basic var and adds to vars
Var* addVar(char* name, Vars* vars) {
    Var *v = malloc(sizeof(Var) + strlen(name) + 1);
    v->lifeSpan = -1;
    v->loop = false;
    v->constant = true;
    v->inMemory = false;
    v->address = false;
    v->noKill = false;
    v->memAddress = vars->count + 1000;
    strcpy(v->reg, "new");
    strcpy(v->name, name);
    addNewVar(v, vars);
    return v;
}

Var* copyVar(char* name, Var* var, Vars* vars) {
    Var *v = malloc(sizeof(Var) + strlen(name) + 1);
    v->value = var->value;
    v->lifeSpan = var->lifeSpan;
    v->loop = var->loop;
    v->constant = var->constant;
    v->inMemory = var->inMemory;
    v->address = var->address;
    v->noKill = var->noKill;
    v->memAddress = var->memAddress;
    strcpy(v->reg, var->reg);
    strcpy(v->name, name);
    addNewVar(v, vars);
    return v;
}

int removeVar(Var* delVar, Vars* vars) {
    bool found = false;
    for (int i = 0 ; i < vars->count ; i++) {
        // printf("%d %d\n",i,vars->count);
        if(strcmp(vars->vars[i]->name, delVar->name) == 0) {
            if (i < vars->count - 1) {
                vars->vars[i] = vars->vars[i+1];
            }
            vars->count -= 1;
            found = true;
        }
        else if (found) {
            vars->vars[i] = vars->vars[i+1];
        }
    }
    free(delVar);
    if (found) return 0;
    else return -1;
}

// Updates the lifespan of var whenever it is referenced
void updateLifespan(char* name, Vars* vars, int currLine, bool loop) {
    Var* v = findVar(name, vars);
    if (v == NULL) {
        return;
    }
    else if (loop){
        v->loop = true;
    }
    else if (v->lifeSpan <= currLine) {
        v->lifeSpan = currLine+1;
    }
}

// Updates the lifespan of all vars to the end of current loop
void updateLoopVars(Vars* vars, int currLine) {
    for (int i = 0 ; i < vars->count ; i++) {
        Var* v = vars->vars[i];
        if (v->loop){
            v->lifeSpan = currLine+1;
            v->loop = false;
        }
    }
}

// Create an exact copy of every var
Vars* copyVars(Vars* vars){
    Vars* copy = malloc(sizeof(Vars) + sizeof(Var*)*vars->maxSize);
    copy->count = vars->count;
    copy->maxSize = vars->maxSize;
    for (int i = 0 ; i < vars->count ; i++){
        Var* var = vars->vars[i];
        Var *newVar = malloc(sizeof(Var) + strlen(var->name) + 1);
        strcpy(newVar->name, var->name);
        strcpy(newVar->reg, var->reg);
        newVar->value = var->value;
        newVar->lifeSpan = var->lifeSpan;
        newVar->loop = var->loop;  // Not used?
        newVar->constant = var->constant;
        newVar->inMemory = var->inMemory;
        newVar->memAddress = var->memAddress;
        newVar->address = var->address;
        newVar->noKill = var->noKill;

        copy->vars[i] = newVar;
    }
    return copy;
}

// Free all memory allocated to vars
void freeVars(Vars* vars) {
    for (int i = 0 ; i < vars->count ; i++){
        free(vars->vars[i]);
    }
    free(vars);
}

// Delete all vars no longer needed in registers
void deleteStaleVars(int line, Vars* vars) {
    for (int i = 0 ; i < vars->count ; i++) {
        Var* v = vars->vars[i];
        
        if (v->noKill) {
            continue;
        }
        if (v->lifeSpan == line || v->constant || v->inMemory){
            strcpy(v->reg, "new");
        }
        if (v->lifeSpan == line) {
            v->lifeSpan = -1;
        }
    }
}
