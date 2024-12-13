#include <Wire.h>  
#include <Adafruit_INA219.h>  
#include <WiFi.h>  
#include <HTTPClient.h>  

const char* ssid = "Nha 10 - Tang 5"; //Tên wifi sẽ kết nối
const char* password = "0343726666"; //Mật khẩu wifi
  
const char* serverName = "https://script.google.com/macros/s/AKfycbwGToRu3b3ooggIlQwbSPQM0rqZ0NS9sPRQ3NtjsorT6sq43x3GHTy_ScyPqI2IhwmF8Q/exec"; // Thay bằng URL của bạn  

Adafruit_INA219 ina219; // Khai báo biến INA219  

void setup() {  
    Serial.begin(115200);  
    Wire.begin();  
    ina219.begin();  

    WiFi.begin(ssid, password);  
    while (WiFi.status() != WL_CONNECTED) {  
        delay(1000);  
        Serial.println("Connecting to WiFi...");  
    }  
    Serial.println("Connected to WiFi");  
    Serial.println(WiFi.localIP());  
    
    if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
    }
}  

void loop() {    
    // Lấy giá trị dòng điện và điện áp  
    float current = ina219.getCurrent_mA();  
    float voltage = ina219.getBusVoltage_V();  
    float power = ina219.getPower_mW();

    Serial.println("voltage = " + String(voltage));
    Serial.println("current = " + String(current));
    Serial.println("power = " + String(power));

    if (WiFi.status() == WL_CONNECTED) {  
        HTTPClient http;  

        String httpRequestData = "voltage=" + String(voltage) + "&current=" + String(current) + "&power=" + String(power);  
        
        // Gửi yêu cầu POST  
        http.begin(serverName); // Khởi tạo kết nối  
        http.addHeader("Content-Type", "application/x-www-form-urlencoded"); // Thiết lập tiêu đề  
        int httpResponseCode = http.POST(httpRequestData); // Gửi dữ liệu  

        if (httpResponseCode > 0) {  
            String response = http.getString();  
            Serial.print("Response code: ");  
            Serial.println(httpResponseCode);  
            Serial.println("Response: " + response);  
        } else {  
            Serial.print("Error on sending POST: ");  
            Serial.println(httpResponseCode); // In mã lỗi  
        }  
        http.end(); // Kết thúc kết nối  
    } else {  
        Serial.println("WiFi Disconnected");  
    }  
    delay(10000); // Giữ lại mỗi 10 giây  
}