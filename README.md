# incanedible

## ผู้จัดทำ

ทีม 4ตัวบาท 3โลร้อย  
ภาควิชาวิศวกรรมคอมพิวเตอร์ คณะวิศวกรรมศาสตร์ มหาวิทยาลัยเกษตรศาสตร์  
รายชื่อสมาชิก :  
พฤทธิ์ อัศวพัชระ 6810503781  
กนกพัฒน์ เปาจีน 6810503323      
กษิติธร พิทยาภรณ์ 6810503374      
วรากร แก้วมณี 6810503897   

## ภาพรวมโครงงาน  
เป็นระบบสำหรับเช็คตำแหน่งผู้ใช้ไม้เท้า และการแจ้งเตือนการล้มของผู้พิการ พร้อมระบบเปิดไฟอัตโนมัติในตอนกลางคืน  

## Hardware
-  Buzzer 1 ชิ้น  
-  LED 5 ชิ้น  
-  GPS 1 ชิ้น  
-  LDR 1 ชิ้น  
-  esp32 s3 dev module 2 ชิ้น  
-  relay 1 channel 1 ชิ้น  
-  LED 3W 5V 20 mm 4 ชิ้น
-  Relay 1 Channel 1 ชิ้น
-  LDR 1 ชิ้น
-  Gyroscope 1 ชิ้น



## แผนผังการทำงาน  

![Flow](https://github.com/varakornpz/Incanedible-summary/blob/main/assets/flow.png)


### Wristband
ใช้ microcontroller อ่านค่า GPS sensor และส่งค่า latitude, longitude, sat(number of satellite) ไปให้ MQTT Broker ในรูปแบบ JSON

### Cane

ใช้ microcontroller อ่านค่าจาก Gyroscope และ LDR เพื่อสั่งการ Buzzer และ LED ตามลำดับ โดยมีหลักการในแต่ละส่วนดังนี้
- Gyroscope อ่านค่าความเร็วเชิงมุมของทั้งแกน X, Y, Z เพื่อคำนวนหาความเร็วเชิงมุมรวม ประกอบกับการอ่านค่าแรง g แล้วนำทั้งสองปัจจัยไปคำนวน หากทั้งสองถึงค่าที่กำหนดไว้ จะทำการสั่งให้ buzzer ทำงาน และส่งข้อมูลการล้มไปยัง MQTT Broker
- LDR วัดค่าแสง หากมีแสงน้อยกว่าที่กำหนด จะทำการสั่งเปิด LED

### Backend
ทำหน้าที่หลักๆคือการเป็นหลังบ้านในการยืนยันตัวตนผู้ใช้งานหน้า Dashboard และทำการ ส่ง/รับ ข้อมูลจาก MQTT broker ไปยัง Dashboard ดังนี้
- ยืนยันตัวตนผู้ใช้ โดยใช้ Google OAuth

- เป็น Hub ในการแจกจ่ายข้อมูลของ microcontroller โดยที่ข้อมูลที่ผู้ใช้ร้องขอจะไม่ได้อ่านโดยตรงจาก MQTT broker แต่จะใช้ระบบนี้(Backend) เป็นตัวกลางที่ทำหน้าที่ Subscibe เพียงหนึ่งครั้ง แล้วแจกจ่ายข้อมูลไปยัง Dashboard ผ่าน Websocket

- ส่งไลน์ Notify เมื่อได้รับข้อมูลจาก microcontroller ว่าผู้ใช้เกิดการล้ม
    ### FAQ
      Q: ทำไมต้องใช้ตัวกลางในการคุยกันระหว่าง Dashboard และ MQTT Broker
      A: ลดภาระ MQTT broker , สามารถควบคุมคำขอได้ว่าแต่ละคนมีสิทธ์ทำอะไรได้บ้าง , ผู้ใช้จะไม่รู้ว่าใช้ MQTT Broker อะไร มีรหัสผ่านอะไร

### Dashboard
เว็บไซต์แสดงข้อมูลต่างๆ และติดต่อกับผู้ใช้งาน โดยการดึงข้อมูลต่างๆจาก Backend

## File

Wristband  
firmware\wristband

Cane  
firmware\cane

Dashboard (another github repo)  
[Dashboard Repo](https://github.com/varakornpz/Incanedible-frontend)

Backend (another github repo)  
[Backend Repo](https://github.com/varakornpz/clickything-backend)

## Diagram

### Cane
![Cane diagram](https://github.com/varakornpz/Incanedible-summary/blob/main/assets/flow.png)

### Wristband
![Wristband diagram](https://github.com/varakornpz/Incanedible-summary/blob/main/assets/flow.png)


