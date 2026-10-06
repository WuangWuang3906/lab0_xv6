# Quy tắc cho AI trong lab06_xv6

## Mục tiêu và nguồn yêu cầu

- Đây là Lab0 / Project 0 - System Call. Chỉ làm `trace` theo `docs/assignment/Lab0_System_Call.pdf`.
- Đọc `docs/lab0-overview.md`, `docs/work-plan.md` và Issue được giao trước khi sửa.
- PDF gốc có ưu tiên cao hơn phần tóm tắt. Link MIT là tài liệu tham khảo. Không tự thêm PID vào log, kế thừa mask qua fork, sysinfo hay hello vào phạm vi bài.
- Nếu một yêu cầu mới của người dùng/thầy thay đổi phạm vi, nêu rõ thay đổi và cập nhật tài liệu liên quan. Không bịa yêu cầu còn thiếu.

## Phạm vi file và giao diện đã chốt

- Quang: `kernel/syscall.c`; nhánh `wuang`.
- Dũng: `user/trace.c`, `user/user.h`, `user/usys.pl`, `Makefile`.
- Khanh: `kernel/syscall.h`, `kernel/proc.h`, `kernel/proc.c`, `kernel/sysproc.c`.
- Giao diện chung: `SYS_trace` bằng 22, user `int trace(int mask)`, kernel `uint64 sys_trace(void)`, trường process `int tracemask`.
- Log theo PDF: `syscall <name> -> <return value>` và xuống dòng. Mask thuộc process gọi; giữ qua exec; reset khi cấp/tái sử dụng process. Không copy mask sang con trong phạm vi hiện tại.
- Chỉ sửa file thuộc Issue đang làm. Muốn sửa file của người khác phải nêu phụ thuộc và để nhóm thống nhất trước.
- Repo mới được tạo là bộ khung. Các TODO của người khác chưa hoàn thành là trạng thái dự kiến, không phải lý do để AI làm luôn cả Lab0.

## Quy tắc code

- Giữ cấu trúc xv6 ở gốc repo và style của file hiện có; không refactor ngoài bài.
- Chỉnh stub tại `user/usys.pl`; không chỉnh `user/usys.S` được sinh tự động.
- Starter đã có `user/trace.c`; đọc và kiểm tra nó trước khi sửa. Chú ý giới hạn MAXARG, phần tử NULL kết thúc argv và xử lý lỗi exec.
- Trong source này `argint()` trả về `void`. Hàm xử lý syscall kernel trả về `uint64`; không áp dụng máy móc prototype từ snapshot khác.
- Giữ `README` gốc, `user/grep.c`, các syscall đọc file và bộ chấm MIT. Không sửa chúng để ép output khớp mẫu.
- Các dòng Makefile cần tab thật. Dùng LF; giữ license/acknowledgments gốc.
- Xóa TODO của phần đã hoàn thiện; không xóa TODO người khác để làm checklist trông sạch.

## Kiểm tra và báo cáo

- Build/chạy trong Ubuntu/WSL: `make qemu`. Thoát bằng Ctrl+A rồi X; dọn bằng `make clean`.
- Dùng `docs/verification.md` để kiểm tra theo PDF. Bộ `make grade` MIT kiểm thêm yêu cầu ngoài phạm vi lớp, không dùng làm kết luận đạt 10 điểm.
- Nếu task chưa nối đủ vì phần của thành viên khác chưa merge, ghi rõ; không nhận là đã pass toàn bài.
- Không commit output build, ảnh đĩa, ZIP nộp bài, file report đã xuất hoặc thông tin xác thực.
- Cuối mỗi task, báo file đã sửa, lý do, luồng hoạt động bằng tiếng Việt dễ hiểu, lệnh đã chạy/kết quả thực tế và phần còn phụ thuộc.
- Code do AI sinh ra phải đủ rõ để người phụ trách giải thích khi vấn đáp. Không khẳng định đã kiểm tra nếu chưa chạy.
