/*
 * StepperMotor.c
 *
 *  Created on: Apr 14, 2023
 *      Author: ztwil
 */
#include "StepperMotor.h"

int getNumSteps(int bin) {
    static int bin_to_steps[NUM_BINS] = {0, 4, 8, 11, 15, 19, 23, 27, 31, 35, 38, 42, 46, 50, 54, 58, 61, 65, 69, 73, 77, 81, 85, 88, 92, 96, 100, 104, 108, 111, 115, 119, 123, 127, 131, 135, 138, 142, 146, 150, 154, 158, 161, 165, 169, 173, 177, 181, 185, 188, 192, 196};
    return bin_to_steps[bin];
}

int numStepsBetweenBins(int to_bin, int dir) { // DIR = 1 is Clockwise
    if(dir) {
        int steps = getNumSteps(to_bin) - getNumSteps(current_bin);
        return steps >= 0 ? steps : 200 + steps;
    } else {
        int steps = (200 + getNumSteps(current_bin)) - getNumSteps(to_bin);
        return steps < 200 ? steps : steps - 200;
    }
}
