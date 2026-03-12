# incanedible
 
<h1>ผู้จัดทำ</h1>
ทีม 4ตัวบาท 3โลร้อย<br>
ภาควิชาวิศวกรรมคอมพิวเตอร์ คณะวิศวกรรมศาสตร์ มหาวิทยาลัยเกษตรศาสตร์<br>
รายชื่อสมาชิก :<br>
พฤทธิ์ อัศวพัชระ 6810503781<br>  
กนกพัฒน์ เปาจีน 6810503323<br>    
กษิติธร พิทยาภรณ์ 6810503374<br>    
วรากร แก้วมณี 6810503897<br>  

ภาพรวมโครงงาน<br>
เป็นระบบสำหรับเช็คตำแหน่งผู้ใช้ไม้เท้า และการแจ้งเตือนการล้มของผู้การ พร้อมระบบเปิดไฟในตอนกลางคืน<br>  
โดยมีอุปกรณ์ที่ใช้ ได้แก่<br>  
-  Buzzer 1 ชิ้น<br>
-  LED 5 ชิ้น<br>
-  หน้าเว็บ Dashboard (Next.js)<br>
-  GPS 1 ชิ้น<br>
-  LDR 1 ชิ้น<br>
-  esp32s3 dev module 2 ชิ้น<br>
-  relay 1 channel 1 ชิ้น<br>



<h2>แผนผังการทำงาน</h2><br>
![Flow](https://github.com/varakornpz/Incanedible-summary/blob/main/assets/flow.png)

<h2>โครงสร้างไฟล์</h2>

ริสแบนด์<br>
firmware\wristband

ไม้เท้า<br>
firmware\cane

แดชบอร์ด (github repo แยก)<br>
[Dashboard Repo](https://github.com/varakornpz/Incanedible-frontend)

หลังบ้าน (github repo แยก)<br>
[ฺBackend Repo](https://github.com/varakornpz/clickything-backend)



รายละเอียดไฟล์ซอร์สโค้ด  
1. mpu6050linenotifylicensed.ino (บอร์ดที่ 1)
-----------------------------------------------------------  
หน้าที่หลัก:
- มี ldr วัดความสว่างเพื่อ LED
- อ่านค่าจากเซนเซอร์ MPU6050 (accelerometer + gyroscope)
- ประมวลผลข้อมูลจาก gps แปลงเป็น json
- แจ้งเตือน Buzzers เมื่อล้ม
- esp32s3 นำข้อมูลที่ได้จาก MPU6050 ผ่านการประมวลผล ส่งไปที่ MQTT ไปยัง golang แล้วไป dashboard ต่อ

Hardware ที่ใช้:
- ESP32S3
- LED 3W 5V 20 mm
- Buzzer
- relay
- ldr


2. newOLEDlicensed.ino (บอร์ดที่ 2)
-----------------------------------------------------------
หน้าที่หลัก:
- gps รับข้อมูลตำแหน่งจาก satellite 
- ประมวลผลข้อมูลจาก gps แปลงเป็น json
- esp32s3 นำข้อมูลที่ได้จาก gps ผ่านการประมวลผล ส่งไปที่ mqtt 


Hardware ที่ใช้:
- ESP32 (หรือบอร์ดที่รองรับ WiFi)
- Gy-neo 6m



แบบผังวงจร (Schematics)
ไฟล์ PDF ทั้งสองแบบออกแบบด้วย EasyEDA สามารถเปิดดูหรือแก้ไขต่อได้ 
ด้วยโปรแกรม EasyEDA หรือโปรแกรมดู PDF ทั่วไป

- Board 1 schematics.pdf : ผังวงจรบอร์ดหลัก (ESP32 + MPU6050 + Buzzer + Switch)
- Board 2 schematics.pdf : ผังวงจรบอร์ดแสดงผล (ESP32 + GPS)


การติดตั้งและใช้งาน

ความต้องการเบื้องต้น:
- golang (สำหรับระบบหลังบ้าน)
- Javascript runtime (สำหรับ Next.js Dashboard)
- Arduino IDE (สำหรับติดตั้ง firmware ลงใน microcontroller)

ขั้นตอนการติดตั้ง:

1. ติดตั้ง library / board manager ที่จำเป็นใน Arduino IDE
   - ติดตั้ง board manager รุ่นที่ต้องการ (เช่น esp32s3)
   - PubSubClient (สำหรับ MQTT)
   - TinyGPS++
   - WiFiClient
   - 

2. ตั้งค่า WiFi และ MQTT
   - แก้ไข SSID, Password ในไฟล์ .ino ทั้งสอง
   - ตั้งค่า MQTT Broker และ Topic ให้ตรงกัน

3. ตั้งค่า LINE Notify
   - ขอ Line Token จาก https://notify-bot.line.me
   - ใส่ Token ในไฟล์ mpu6050linenotifylicensed.ino

4. อัปโหลดโค้ด
   - อัปโหลด mpu6050linenotifylicensed.ino ไปยังบอร์ดที่ 1
   - อัปโหลด newOLEDlicensed.ino ไปยังบอร์ดที่ 2

5. ตั้งค่า Next.js (ถ้าต้องการ)
   - รับ MQTT และสร้าง Dashboard สำหรับแสดงสถานะ
