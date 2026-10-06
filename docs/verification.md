# Kiểm tra trace theo PDF

Các mục dưới đây dùng sau khi ba phần đã tích hợp. Bộ khung mới chưa thực hiện trace; việc xv6 boot được chỉ xác nhận môi trường/source ban đầu, không có nghĩa bài đã hoàn tất.

## Build và chạy

Trong terminal Ubuntu/WSL ở repo, commit hoặc biết rõ các thay đổi đang làm trước khi đổi nhánh:

```bash
git switch syscall
make clean
make qemu
```

Trong shell xv6, nhập các lệnh tiếp theo. Thoát QEMU bằng Ctrl+A rồi X. Sau khi trở về Ubuntu, chạy make clean khi cần dọn build.

## Ví dụ 1: chỉ theo dõi read

```text
trace 32 grep hello README
```

Mẫu PDF trang 4:

```text
syscall read -> 1023
syscall read -> 966
syscall read -> 70
syscall read -> 0
```

- [ ] Trong log chỉ có syscall read, không có log open/exec/close.
- [ ] Mỗi giá trị là giá trị read thực sự trả về; lần cuối 0 là EOF.
- [ ] Grep vẫn thực hiện tìm kiếm và in các dòng khớp nếu có.

## Ví dụ 2: theo dõi các bit thấp đều bật

```text
trace 2147483647 grep hello README
```

Mẫu PDF:

```text
syscall trace -> 0
syscall exec -> 3
syscall open -> 3
syscall read -> 1023
syscall read -> 966
syscall read -> 70
syscall read -> 0
syscall close -> 0
```

- [ ] Có log trace ngay sau khi handler đã lưu mask và trả 0.
- [ ] Có tên và giá trị trả về của các syscall trong đường đi trên.
- [ ] Log theo format `syscall <name> -> <return value>`, không thêm PID vào format của PDF.
- [ ] Không đổi kết quả/luồng thực hiện syscall để tạo log.

## Ví dụ 3: lệnh không bật trace

```text
grep hello README
```

- [ ] Không có dòng log syscall; nếu không khớp hello thì chỉ trở lại dấu nhắc.
- [ ] Chạy plain grep ngay sau lệnh trace phía trên cũng không có log, chứng tỏ shell/lệnh khác không bị bật theo.

## Số byte và dữ liệu mẫu

File README của MIT 2023 khác snapshot trong PDF, nên các số read giữa hai nguồn có thể khác (bộ chấm MIT cũng ghi số khác). Đối chiếu syscall, bit lựa chọn, giá trị thực tế và EOF. Giữ nguyên README gốc để còn biết dữ liệu đang dùng. README.md của nhóm là hướng dẫn GitHub và không được Makefile đưa vào ảnh đĩa như file README gốc.

## Các kiểm tra nhỏ nhóm khuyến nghị

Đây là kiểm tra chất lượng chương trình, không phải bài điểm mới:

- [ ] `trace 0 grep hello README`: không in log syscall do không bit nào bật.
- [ ] Thiếu mask hoặc lệnh: wrapper báo cách dùng và kết thúc, không truy cập argv ngoài giới hạn.
- [ ] Lệnh không tồn tại: báo lỗi exec và kết thúc đúng, không im lặng báo thành công.
- [ ] Lệnh có nhiều đối số vẫn nhận đủ đối số; argv có NULL cuối và không vượt MAXARG.
- [ ] Sau nhiều lần chạy/thoát, process được tái sử dụng không thừa mask cũ.
- [ ] Syscall trả lỗi được log đúng giá trị âm; không in -1 thành số dương lớn.

## Ghi bằng chứng trong PR/Issue

```text
Commit được kiểm tra:
Môi trường:
Lệnh đã chạy:
Kết quả quan sát:
Yêu cầu tương ứng trong PDF:
Phần chưa kiểm tra/đang chờ:
```

`make grade` đi kèm starter MIT kiểm PID, trace con, sysinfo và phần trả lời/time. Nó khác rubric lớp; không sửa bộ chấm hoặc thêm tính năng ngoài PDF để đạt bộ chấm đó.
