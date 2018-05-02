 /**
* @mainpage ZumoBot Project
* @brief    You can make your own ZumoBot with various sensors.
* @details  <br><br>
    <p>
    <B>General</B><br>
    You will use Pololu Zumo Shields for your robot project with CY8CKIT-059(PSoC 5LP) from Cypress semiconductor.This 
    library has basic methods of various sensors and communications so that you can make what you want with them. <br> 
    <br><br>
    </p>
    
    <p>
    <B>Sensors</B><br>
    &nbsp;Included: <br>
        &nbsp;&nbsp;&nbsp;&nbsp;LSM303D: Accelerometer & Magnetometer<br>
        &nbsp;&nbsp;&nbsp;&nbsp;L3GD20H: Gyroscope<br>
        &nbsp;&nbsp;&nbsp;&nbsp;Reflectance sensor<br>
        &nbsp;&nbsp;&nbsp;&nbsp;Motors
    &nbsp;Wii nunchuck<br>
    &nbsp;TSOP-2236: IR Receiver<br>
    &nbsp;HC-SR04: Ultrasonic sensor<br>
    &nbsp;APDS-9301: Ambient light sensor<br>
    &nbsp;IR LED <br><br><br>
    </p>
    
    <p>
    <B>Communication</B><br>
    I2C, UART, Serial<br>
    </p>
*/

#include <project.h>
#include <stdio.h>
#include "Systick.h"
#include "Motor.h"
#include "Ultra.h"
#include "Nunchuk.h"
#include "Reflectance.h"
#include "I2C_made.h"
#include "Gyro.h"
#include "Accel_magnet.h"
#include "IR.h"
#include "Ambient.h"
#include "Beep.h"
#include <time.h>
#include <sys/time.h>

int rread(void);

/**
 * @file    main.c
 * @brief   
 * @details  ** Enable global interrupt since Zumo library uses interrupts. **<br>&nbsp;&nbsp;&nbsp;CyGlobalIntEnable;<br>
*/

//#if 1
//battery level//

