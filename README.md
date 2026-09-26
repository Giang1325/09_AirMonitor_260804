# Trạm Quan Trắc Chất Lượng Không Khí Đa Thông Số

Hệ thống quan trắc chất lượng không khí chi phí thấp, xây dựng trên nền tảng vi điều khiển ESP32, có khả năng đo đồng thời nồng độ bụi mịn (PM2.5, PM10), nhiệt độ, độ ẩm và áp suất; hiển thị trực tiếp trên màn hình OLED; đồng bộ thời gian thực; và truyền dữ liệu lên nền tảng IoT qua giao thức MQTT. Dự án còn xây dựng mô hình hiệu chuẩn cảm biến bằng hồi quy tuyến tính đa biến, đối chiếu với dữ liệu từ trạm quan trắc AQI tham chiếu.

Đây là bài tập lớn học phần **Kỹ thuật Vi xử lý (ET3300)**, Trường Điện – Điện tử, Đại học Bách Khoa Hà Nội.

## Thông tin thực hiện

| | |
|---|---|
| Giảng viên hướng dẫn | TS. Hàn Huy Dũng |
| Nhóm thực hiện | Nhóm 9 |
| Thành viên | Nguyễn Trường Giang (MSSV: 20233375)<br>Phan Anh Hào (MSSV: 20233386)<br>Chu Đức Nam (MSSV: 20233540) |
| Chương trình đào tạo | Kỹ thuật Điện tử – Viễn thông |
| Thời gian thực hiện | 2026 |

## Bối cảnh và mục tiêu

Ô nhiễm không khí, đặc biệt là bụi mịn PM2.5 và PM10, đang ảnh hưởng đáng kể đến môi trường sống tại các đô thị lớn. Các hệ thống quan trắc chuyên dụng hiện có số lượng hạn chế và chi phí đầu tư cao, chưa đáp ứng tốt nhu cầu giám sát tại quy mô nhỏ hoặc phục vụ nghiên cứu thực nghiệm. Dự án hướng đến xây dựng một thiết bị quan trắc vi mô có chi phí hợp lý, khả năng hoạt động độc lập và hỗ trợ thu thập dữ liệu liên tục.

## Kiến trúc hệ thống

Hệ thống được tổ chức theo mô hình thu thập – xử lý – xuất dữ liệu, với ESP32 làm khối xử lý trung tâm tiếp nhận dữ liệu từ các cảm biến, đồng bộ thời gian, đóng gói thành cấu trúc dữ liệu thống nhất, sau đó phân phối đến màn hình hiển thị và nền tảng IoT.

| STT | Linh kiện | Khối chức năng |
|---|---|---|
| 1 | ESP32-WROOM-32 | Khối xử lý trung tâm |
| 2 | PMS5003 | Cảm biến bụi (UART) |
| 3 | BME680 | Cảm biến môi trường: nhiệt độ, độ ẩm, áp suất, VOC (I2C) |
| 4 | RTC DS3231 | Đồng bộ thời gian thực |
| 5 | Màn hình OLED 0.96" | Hiển thị dữ liệu |
| 6 | Module UPS IP5328 + Pin Li-ion 18650 | Khối nguồn, đảm bảo hoạt động liên tục khi mất nguồn ngoài |

Firmware được thiết kế theo cơ chế **lập lịch không chặn (non-blocking scheduling)** nhằm xử lý đồng thời nhiều giao tiếp ngoại vi (UART, I2C) mà không làm gián đoạn các tác vụ khác — một yêu cầu quan trọng đối với hệ thống nhúng thời gian thực có nhiều nguồn dữ liệu.

## Truyền và lưu trữ dữ liệu

Dữ liệu được truyền theo thời gian thực qua Wi-Fi bằng giao thức **MQTT** lên nền tảng **Adafruit IO** để phục vụ giám sát từ xa, đồng thời được lưu cục bộ dưới định dạng CSV phục vụ phân tích và hiệu chuẩn sau này.

## Mô hình hiệu chuẩn cảm biến

Bên cạnh việc thu thập dữ liệu, nhóm xây dựng quy trình hiệu chuẩn cảm biến bụi PMS5003 bằng mô hình hồi quy **Full Quadratic** kết hợp chuẩn hóa Z-score, sử dụng các tham số nhiệt độ, độ ẩm và áp suất, đối chiếu với dữ liệu từ trạm quan trắc AQI tham chiếu.

**Kết quả trước và sau hiệu chuẩn:**

**PM2.5**

