#include<stdio.h>
#include<math.h>

#define pi 3.1415
int main(){

		printf("%f\n%f\n%f\n%f\n", atan2(-4.0,0.0)*180.0/pi, atan2(-3.0,-3.0)*180.0/pi, floor(-(atan2(-3.0,2.0)*180.0/pi)/30.0), atan2(2.0,5.0)*180.0/pi);

		printf("%d", ((int)floor((146.0)/60.0)+5)%6);


	return 0;
}