#if 0
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    Systick_Start();
    struct sensors_ ref;
    struct sensors_ dig;
    
    ADC_Battery_Start();        

    int16 adcresult =0;
    float volts = 0.0;
    float Kp = 120, Kd = Kp/6, Ki = 0;
    
    int last_error = 0;
    int Sensor_max = 23770;
    int white_value = 3200;
    int max_speed = 80;
    int last_position, last_black;
    
    printf("\nBoot\n");
    
    //BatteryLed_Write(1); // Switch led on 
    //BatteryLed_Write(0); // Switch led off 
   
    bool led = false;
    
    uint32_t IR_val;
    IR_Start();
    reflectance_start();
    motor_start();
   // reflectance_set_threshold(5000, 5000, 15000, 15000, 5000, 5000); // set center sensor threshold to 11000 and others to 9000
    reflectance_read(&ref);
    
    
    /*if(ref.r1 >= 15000)
    {
        MotorDirRight_Write(0);
        PWM_WriteCompare2(200);
        
    }*/
    //postion = reflectance_read(&ref);
    
    
    //--------------Simple movement and turns to follow line without sensors(straight and 90 degree track)
    
   /* motor_forward(100,3300);
    
    motor_turn(235,28,500);     // turn
    motor_forward(100,2710);
    motor_turn(240,28,500);
    motor_forward(100,2850);
    motor_turn(200,35,800);
    motor_turn(180,100,1000);
    motor_turn(180,95,800);
    
    motor_forward(100,800);
    MotorDirLeft_Write(1);      // set LeftMotor backwards mode
    MotorDirRight_Write(0);     // set RightMotor forward mode
    PWM_WriteCompare1(200); 
    PWM_WriteCompare2(200);
    
    CyDelay(1700);
    //motor_forward(100,800);
    //motor_turn(200,105,2000);
    //motor_turn(200,75,2000);
    
    //motor_turn(50,200,2000);
    
    motor_stop();*/
    
    /*do {
    
        reflectance_digital(&dig);      //print out 0 or 1 according to results of reflectance period
        printf("%5d %5d %5d %5d %5d %5d \r\n", dig.l3, dig.l2, dig.l1, dig.r1, dig.r2, dig.r3);
        motor_forward(50,0);
        
    }
    while((dig.r3 + dig.l3)< 2);*/
       
    
    IR_wait(); // wait for IR command
    printf("IR command received\n");
    
    
    //----------Reading from the sensors following the line
    for(;;)
    {
         
         //motor_forward(200,0);
       
        // read raw sensor values
        
        reflectance_read(&ref);
        printf("%hu %hu %hu %hu %hu %hu\n", ref.l3, ref.l2, ref.l1, ref.r1, ref.r2, ref.r3);       // print out each period of reflectance sensors
        
        reflectance_digital(&dig);      //print out 0 or 1 according to results of reflectance period
        printf("%5d %5d %5d %5d %5d %5d \r\n", dig.l3, dig.l2, dig.l1, dig.r1, dig.r2, dig.r3);
        
        
        
        
        
        
        float error_left = (Sensor_max - ref.l1)/(Sensor_max - white_value);
        float error_right = (Sensor_max - ref.r1)/(Sensor_max - white_value);
        
        float last_errorleft = error_left;
        float last_errorRight = error_right;
        
        int right_speed = max_speed -(Kp * error_right); //+ Kd *(error_right -last_errorRight);
        int left_speed = max_speed -(Kp * error_left); //+ Kd *(error_left -last_errorleft) ;
        
        if (right_speed < 0) right_speed = max_speed + right_speed;
        if (left_speed < 0) left_speed = max_speed + left_speed;
        
       printf("\nright speed: %d , left_speed: %d\n", right_speed, left_speed);
       CyDelay(1000);
        
        
        #if 0
        int average_left = (ref.l1 + ref.l2)/2;
        int average_right = (ref.r1 + ref.r2)/2;
        int average_mid = (ref.l1 + ref.r1)/2;
        
        float error_left = (Sensor_max - average_left)/(Sensor_max - white_value);
        float error_right = (Sensor_max - average_right)/(Sensor_max - white_value);
        float error_mid = (Sensor_max - average_mid)/(Sensor_max - white_value);
        
        float last_errorleft = error_left;
        float last_errorRight = error_right;
        float last_errormid = error_mid;
        
        int right_speed = max_speed -(Kp * error_right); //+ Kd *(error_right -last_errorRight);
        int left_speed = max_speed -(Kp * error_left); //+ Kd *(error_left -last_errorleft) ;
        int mid_speed = max_speed -(Kp * error_mid); //+ Kd *(error_mid -last_errormid);
        #endif
        // for derivative
        //printf("\nright speed: %d , left_speed: %d\n", right_speed, left_speed);
       //CyDelay(1000);
       
      /* if(average_left < average_right)
        {
            */
            MotorDirLeft_Write(0);     // forward
            MotorDirRight_Write(0);
            PWM_WriteCompare1(left_speed); 
            PWM_WriteCompare2(right_speed);
            CyDelay(0);
            
          /* 
        }
        
        else if(average_left > average_right)
        {
            
            MotorDirLeft_Write(0);     
            MotorDirRight_Write(0);         // forward
            PWM_WriteCompare1(left_speed); 
            PWM_WriteCompare2(right_speed);
            CyDelay(0);
           
        }
        
           else if(average_mid > average_left && average_mid> average_right)
        {
            
            MotorDirLeft_Write(0);     // forward
            MotorDirRight_Write(0);
            PWM_WriteCompare1(mid_speed); 
            PWM_WriteCompare2(mid_speed);
            CyDelay(0);
           
        }
        */
        //CyDelay(100);
        //#endif
        
        /// using only digital value to follow the line
        
        #if 0
        if ((dig.l1 == 1 && dig.r1 == 1)) 
        {
            
           /* MotorDirLeft_Write(0);     // forward
            MotorDirRight_Write(0);
            PWM_WriteCompare1(200); 
            PWM_WriteCompare2(200);
            CyDelay(0);*/
            
            motor_forward(250,0);
            
        }
        
        else if (dig.l1 == 0 && dig.r1 == 1)
        {
            
            MotorDirLeft_Write(0);     
            MotorDirRight_Write(1);
            PWM_WriteCompare1(200); 
            PWM_WriteCompare2(198);
            CyDelay(0);
            
            //motor_turn(150,0,0);
            
        }
        
        else if (dig.l1 == 1 && dig.r1 == 0)
        {
            MotorDirLeft_Write(1);     
            MotorDirRight_Write(0);
            PWM_WriteCompare1(198); 
            PWM_WriteCompare2(200);
            CyDelay(0);
            //int motor_speed = 190;
           // motor_turn(0,150,0);
            
        }
        //#if 0
        #endif
        #if 0
        else if (dig.l3 == 1 )//|| dig.l2 == 1)
        {
            last_position = 0;    // remembers the last position as black  
            MotorDirLeft_Write(1);     
            MotorDirRight_Write(0);
            PWM_WriteCompare1(50); 
            PWM_WriteCompare2(100);
            CyDelay(0);
            
             //motor_turn(10,150,0);
            
        }
        
        else if (dig.r3 == 1)// || dig.r2 == 1)
        {
            last_position = 1;
            MotorDirLeft_Write(0);     
            MotorDirRight_Write(1);
            PWM_WriteCompare1(100); 
            PWM_WriteCompare2(50);
            CyDelay(0);
            
            //motor_turn(180,5,0);
            
        }
        
        //#endif
        
        else if(last_position == 1)
        {
            
            MotorDirLeft_Write(0);     
            MotorDirRight_Write(1);
            PWM_WriteCompare1(100); 
            PWM_WriteCompare2(50);
            CyDelay(0);
        }
        else if(last_position != 1)
        {
           
            MotorDirLeft_Write(1);     
            MotorDirRight_Write(0);
            PWM_WriteCompare1(50); 
            PWM_WriteCompare2(100);
            CyDelay(0);
        }
        #endif
        
         if(dig.l3 == 1 && dig.l2== 1 && dig.l1==1 && dig.r1==1 && dig.r2==1 && dig.r3==1)
        {
            motor_stop();
        
        }
        /*else if (dig.r1 == 0 && dig.l1 == 0)
        {
            last_position = 1;
            //PWM_WriteCompare1(0); 
            //PWM_WriteCompare2(0);
            //motor_turn(20,200,200);
        
        }*/
        //#endif
        
        /*else if (dig.r1 == 1 && dig.l1 == 1 && dig.r2 == 1 && dig.l2 == 1 && dig.l)
        {
           
            motor_turn(200,30,500);
        
        }*/
        /*if (dig.l1 == 0 && dig.r1 == 0)
        {
           
            motor_turn(100,30,300);
            
        
        }*/
        
        
        
        //for errors find proportional constant and derivative
        //
        
        // read digital values that are based on threshold. 0 = white, 1 = black
        // when blackness value is over threshold the sensors reads 1, otherwise 0
        //reflectance_digital(&dig);      //print out 0 or 1 according to results of reflectance period
        //printf("%5d %5d %5d %5d %5d %5d \r\n", dig.l3, dig.l2, dig.l1, dig.r1, dig.r2, dig.r3);        //print out 0 or 1 according to results of reflectance period
        
        
        //CyDelay(200);
        
    }
    
    //motor_start();
    //motor_forward(100,500);
     
    //move motors in opposite directions
    /*BatteryLed_Write(1);
    ShieldLed_Write(1);*/
    
    //MotorDirLeft_Write(1);      // set LeftMotor backwards mode
    //MotorDirRight_Write(0);     // set RightMotor forward mode
    /*PWM_WriteCompare1(100); 
    PWM_WriteCompare2(100);*/ 
