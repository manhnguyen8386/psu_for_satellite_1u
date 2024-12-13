#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

// Thông tin mạng Wi-Fi do ESP32 phát
const char* ssid = "ESP32_Server";
const char* password = "12345678";

// Tạo một server chạy trên cổng 80 (HTTP)
WiFiServer server(80);

void setup() {
  Serial.begin(115200);

  // Khởi tạo cảm biến INA219
  if (!ina219.begin()) {
    Serial.println("Không tìm thấy cảm biến INA219!");
    while (1) { delay(10); }
  }

  // Cấu hình ESP32 làm Access Point
  WiFi.softAP(ssid, password);
  Serial.println("Access Point đã khởi tạo!");
  Serial.print("Địa chỉ IP: ");
  Serial.println(WiFi.softAPIP());

  // Bắt đầu server
  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("Client mới đã kết nối!");
    String currentLine = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        currentLine += c;

        // Nếu client yêu cầu trang HTML
        if (currentLine.endsWith("\r\n\r\n")) {
          if (currentLine.indexOf("GET /data") >= 0) {
            // Trả về dữ liệu cảm biến dưới dạng JSON
            float busVoltage = ina219.getBusVoltage_V();
            float current = ina219.getCurrent_mA();
            float power = ina219.getPower_mW();

            String json = "{";
            json += "\"voltage\": " + String(busVoltage) + ",";
            json += "\"current\": " + String(current) + ",";
            json += "\"power\": " + String(power);
            json += "}";

            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: application/json");
            client.println("Connection: close");
            client.println();
            client.println(json);
          } else {
            // Trả về trang HTML
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<head>");
            client.println("<title>ESP32 Sensor Data</title>");
            client.println("<style>");
            client.println("table {width: 60%; border-collapse: collapse; margin: 20px auto;}");
            client.println("th, td {border: 1px solid #ddd; padding: 12px; text-align: center;}");
            client.println("th {background-color: #4CAF50; color: white;}");
            client.println("tr:nth-child(even) {background-color: #f2f2f2;}");
            client.println("tr:hover {background-color: #ddd;}");
            client.println("h1 {text-align: center;}");
            client.println("</style>");
            client.println("</head>");
            client.println("<body>");
            client.println("<h1>ESP32 Sensor Data</h1>");
            client.println("<table>");
            client.println("<tr><th>Date</th><th>Voltage (V)</th><th>Current (mA)</th><th>Power (mW)</th></tr>");
            client.println("<tr>");
            client.println("<td id='currentTime'></td>");
            client.println("<td id='voltage'>Loading...</td>");
            client.println("<td id='current'>Loading...</td>");
            client.println("<td id='power'>Loading...</td>");
            client.println("</tr>");
            client.println("</table>");

            client.println("<script>");
            client.println("function updateTime() {");
            client.println("  const now = new Date();");
            client.println("  const dateTimeString = now.toLocaleString();");
            client.println("  document.getElementById('currentTime').innerText = dateTimeString;");
            client.println("}");
            client.println("updateTime();");
            client.println("setInterval(updateTime, 1000);");

            client.println("function fetchData() {");
            client.println("  fetch('/data')");
            client.println("    .then(response => response.json())");
            client.println("    .then(data => {");
            client.println("      document.getElementById('voltage').innerText = data.voltage.toFixed(2);");
            client.println("      document.getElementById('current').innerText = data.current.toFixed(2);");
            client.println("      document.getElementById('power').innerText = data.power.toFixed(2);");
            client.println("    })");
            client.println("    .catch(error => console.error('Error fetching data:', error));");
            client.println("}");
            client.println("setInterval(fetchData, 1000);"); // Cập nhật dữ liệu mỗi giây
            client.println("</script>");

            client.println("</body>");
            client.println("</html>");
          }
          break;
        }
      }
    }
    // Đóng kết nối với client
    client.stop();
    Serial.println("Client đã ngắt kết nối!");
  }
}

