# Phân công và giao diện cho trace

Tài liệu này ghi lựa chọn làm nhóm trên source hiện tại. Yêu cầu chức năng lấy từ PDF trang 4-5; những tên biến/nhánh dưới đây là quy ước nhóm để các phần nối được với nhau.

## Giao diện chung

| Phần | Tên/chữ ký đã chốt | Người tạo | Người dùng |
| --- | --- | --- | --- |
| Số hiệu syscall | `SYS_trace` bằng `22` (số 1-21 đã có) | Khanh | Stub của Dũng và dispatcher của Quang |
| Hàm user | `int trace(int mask)` | Dũng: khai báo và entry stub | `user/trace.c` |
| Handler kernel | `uint64 sys_trace(void)` | Khanh | Bảng dispatcher của Quang |
| Trường process | `int tracemask` | Khanh | sys_trace và dispatcher |
| Log | `syscall <name> -> <return value>` + newline | Quang | Ví dụ/demo |

Mask mới bắt đầu ở 0; reset khi cấp/tái dùng process để mask cũ không rò sang process khác. Khi exec thành công, vẫn là process đó và mask cần được giữ. Không bổ sung kế thừa qua fork trong phạm vi hiện tại.

## Quang: dispatcher và log

Task: [Issue #1](https://github.com/WuangWuang3906/lab06_xv6/issues/1). Nhánh: `wuang`. File sở hữu: `kernel/syscall.c`.

- [ ] Khai báo handler kernel mới và nối SYS_trace vào bảng dispatch.
- [ ] Tạo mảng tên syscall, index khớp số hiệu hiện có và trace.
- [ ] Sau khi handler trả về, chọn log theo tracemask của process hiện tại.
- [ ] In đúng tên/giá trị thực tế; đọc kernel/printf.c để dùng format và kiểu phù hợp, kể cả giá trị lỗi -1.
- [ ] Giữ nhánh unknown syscall và kết quả trả về cho user hoạt động đúng.
- [ ] Tổ chức tích hợp cùng nhóm, đối chiếu ví dụ PDF và tổng hợp report/patch/source.

Phụ thuộc: trường tracemask và sys_trace từ Khanh; lệnh trace/stub từ Dũng. Quang cần tự hoàn thiện phần log, không chỉ merge bài.

## Dũng: lệnh trace và phía user

Task: [Issue #2](https://github.com/WuangWuang3906/lab06_xv6/issues/2). Nhánh đề xuất: `dung/trace-user-command`. Files: `user/trace.c`, `user/user.h`, `user/usys.pl`, `Makefile`.

- [ ] Đọc wrapper user/trace.c có sẵn và kiểm tra các đối số.
- [ ] Lấy mask, gọi trace trước exec và truyền đủ chương trình/đối số lệnh đích.
- [ ] Đảm bảo argv truyền cho exec có phần tử NULL cuối, không vượt MAXARG; xử lý exec thất bại.
- [ ] Thêm khai báo user và entry trong generator stub; không sửa usys.S.
- [ ] Thêm `_trace` vào UPROGS đúng cú pháp Makefile.
- [ ] Giải thích được đường đi từ user trace qua stub vào kernel và tự viết phần report của mình.

Kiểm tra đối số và lỗi là phần làm cho chương trình dùng được; đây không phải bài điểm bổ sung. Phụ thuộc: SYS_trace và handler kernel từ Khanh, dispatch từ Quang.

## Khanh: mask và handler kernel

Task: [Issue #3](https://github.com/WuangWuang3906/lab06_xv6/issues/3). Nhánh đề xuất: `khanh/trace-process-mask`. Files: `kernel/syscall.h`, `kernel/proc.h`, `kernel/proc.c`, `kernel/sysproc.c`.

- [ ] Thêm SYS_trace bằng 22 và trường int tracemask theo giao diện trên.
- [ ] Khởi tạo/reset mask để process mới và process tái dùng không thừa mask của chương trình khác.
- [ ] Viết uint64 sys_trace(void), lấy đối số 0 bằng argint, lưu vào myproc và trả 0 khi thành công.
- [ ] Giữ mask qua exec; không đưa mask vào biến chung toàn kernel.
- [ ] Kiểm tra tính riêng biệt của process sau tích hợp và tự viết phần report của mình.

Phụ thuộc lúc chạy thử: user/stub từ Dũng và dispatcher từ Quang. Không sửa kernel/syscall.c thay Quang để nhánh tự build độc lập.

## TODO trong code và tiến độ trên GitHub

Các TODO là comment chỉ vị trí phải làm, không phải lời giải hoàn chỉnh. user/trace.c có mã wrapper gốc để Dũng đọc/hoàn thiện. Không bỏ hoặc viết lại toàn bộ mã gốc chỉ vì thấy TODO.

Issue là nguồn cập nhật tiến độ chính; các checkbox ở đây diễn tả đầu ra của task. Tìm Issue theo tên người ở [danh sách Issues](https://github.com/WuangWuang3906/lab06_xv6/issues). GitHub usernames của Dũng/Khanh cần được bổ sung để gán assignee; việc này không cản hai bạn đọc task.

Các phần phụ thuộc nên nhóm cần kiểm tra giao diện cùng nhau. Sau khi từng PR được review, phối hợp merge và chạy lại checklist trên nhánh chung; chỉ ghi toàn bài pass sau bước đó.

## Mỗi người hoàn thành một vòng học và giải thích

Đọc phần mình -> tự thực hiện hoặc dùng AI hỗ trợ theo task -> đọc diff -> chạy phần có thể chạy -> giải thích cho một bạn khác -> sửa góp ý. Tất cả cùng hiểu luồng đầy đủ để demo, còn Quang theo dõi trạng thái và phụ thuộc.