#endif
    
 # if 0   
   for (int i = 0; i < 40; i++)  // change the duration of the motor
    {
        led = !led;
        BatteryLed_Write(led);
        ShieldLed_Write(!led);
        //Beep(100, 100);
        CyDelay(50);        // change the speed of the light 
    }
    
    for (int i = 0; i < 40; i++)  // change the duration of the motor
    {
        led = !led;
        BatteryLed_Write(led);
        ShieldLed_Write(!led);
        //Beep(100, 100);
        CyDelay(50);        // change the speed of the light 
    }
    
    
    //motor_forward(100,1000);
    
    //motor_stop();
    //BatteryLed_Write(0);
    //ShieldLed_Write(0);
    
    for(;;)
    {
        
        ADC_Battery_StartConvert();
        if(ADC_Battery_IsEndConversion(ADC_Battery_WAIT_FOR_RESULT)) {   // wait for get ADC converted value
            adcresult = ADC_Battery_GetResult16(); // get the ADC value (0 - 4095)
            // convert value to Volts
            // you need to implement the conversion
            volts = (float) (adcresult * 5) / 4096 * 1.5;
            
            // Print both ADC results and converted value
            printf("%d %f V\r\n",adcresult, volts);
            
        }
        
        if(volts < 4.0){
            Beep(100, 255);
            CyDelay(1000);
            ADC_Battery_Stop();
        }
        
        
        CyDelay(500);
        
    }
 }
