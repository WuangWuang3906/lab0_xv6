# Hiểu Lab0: system call và trace

Nguồn yêu cầu: [Lab0_System_Call.pdf](assignment/Lab0_System_Call.pdf), đủ 5 trang. Nhóm đã chốt chỉ làm `trace` theo PDF.

## 1. Đồ án yêu cầu gì?

xv6 là một hệ điều hành nhỏ dùng để học. Kernel là phần lõi quản lý các việc như đọc file, tạo process và phân phối tài nguyên. Chương trình như `grep` phải nhờ kernel làm những việc đó thông qua system call.

Lab0 yêu cầu thêm system call `trace(mask)` và chương trình user `trace`. Kernel sẽ in tên và giá trị trả về của system call được chọn, ngay trước khi lời gọi trả về. Tính năng này giúp quan sát một chương trình đang nhờ hệ điều hành làm gì, phục vụ debug các bài sau.

`grep` là chương trình có sẵn: `grep hello README` đọc file README và in những dòng chứa chữ hello. Nếu không có dòng khớp, nó không in dòng kết quả. Nhóm vẫn dùng grep gốc để kiểm tra.

## 2. Tại sao cùng tên trace?

| Tên | Là gì? | Làm việc gì? |
| --- | --- | --- |
| `trace` | Chương trình trong user space | Đọc mask và lệnh, bật theo dõi rồi chạy lệnh. |
| `trace(mask)` | System call được chương trình gọi | Đưa mask vào kernel. |
| `sys_trace()` | Hàm xử lý trong kernel | Lấy mask từ đối số và lưu vào process gọi. |

Với `trace 32 grep hello README`, chương trình trace lấy mask 32, gọi `trace(32)` rồi `exec` để process đó chạy grep. `exec` thay chương trình đang chạy của cùng process, nên mask đã lưu vẫn dùng được.

Khi grep gọi read, dispatcher kernel thực hiện read như bình thường. Sau đó nó kiểm tra bit tương ứng trong mask; nếu bit bật, kernel ghi một dòng log chứa tên read và giá trị trả về thực tế.

## 3. Mask là một số chứa nhiều lựa chọn

Mỗi bit của mask là một công tắc. Bit ở vị trí bằng số hiệu syscall quyết định có log syscall đó hay không. Trong source này `SYS_read` bằng 5, nên `32 = 1 << 5` chỉ bật bit số 5.

`2147483647 = 2^31 - 1` có 31 bit thấp bằng 1, chọn mọi syscall có số hiệu từ 0 đến 30. Những syscall hiện có của bài nằm trong vùng đó. Mask chọn lời gọi để theo dõi; chương trình đang chạy quyết định thực sự gọi syscall nào.

Giá trị của read là số byte đọc được; 0 nghĩa là hết file. Một lần read có thể chứa nhiều dòng, nên số lần read không phải số dòng trong README.

## 4. Toàn bộ ví dụ của PDF

PDF trang 4 đưa mẫu sau:

```text
$ trace 32 grep hello README
syscall read -> 1023
syscall read -> 966
syscall read -> 70
syscall read -> 0
$
$ trace 2147483647 grep hello README
syscall trace -> 0
syscall exec -> 3
syscall open -> 3
syscall read -> 1023
syscall read -> 966
syscall read -> 70
syscall read -> 0
syscall close -> 0
$
$ grep hello README
$
```

Mẫu không có dòng grep khớp hello. Điều đó cho thấy file README của ví dụ không có dòng phù hợp; chương trình vẫn phải đọc file để tìm nên các log read xuất hiện.

Starter MIT 2023 của nhóm có nội dung README khác snapshot PDF, nên số byte read có thể khác. Cần log đúng giá trị syscall thực sự trả về; không sửa read hoặc README để ép số 966/70. Xem [verification.md](verification.md) để đối chiếu hành vi.

## 5. Các bước kỹ thuật PDF yêu cầu

PDF trang 2-3 dùng system call hello để minh họa: gán số hiệu, nối handler vào bảng kernel, viết handler, khai báo phía user, thêm stub, chương trình thử và Makefile. Đây là hướng dẫn cách thêm syscall, không phải một bài hello cần nộp.

Đối với trace, PDF trang 4-5 yêu cầu:

- User nhận mask ở `argv[1]`; chương trình cần chạy ở `argv[2]`, các đối số còn lại từ `argv[3]`.
- User gọi `trace(mask)` trước khi exec lệnh đích.
- Thêm `_trace` vào UPROGS của Makefile.
- Khai báo phía user, thêm stub trong `user/usys.pl` và số hiệu trong `kernel/syscall.h`.
- Viết `sys_trace()` lấy mask bằng argint và lưu trong một trường mới của struct proc.
- Sửa `syscall()` để in log syscall được mask chọn; có mảng tên syscall theo số hiệu.
- Mask gắn với process gọi; việc bật trace không làm process khác tự bật trace.
- Log gồm tên syscall và giá trị trả về, theo format mẫu trên.

Source hiện có dùng handler `uint64` và `argint()` trả về void. Khi viết code, dùng chữ ký của source đang làm; phần hello trong PDF là minh họa tổng quát.

## 6. Cần đọc và hiểu trước khi làm

Theo trang 3, đọc Chương 2 của sách xv6 và mục 4.3, 4.4 Chương 4. Các nhóm file cần đọc:

| File | Vai trò |
| --- | --- |
| `user/user.h` | Khai báo hàm system call phía user. |
| `user/usys.pl` | Sinh `user/usys.S`, stub dùng ecall để đi vào kernel. |
| `kernel/syscall.h`, `kernel/syscall.c` | Số hiệu, lấy đối số và dispatcher. |
| `kernel/sysproc.c` | Handler như sys_getpid, sys_sleep; nơi viết sys_trace. |
| `kernel/proc.h`, `kernel/proc.c` | Dữ liệu riêng và vòng đời process. |
| `user/trace.c`, `Makefile` | Lệnh user và cách đưa nó vào ảnh đĩa. |

Trang 3 cũng dẫn `sys_fstat()`, `sys_exec()` trong `kernel/sysfile.c` và `filestat()` trong `kernel/file.c` để xem cách nhận đối số/trả dữ liệu: argint, argaddr, fetchaddr/copyin và copyout. Bài trace dùng đối số nguyên; không cần tự thêm một bài copyout riêng.

PDF hướng dẫn bắt đầu ở nhánh syscall rồi make clean. Repo nhóm đã lấy đúng nhánh MIT đó; thành viên clone nhánh syscall của repo nhóm theo README.

## 7. Phạm vi lớp và tài liệu tham khảo

Bảng điểm trang 5 chỉ có trace, 10 điểm. PID trong log, kế thừa mask qua fork và bài sysinfo của trang MIT không được đưa vào phạm vi nhóm đã chốt. Các file sysinfo và bộ chấm MIT vẫn có trong starter gốc; việc chúng tồn tại không biến chúng thành yêu cầu Lab0 của lớp.

## 8. Các quy định ngoài code

Trang 1 quy định nhóm tối đa 3 người; nộp Moodle, không nhận email hay hình thức khác. ZIP và report dùng mã sinh viên tăng dần. Bộ nộp gồm report PDF ngắn không chứa source code, patch và ZIP source xv6 đã make clean.

Demo đánh giá hiểu code và đóng góp từng người. PDF cảnh báo bài làm giống nhau có thể bị 0 cho toàn bộ phần thực hành. Các thành viên cần tự hiểu và giải thích phần mình, kể cả khi AI hỗ trợ.

Xem [submission.md](submission.md) để chuẩn bị đúng tên file và mốc diff sau khi đã commit code.
