#include "FallDetector.h"
#include <Wire.h>
#include <math.h>

bool FallDetector::begin(int sda_pin, int scl_pin) {
  Wire.begin(sda_pin, scl_pin);
  
  // ตรวจสอบว่าพบเซ็นเซอร์หรือไม่
  Wire.beginTransmission(MPU_ADDR);
  if (Wire.endTransmission() != 0) {
    return false; // ไม่พบเซ็นเซอร์
  }
  
  // ปลุก MPU6050 จาก Sleep Mode
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission(true);
  
  return true; // พบเซ็นเซอร์และพร้อมทำงาน
}

void FallDetector::update() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // เริ่มอ่านตั้งแต่ Register ของ Accelerometer
  
  if (Wire.endTransmission(false) != 0) {
    gX = 0; gY = 0; gZ = 0; 
    aX = 0; aY = 0; aZ = 0; 
    return;
  }
  
  // ร้องขอข้อมูล 14 ไบต์ (Accel 6 + Temp 2 + Gyro 6)
  uint8_t bytesReceived = Wire.requestFrom(MPU_ADDR, 14, true);
  
  if (bytesReceived == 14) {
    // อ่านค่าความเร่ง (Accelerometer)
    int16_t raw_ax = (Wire.read() << 8) | Wire.read();
    int16_t raw_ay = (Wire.read() << 8) | Wire.read();
    int16_t raw_az = (Wire.read() << 8) | Wire.read();
    
    // ข้ามค่าอุณหภูมิ 2 ไบต์
    Wire.read(); Wire.read(); 
    
    // อ่านค่าการหมุน (Gyroscope)
    int16_t raw_gx = (Wire.read() << 8) | Wire.read();
    int16_t raw_gy = (Wire.read() << 8) | Wire.read();
    int16_t raw_gz = (Wire.read() << 8) | Wire.read();
    
    // แปลงค่า Accel ให้อยู่ในหน่วย g (หาร 16384 ตามสเปคตั้งต้นของ MPU6050 ที่ +/- 2g)
    aX = raw_ax / 16384.0;
    aY = raw_ay / 16384.0;
    aZ = raw_az / 16384.0;

    // แปลงค่า Gyro ให้อยู่ในหน่วย องศา/วินาที
    gX = raw_gx / 131.0;
    gY = raw_gy / 131.0;
    gZ = raw_gz / 131.0;
  } else {
    // 4. กรณีข้อมูลมาไม่ครบ (กระแทกแล้วสายสั่น)
    gX = 0; gY = 0; gZ = 0;
    
    // สั่งปลุก (Reset) เซ็นเซอร์ใหม่ เผื่อวงจรภายในค้าง
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x6B);
    Wire.write(0x00);
    Wire.endTransmission(true);
  }
}

bool FallDetector::isFalling() {
  // คำนวณค่าแรงกระแทกและการหมุน
  float accelMagnitude = sqrt((aX * aX) + (aY * aY) + (aZ * aZ));
  float gyroMagnitude = sqrt((gX * gX) + (gY * gY) + (gZ * gZ));
  
  // สถานะที่ 1: ปกติ -> ตรวจหาแรงกระแทก (ล้ม)
  if (!isWaitingForImmobility) {
    if (accelMagnitude > ACCEL_FALL_THRESHOLD && gyroMagnitude > GYRO_FALL_THRESHOLD) {
      // ตรวจพบการกระแทกแรงๆ! เข้าสู่โหมดรอดูอาการ
      isWaitingForImmobility = true;
      fallStartTime = millis(); // จดเวลาที่ล้มไว้
    }
  } 
  // สถานะที่ 2: เพิ่งล้มไป -> รอดูอาการ 5 วินาที ว่าขยับตัวไหม
  else {
    // ถ้ามีการขยับตัว (ค่า Gyro สูงขึ้นกว่าเกณฑ์ขยับตัว)
    if (gyroMagnitude > GYRO_MOVEMENT_THRESHOLD) {
      // แปลว่าคนใช้งานลุกขึ้นมาแล้ว หรือเก็บไม้เท้าขึ้นมาถือได้เอง
      isWaitingForImmobility = false; // ยกเลิกการล้ม
      fallStartTime = 0;
      return false; 
    }
    
    // ถ้าไม่มีการขยับตัวเลย และเวลาผ่านไปครบ 5 วินาทีแล้ว
    if (millis() - fallStartTime > WAIT_TIME_AFTER_FALL) {
      isWaitingForImmobility = false; // รีเซ็ตสถานะ
      fallStartTime = 0;
      return true; // คืนค่า true ยืนยันว่าล้มและไม่ขยับตัว (ส่งแจ้งเตือนได้เลย!)
    }
  }
  
  // ถ้ายังอยู่ในช่วง 5 วินาที หรือไม่ได้ล้ม ให้คืนค่า false ไปก่อน
  return false;
}