#endif


#if 1
    /// PD controller
    
    void motor_go( float left_speed, float right_speed);
    float scale (float val, float min, float max);
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    Systick_Start();
    ADC_Battery_Start(); 
    motor_start();
    reflectance_start();
    IR_Start();
    Ultra_Start();
    
    
    struct sensors_ ref;
    struct sensors_ dig;
    int16 adcresult =0;
    float volts = 0.0;
    float Kp = 160,Kd = 1, Ki = 0;
    
    float last_error = 0;
    int max = 21000;
    int min = 4000;
    int max_speed =255;
    int last_position;
    float right_speed, left_speed;
    float error_l3,error_l2, error_l1,error_r1,error_r2,error_r3;
    float last_errorl3 = 0, last_errorl2 = 0, last_errorl1 = 0, last_errorr1 = 0, last_errorr2 = 0, last_errorr3 = 0;
    float last_errorleft, last_errorright, Error, last_errorl, last_errorr;
    float time = GetTicks(), dt=0;
    bool led = false;
    int Check_point = 1;
    /*do {
    
        reflectance_digital(&dig);      //print out 0 or 1 according to results of reflectance period
        printf("%5d %5d %5d %5d %5d %5d \r\n", dig.l3, dig.l2, dig.l1, dig.r1, dig.r2, dig.r3);
        motor_forward(50,0);
        
    }
    while((dig.r3 + dig.l3)< 2);*/
       
    
    /*IR_wait(); // wait for IR command
    printf("IR command received\n");*/
    
    /*if(SW1_Read() == 0) {
            motor_start();
            motor_forward(50,2000);
            led = !led;
            BatteryLed_Write(led);
            ShieldLed_Write(led);
            if(led) printf("Led is ON\n");
            else printf("Led is OFF\n");
            Beep(1000, 150);
            while(SW1_Read() == 0) CyDelay(10); // wait while button is being pressed
        }*/
    
    
    
    //----------Reading from the sensors following the line
    for(;;)
    {
        
        
         #if 0
        // read raw sensor values
        
        reflectance_read(&ref);
        reflectance_digital(&dig);
        printf(" dig value %5d %5d %5d %5d %5d %5d \r\n", dig.l3, dig.l2, dig.l1, dig.r1, dig.r2, dig.r3);
        //printf("%hu %hu %hu %hu %hu %hu\n", ref.l3, ref.l2, ref.l1, ref.r1, ref.r2, ref.r3);       // print out each period of reflectance sensors
        
        error_l3 = scale( ref.l3, min , max);
        error_l2 = scale( ref.l2, min , max);
        error_l1 = scale( ref.l1, min , max);
        error_r1 = scale( ref.r1, min , max);
        error_r2 = scale( ref.r2, min , max);
        error_r3 = scale( ref.r3, min , max);
        
        float error_left = error_l1 + error_l2 + error_l3;
        float error_right = error_r1 + error_r2 + error_r3;
        
        
        Error = error_left -error_right;
        dt = GetTicks() - time;
        
        last_errorl = (error_l1 - last_errorl)/dt;
        last_errorr = (error_r1 - last_errorr)/dt;
        last_error = (Error - last_error)/dt;
        time = GetTicks();
        
        
        printf("%f %f %f %f %f %f\n", error_l3, error_l2, error_l1, error_r1, error_r2, error_r3);       // print out each period of reflectance sensors
        right_speed = (3.1875*Kp * error_l1) + last_errorl * Kd;//(error_l1-last_errorl1)*Kd;// + (3.1875*Kp*error_l2-(error_l2-last_errorl2)*Kd); //+ 3*Kp*l3; //2*Kp * l2 + 3*Kp * l3; to get max speed
        left_speed = (3.1875*Kp * error_r1)+ last_errorr * Kd;//(error_r1-last_errorr1)*Kd;// + (3.1875*-Kp*error_r2-(error_r2-last_errorr2)*Kd); //+ 3*Kp*r3; //2*Kp*r2; // + Kp * 3*r3;
        
        if(right_speed > 255) right_speed = 255;
        if(left_speed > 255) left_speed = 255;
        if(left_speed < 0)  left_speed = 0;
        if(right_speed < 0)  right_speed = 0;
        /*right_speed = 255 + ((Kp * error_l1) + (Kd * last_errorl));
        left_speed =  255 - ((Kp * error_r1) + (Kd * last_errorr));
        
        if(right_speed > 255) right_speed = 255;
        if(left_speed > 255) left_speed = 255;
        if(left_speed < 0)  left_speed = 0;
        if(right_speed < 0)  right_speed = 0;*/
        
        //int speed = left_speed+right_speed;
        printf("\n%f \n%f\n", right_speed, left_speed);
        
        
        
        last_errorl =error_l1;
        last_errorr = error_r1;
        
        
        
        last_errorleft = error_left;
        last_errorright = error_right;
        
        
        last_errorl3 = error_l3;
        last_errorl2 = error_l2;
        last_errorl1 = error_l1;
        last_errorr1 = error_r1;
        last_errorr2 = error_r2;
        last_errorr3 = error_r3;
        
        
      
    /*    if((error_l3+error_l2+error_l1+error_r1+error_r2+error_r3) == 6.00)// || (error_l3+error_l2+error_l1+error_r1+error_r2+error_r3)>=5 )
        {
            if(Check_point ==1 || Check_point == 3)
            {
                printf("chk1");
                motor_go(0,0);
                Check_point +=1;
                CyDelay(000);
                IR_wait(); // wait for IR command
                printf("IR command received\n");
                
            }   else   { 
                Check_point+=1;
                printf("else loop counter\n");
            }
         
           }*/
        if((error_l3+error_l2+error_l1+error_r1+error_r2+error_r3) == 6.00){
        switch (Check_point){
            case 1: 
                    printf("case 1\n");
                    motor_go(0,0);
                    Check_point+=1;
                    IR_flush();
                    IR_wait();
                    
                    break;
             case 2:
                    printf("case 2\n");
                    motor_go(left_speed,right_speed);
                    Check_point+=1;
                    CyDelay(200);
                    break;
            case 3:
                    printf("case 3\n");
                    motor_stop();
                    break;
        }
        }
        /*else if((dig.l1 + dig.r1)>=2 && (dig.l2 + dig.r2 + dig.l3 + dig.r3)==0)
        {
            motor_forward(255,0);
        }*/
        
        else if((error_l1+ error_r1)>0.7)// && (dig.l3 + dig.r3)== 0) 
        {
            //motor_forward(200,0);
            motor_go(left_speed, right_speed);
        }
       
        //#if 0
        else if (dig.l3 == 1 && dig.r3 == 0 )//|| dig.l2 == 1)
        {
            last_position = 0;    // remembers the last position as black  
            MotorDirLeft_Write(1);     
            MotorDirRight_Write(0);
            PWM_WriteCompare1(100); // 90
            PWM_WriteCompare2(255); // 200
            CyDelay(0);
            
             //motor_turn(10,150,0);
            
        }
        
        else if (dig.r3 == 1 && dig.l3 == 0)// || dig.r2 == 1)
        {
            last_position = 1;
            MotorDirLeft_Write(0);     
            MotorDirRight_Write(1);
            PWM_WriteCompare1(255); 
            PWM_WriteCompare2(100);
            CyDelay(0);
            
            //motor_turn(180,5,0);
            
        }
       
       else if(last_position == 1)
        {
            printf("\nturn left\n");
            MotorDirLeft_Write(0);     
            MotorDirRight_Write(1);
            PWM_WriteCompare1(255); 
            PWM_WriteCompare2(100);
            CyDelay(0);
        }
        else if(last_position != 1)
        {
            printf("\nturn right\n");
            MotorDirLeft_Write(1);     
            MotorDirRight_Write(0);
            PWM_WriteCompare1(100); 
            PWM_WriteCompare2(255);
            CyDelay(0);
        }
        #endif
        
          CyDelay(1);  
        
        /////////////////////sumo ................
        
        reflectance_digital(&dig);
        printf(" dig value %5d %5d %5d %5d %5d %5d \r\n", dig.l3, dig.l2, dig.l1, dig.r1, dig.r2, dig.r3);
        
        float d = Ultra_GetDistance();
        //If you want to print out the value  
        printf("distance = %f\r\n", d);
        
        
         if((dig.l3 + dig.r3)> 1.2)
        {
            MotorDirLeft_Write(1);     
            MotorDirRight_Write(0);
            PWM_WriteCompare1(200); 
            PWM_WriteCompare2(200);
            CyDelay(300);
            
            motor_forward(200,100);
        
        }
        
         else if((dig.l3 == 0 && dig.r3 == 0) && d > 15)
        {
            MotorDirLeft_Write(0);     
            MotorDirRight_Write(0);
            PWM_WriteCompare1(150); 
            PWM_WriteCompare2(105);
            CyDelay(0);
        }
        
        else if (dig.l3 == 1)
        {
            MotorDirLeft_Write(0);     
            MotorDirRight_Write(1);
            PWM_WriteCompare1(200); 
            PWM_WriteCompare2(200);
            CyDelay(300);
            motor_forward(200,100);
        }
        
        else if (dig.r3 == 1)
        {
            MotorDirLeft_Write(1);     
            MotorDirRight_Write(0);
            PWM_WriteCompare1(200); 
            PWM_WriteCompare2(200);
            CyDelay(300);
            motor_forward(200,100);
        }
        if(d < 15){
            motor_forward(255,0);
        }
        
        /*while(1) {
        int d = Ultra_GetDistance();
        //If you want to print out the value  
        printf("distance = %d\r\n", d);
        
        CyDelay(200);
    }*/
        
    }
    
}
    
    void motor_go( float left_speed, float right_speed)
    {
        /*if(left_speed>255)  {left_speed=255;MotorDirLeft_Write(0); }
        else if(left_speed<0)  { left_speed=0;MotorDirLeft_Write(1); left_speed=left_speed*-1;}
        if(right_speed>255)  {right_speed=255; MotorDirRight_Write(0);}
        else if(right_speed<0)   {right_speed=0;MotorDirRight_Write(1); right_speed=right_speed*-1;}*/
        
            MotorDirLeft_Write(0);     // forward
            MotorDirRight_Write(0);
            PWM_WriteCompare1(left_speed); 
            PWM_WriteCompare2(right_speed);
            printf("\n%f %f\n", left_speed, right_speed);    // forward
            
    }
    
    float scale (float val, float min, float max)
    {
        if ( val < min)
        val = min;
        else if (val > max)
        val = max;
        
        return (val - min)/ (max - min);
    }

