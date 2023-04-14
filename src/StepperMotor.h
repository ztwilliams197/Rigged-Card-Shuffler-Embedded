/*
 * StepperMotor.h
 *
 *  Created on: Apr 14, 2023
 *      Author: ztwil
 */

#ifndef STEPPERMOTOR_H_
#define STEPPERMOTOR_H_

#define NUM_BINS 52
#define STEPS_PER_ROT 200

int current_bin;

int getNumSteps(int bin);
int numStepsBetweenBins(int to_bin, int dir);


#endif /* STEPPERMOTOR_H_ */
