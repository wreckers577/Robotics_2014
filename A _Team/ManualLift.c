#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTMotor,  HTServo)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Motor,  mtr_S1_C1_1,     driveL,        tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C1_2,     driveR,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_1,      ,             tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_2,     lift,          tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_1,     spinner,       tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_2,     motorI,        tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S1_C4_1,    spinnerA,             tServoStandard)
#pragma config(Servo,  srvo_S1_C4_2,    wingL,                tServoStandard)
#pragma config(Servo,  srvo_S1_C4_3,    wingR,                tServoStandard)
#pragma config(Servo,  srvo_S1_C4_4,    ramp,                 tServoContinuousRotation)
#pragma config(Servo,  srvo_S1_C4_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C4_6,    block,                tServoStandard)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

task main()
{
	while(true){
		if(nNxtButtonPressed == 2)
		{
			motor[lift] = 100;
		}
		if(nNxtButtonPressed == 1)
		{
			motor[lift] = -100;
		}
		if(nNxtButtonPressed == -1)
		{
			motor[lift] = 0;
		}
	}
}