#endif

//button
#if 0
// button
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    Systick_Start();
    
    printf("\nBoot\n");

    //BatteryLed_Write(1); // Switch led on 
    BatteryLed_Write(0); // Switch led off 
    
    //uint8 button;
    //button = SW1_Read(); // read SW1 on pSoC board
    // SW1_Read() returns zero when button is pressed
    // SW1_Read() returns one when button is not pressed
    
    bool led = false;
    
    for(;;)
    {
        // toggle led state when button is pressed
        if(SW1_Read() == 0) {
            led = !led;
            BatteryLed_Write(led);
            ShieldLed_Write(led);
            if(led) printf("Led is ON\n");
            else printf("Led is OFF\n");
            Beep(1000, 150);
            while(SW1_Read() == 0) CyDelay(10); // wait while button is being pressed
        }        
    }
 }   
#endif

// ultra sonic
#if 0
//ultrasonic sensor//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    Systick_Start();
    Ultra_Start();                          // Ultra Sonic Start function
    while(1) {
        int d = Ultra_GetDistance();
        //If you want to print out the value  
        printf("distance = %d\r\n", d);
        CyDelay(200);
    }
}   
#endif

// IR
#if 0
//IR receiver//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    IR_Start();
    
    uint32_t IR_val; 
    
    printf("\n\nIR test\n");
    
    IR_flush(); // clear IR receive buffer
    printf("Buffer cleared\n");
    
    IR_wait(); // wait for IR command
    printf("IR command received\n");
    
    // print received IR pulses and their lengths
    for(;;)
    {
        if(IR_get(&IR_val)) {
            int l = IR_val & IR_SIGNAL_MASK; // get pulse length
            int b = 0;
            if((IR_val & IR_SIGNAL_HIGH) != 0) b = 1; // get pulse state (0/1)
            printf("%d %d\r\n",b, l);
            //printf("%d %lu\r\n",IR_val & IR_SIGNAL_HIGH ? 1 : 0, (unsigned long) (IR_val & IR_SIGNAL_MASK));
        }
    }    
 }   
