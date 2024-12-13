# psu_for_satellite_1u
đây là bộ phân phối và theo dõi điện áp của vệ tinh cubesat 
1. ESP32 làm Access Point
ESP32 sẽ tạo một mạng Wi-Fi có tên là ESP32_Server và mật khẩu là 12345678. Khi thiết bị khác kết nối vào mạng này, ESP32 sẽ bắt đầu gửi dữ liệu qua HTTP server.
2. Gửi Dữ Liệu Cảm Biến dưới Dạng JSON
Khi client truy cập đường dẫn /data, ESP32 sẽ trả về dữ liệu cảm biến (Voltage, Current, Power) dưới dạng JSON. Đây là dữ liệu được gửi từ cảm biến INA219:
{
  "voltage": 12.5,
  "current": 50.2,
  "power": 600
}
3. Hiển Thị Dữ Liệu trên Giao Diện Web
•	Trang HTML có một bảng hiển thị Voltage, Current, và Power.
•	JavaScript sẽ sử dụng fetch để gọi API /data mỗi giây và cập nhật các ô trong bảng mà không cần tải lại trang.
4. Cập Nhật Thời Gian và Dữ Liệu Cảm Biến
•	Thời gian được lấy từ máy tính của người dùng và cập nhật mỗi giây.
•	Các giá trị cảm biến được lấy mỗi giây từ server ESP32 và cập nhật trực tiếp trên trang web.
