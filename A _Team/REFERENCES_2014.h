#include "hitechnic-gyro.h"


void print(tSensors x){
	nxtDisplayCenteredBigTextLine(3, "%d, %d", SensorValue[x]);
}
void gyroLeft(int x){//, int z){ //Uses gyro to accurately rotate. Integer x in this situation is the desired degree masurement to rotate
	float rotSpeed = 0;//Zero-ing roating speed. To be given a value soon
	float heading = 0;//Zero-ing heading.
	HTGYROstartCal(gyro);//Calibrate gyro
	time1[T1] = 0;//Clear + start degree calculation timer
	while(heading > (-x)){//While the robot is more than 9 degrees away from desired position, rotate at 100% speed
		motor[driveR] = -100;
		motor[driveL] = 100;
		if(time1[T1] >= 20){//Allow very small time to pass (dt)
			time1[T1]=0;//Clear + start degree calculation timer
   		rotSpeed = HTGYROreadRot(gyro);//Take current rotatoinal speed
 	  	heading += rotSpeed * 0.02;//Multiply by time in order to accurately find degrees rotated during that short interval of time (dt * speed)
    }
 	}
 	nxtDisplayCenteredTextLine(3, "%d, %d", heading);//Display current rotational position
}
//##########################################################################################################################
//##########################################################################################################################
void gyroRight(int x){//, int z){ //Uses gyro to accurately rotate. Integer x in this situation is the desired degree masurement to rotate
	float rotSpeed = 0;//Zero-ing roating speed. To be given a value soon
	float heading = 0;//Zero-ing heading.
	HTGYROstartCal(gyro);//Calibrate gyro
	time1[T1] = 0;//Clear + start degree calculation timer
	while(heading < x){//While the robot is more than 9 degrees away from desired position, rotate at 100% speed
		motor[driveR] = 100;
		motor[driveL] = -100;
		if(time1[T1] >= 20){//Allow very small time to pass (dt)
			time1[T1]=0;//Clear + start degree calculation timer
   		rotSpeed = HTGYROreadRot(gyro);//Take current rotatoinal speed
 	  	heading += rotSpeed * 0.02;//Multiply by time in order to accurately find degrees rotated during that short interval of time (dt * speed)
    }
 	}
 	nxtDisplayCenteredTextLine(3, "%d, %d", heading);//Display current rotational position
}
