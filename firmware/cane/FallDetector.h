#ifndef FALLDETECTOR_H
#define FALLDETECTOR_H

#include <Wire.h>

// --- เกณฑ์สำหรับตรวจจับการล้ม (กระแทก) ---
#define ACCEL_FALL_THRESHOLD 0.6   // กระแทกแรง
#define GYRO_FALL_THRESHOLD 100.0  // หมุนเร็ว

// --- เกณฑ์สำหรับตรวจจับการขยับตัวหลังล้ม ---
#define GYRO_MOVEMENT_THRESHOLD 50.0 // ถ้าค่า Gyro เกินนี้แปลว่าพยายามลุกลงหรือขยับไม้เท้า
#define WAIT_TIME_AFTER_FALL 1000    // รอดูอาการ 5 วินาที (5000 มิลลิวินาที)

class FallDetector {
  public:
    bool begin(int sda_pin, int scl_pin);
    void update();
    bool isFalling();

  private:
    const int MPU_ADDR = 0x68;
    
    float gX, gY, gZ;
    float aX, aY, aZ;
    
    // --- ตัวแปรสำหรับเช็คสถานะการล้ม ---
    unsigned long fallStartTime = 0; 
    bool isWaitingForImmobility = false; // ใช้จำว่าตอนนี้กำลังรอดูอาการอยู่หรือไม่
};

#endif