| Chỉ số | Trước hiệu chuẩn | Sau hiệu chuẩn | Cải thiện |
|---|---|---|---|
| R² | −9.18 | 0.845 | Giải thích ~84.5% biến thiên dữ liệu tham chiếu |
| RMSE (µg/m³) | 13.69 | 1.69 | Giảm ~87.7% |
| MAE (µg/m³) | 10.68 | 1.32 | Giảm ~87.6% |

**PM10**

| Chỉ số | Trước hiệu chuẩn | Sau hiệu chuẩn | Cải thiện |
|---|---|---|---|
| R² | −7.35 | 0.530 | Cải thiện rõ rệt khả năng dự đoán |
| RMSE (µg/m³) | 16.72 | 3.97 | Giảm ~76.3% |
| MAE (µg/m³) | 14.44 | 2.53 | Giảm ~82.5% |

Kết quả thực nghiệm với tổng thời gian đo xấp xỉ 100 giờ cho thấy hệ thống duy trì hoạt động ổn định, không xảy ra hiện tượng treo hoặc gián đoạn đáng kể trong suốt quá trình vận hành.

## Mức độ hoàn thành mục tiêu

| Nội dung | Kết quả |
|---|---|
| Đo nồng độ bụi PM2.5 và PM10 | Đạt |
| Đo nhiệt độ, độ ẩm và áp suất môi trường | Đạt |
| Hiển thị dữ liệu trên màn hình OLED | Đạt |
| Đồng bộ thời gian bằng RTC DS3231 | Đạt |
| Truyền dữ liệu lên nền tảng IoT qua MQTT | Đạt |
| Thiết kế và chế tạo PCB | Đạt |
| Thu thập dữ liệu thực nghiệm | Đạt |
| Đánh giá với dữ liệu tham chiếu AQI | Đạt |
| Lưu dữ liệu vào thẻ nhớ MicroSD | Chưa đạt |
| Hiệu chuẩn dữ liệu bằng hồi quy đa biến | Bước đầu, chưa hoàn thiện |

Nhóm đánh giá khách quan: mô hình hiệu chuẩn hiện được xây dựng trên tập dữ liệu thu thập trong khoảng thời gian còn hạn chế, cần mở rộng dữ liệu ở nhiều điều kiện môi trường hơn để nâng cao khả năng tổng quát hóa trong các nghiên cứu tiếp theo.

## Công nghệ sử dụng

- **Vi điều khiển:** ESP32 (lập trình trên nền tảng Arduino framework)
- **Ngôn ngữ:** C/C++
- **Giao tiếp ngoại vi:** UART, I2C
- **Giao thức truyền thông:** Wi-Fi, MQTT (Adafruit IO)
- **Công cụ thiết kế phần cứng:** Altium Designer (sơ đồ nguyên lý, layout PCB), Proteus
- **Xử lý và hiệu chuẩn dữ liệu:** Hồi quy tuyến tính đa biến (Full Quadratic), chuẩn hóa Z-score

## Hướng phát triển

- Hoàn thiện cơ chế lưu trữ dữ liệu vào MicroSD và bổ sung các cơ chế giám sát lỗi (Watchdog Timer, tự động khôi phục kết nối MQTT)
- Mở rộng thu thập dữ liệu ở nhiều điều kiện môi trường để nâng cao độ tin cậy của mô hình hiệu chuẩn; thử nghiệm thêm các mô hình học máy khác (Random Forest, Gradient Boosting, XGBoost)
- Phát triển chức năng cập nhật firmware từ xa (OTA) và dashboard giám sát trực quan
- Mở rộng triển khai nhiều node đo kết nối về một máy chủ trung tâm, hướng tới mạng lưới quan trắc quy mô nhỏ
- Sau khi hoàn thiện kiến thức nền tảng về ngắt (interrupt), bộ định thời (timer) và các ngoại vi trên phần cứng thật, tôi dự kiến tự triển khai lại phần điều khiển của hệ thống theo hướng lập trình thanh ghi trực tiếp (bare-metal), nhằm hiểu sâu hơn về kiến trúc và cơ chế vận hành của vi điều khiển ESP32.

## Ghi chú cấu hình

File firmware sử dụng các thông tin WiFi và khóa API dưới dạng placeholder (`YOUR_WIFI_SSID`, `YOUR_WIFI_PASSWORD`, `YOUR_ADAFRUIT_IO_USERNAME`, `YOUR_ADAFRUIT_IO_KEY`). Cần thay bằng thông tin thực tế trước khi biên dịch và nạp vào thiết bị.
