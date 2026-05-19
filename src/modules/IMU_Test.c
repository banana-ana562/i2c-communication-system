/*
 * IMU_Test.c
 *
 * Created on: April 29, 2026
 * Authors: Ana Arante, Emily Hsu, Suphia Sidiqi
 *
 * Measure 3-axis acceleration and 3-axis rotation rate; compute tilt angles
 *
 */

#include "src/drivers/MPU6050.h"
#include "src/drivers/wtimer.h" // for DELAY_1MS
#include "src/modules/IMU_Test.h" // for IMU_Test()

#if defined(IMU_TEST)
void IMU_Test(void){

    MPU6050_ACCEL_t accel;
    MPU6050_GYRO_t gyro;
    MPU6050_ANGLE_t angle;
    char buf[120];

    MPU6050_Get_Accel(&accel);
    MPU6050_Process_Accel(&accel);

    MPU6050_Get_Gyro(&gyro);
    MPU6050_Process_Gyro(&gyro);

    MPU6050_Get_Angle(&accel, &gyro, &angle);

    sprintf(buf,
            "RAW Ax=%d Ay=%d Az=%d | g Ax=%.2f Ay=%.2f Az=%.2f\r\n",
            accel.Ax_RAW, accel.Ay_RAW, accel.Az_RAW,
            accel.Ax, accel.Ay, accel.Az);
    UART0_OutString(buf);

    sprintf(buf,
            "Angle X=%.2f Y=%.2f Z=%.2f\r\n\r\n",
            angle.ArX, angle.ArY, angle.ArZ);
    UART0_OutString(buf);

    DELAY_1MS(100);   // refresh within 100 ms
}
#endif