#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gadgets.h"
#include "utils.h"

Gadget createGadget(GadgetType type, char* assembly){
    char* assm = strdup(assembly);  // Make string writable
    char* line = strdup(assembly);
    char* opcode = strtok(line, " ");  // Peel off opcode
    char* operands = strtok(NULL, "");  // Save all operands
    char** operandList = malloc(3*20*sizeof(char));  // Max 3 operands at 20 chars each
    int numOperands = getGadgetOperands(operandList, operands);

    Gadget gadget = {
        type,
        assm,
        opcode,
        numOperands,
        operandList
        };
    return gadget;
}

Gadgets loadGadgets(){
    int numLoadConstGadgets, numArithOpGadgets, numMoveRegGadgets;
    const int max = 20;
    char line[max];
    FILE *f = fopen("gadgets.txt", "r");
    
    fscanf(f, "%d,%d,%d\n",&numLoadConstGadgets, &numArithOpGadgets, &numMoveRegGadgets);
    Gadget* loadConstGadgets = malloc(sizeof(Gadget) * numLoadConstGadgets);
    Gadget* arithOpGadgets = malloc(sizeof(Gadget) * numArithOpGadgets);
    Gadget* moveRegGadgets = malloc(sizeof(Gadget) * numMoveRegGadgets);
    
    Gadget* curr;
    int count;
    GadgetType type;
    while (!feof(f)){
        fgets(line, max , f);
        line[strcspn(line, "\n")] = '\0';

        if(strcmp(line,"") == 0){
            continue;
        }
        else if(strcmp(line,"loadConst") == 0){
            curr = loadConstGadgets;
            type = LOAD_CONST;
            count = 0;
        }
        else if(strcmp(line,"arithOp") == 0){
            curr = arithOpGadgets;
            type = ARITH_OP;
            count = 0;
        }
        else if(strcmp(line,"moveReg") == 0){
            curr = moveRegGadgets;
            type = MOVE_REG;
            count = 0;
        }
        else {
            curr[count] = createGadget(type, line);
            count++;
        }
    }
    fclose(f);

    Gadgets gadgets = {
        numLoadConstGadgets,
        loadConstGadgets,
        numArithOpGadgets,
        arithOpGadgets,
        numMoveRegGadgets,
        moveRegGadgets
    };

    return gadgets;
}