#endif

//reflectancce
#if 0
//reflectance//
int main()
{
    struct sensors_ ref;
    struct sensors_ dig;

    Systick_Start();

    CyGlobalIntEnable; 
    UART_1_Start();
  
    reflectance_start();
    reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000
    

    for(;;)
    {
        // read raw sensor values
        reflectance_read(&ref);
        printf("%5d %5d %5d %5d %5d %5d\r\n", ref.l3, ref.l2, ref.l1, ref.r1, ref.r2, ref.r3);       // print out each period of reflectance sensors
        
        // read digital values that are based on threshold. 0 = white, 1 = black
        // when blackness value is over threshold the sensors reads 1, otherwise 0
        reflectance_digital(&dig);      //print out 0 or 1 according to results of reflectance period
        printf("%5d %5d %5d %5d %5d %5d \r\n", dig.l3, dig.l2, dig.l1, dig.r1, dig.r2, dig.r3);        //print out 0 or 1 according to results of reflectance period
        
        CyDelay(200);
    }
}   
#endif

//motor
#if 0
//motor//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();

    motor_start();              // motor start

    motor_forward(100,2000);     // moving forward
    motor_turn(200,50,2000);     // turn
    motor_turn(50,200,2000);     // turn
    motor_backward(100,2000);    // movinb backward
       
    motor_stop();               // motor stop
    
    for(;;)
    {

    }
}
#endif


/* [] END OF FILE */
