## Giới thiệu
tinyShell là một trình giả lập dòng lệnh (shell) tối giản được phát triển phục vụ mục đích học tập về cơ chế quản lý tiến trình và hệ thống lệnh trong hệ điều hành Linux.

## Yêu cầu hệ thống
Để biên dịch và chạy chương trình, hệ thống của bạn cần đáp ứng các yêu cầu sau:
* **Hệ điều hành**: Các bản phân phối Linux.
* **Trình biên dịch**: GCC hỗ trợ tiêu chuẩn C++20.
* **Công cụ xây dựng**: CMake, build-essential.

## Hướng dẫn cài đặt và biên dịch
Thực hiện các bước sau theo thứ tự để thiết lập và xây dựng chương trình:

1. **Sao chép kho lưu trữ từ GitHub**:
   ```bash
   git clone [https://github.com/alpine1808/tinyShell.git](https://github.com/alpine1808/tinyShell.git)
   cd tinyShell
   ```
2. **Khởi tạo thư mục xây dựng và biên dịch**:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```
3. **Khởi động shell**:
   ```bash
   ./tinyshell
   ```
