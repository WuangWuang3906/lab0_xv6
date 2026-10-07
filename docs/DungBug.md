# Tổng hợp các lỗi thường gặp trong quá trình làm Lab0 (DungBug)

Tài liệu này tổng hợp toàn bộ các bug thực tế mà Dũng đã gặp phải trong quá trình thiết lập và thêm System Call trong xv6, cùng với nguyên nhân gốc rễ và cách xử lý để các thành viên trong nhóm tham khảo khi gặp sự cố tương tự.

---

## 1. Lỗi sắp xếp `#include` tự động (Auto-format Sort Includes)

### Hiện tượng lỗi
Khi chạy `make qemu`, trình biên dịch văng hàng loạt lỗi kiểu dữ liệu:
```text
kernel/riscv.h:20: error: unknown type name 'uint64'
kernel/riscv.h:23: error: unknown type name 'uint64'
kernel/spinlock.h:3: error: unknown type name 'uint'
kernel/syscall.c:16:7: error: implicit declaration of function 'copyin'
make: *** [Makefile:130: kernel/syscall.o] Error 1
```

### Nguyên nhân
- Trong xv6, file `types.h` là nơi định nghĩa các kiểu số nguyên nền tảng của hệ điều hành: `uint64`, `uint32`, `uint`, `uchar`...
- Các file header khác như `riscv.h`, `stat.h`, `defs.h`, `spinlock.h` đều sử dụng các kiểu này.
- **Thủ phạm:** Tính năng tự động định dạng mã nguồn (Format on Save / Clang-Format) của IDE đã tự động sắp xếp lại các dòng `#include` theo thứ tự bảng chữ cái (A -> Z), vô tình đẩy `#include "types.h"` xuống dòng 8 sau `defs.h`, `riscv.h`...
- Do đó, khi trình biên dịch đọc các file header trước mà chưa thấy `types.h`, nó không biết `uint64` là gì và báo lỗi hàng loạt.

### Cách khắc phục
Trong tất cả các file C của xv6 (cả trong `kernel/` và `user/`), **`#include "types.h"` (hoặc `"kernel/types.h"`) BẮT BUỘC PHẢI LUÔN NẰM Ở DÒNG ĐẦU TIÊN**:

```c
// Chuẩn: types.h luôn đứng đầu
#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "syscall.h"
#include "defs.h"
```

---

## 2. Lỗi tương thích con trỏ hàm trong `user/usertests.c` trên GCC 14+

### Hiện tượng lỗi
```text
user/usertests.c:2583:4: error: initialization of 'void (*)(char *)' from incompatible pointer type 'void (*)(void)' [-Wincompatible-pointer-types]
 2583 |   {rwsbrk, "rwsbrk" },
      |    ^~~~~~
user/usertests.c:244:1: note: 'rwsbrk' declared here
  244 | rwsbrk()
      | ^~~~~~
make: *** [<builtin>: user/usertests.o] Error 1
```

### Nguyên nhân
- Mảng `quicktests[]` trong `user/usertests.c` định nghĩa con trỏ hàm nhận một tham số chuỗi:
  ```c
  struct test {
    void (*f)(char *);
    char *s;
  };
  ```
- Tuy nhiên hàm `rwsbrk` ở dòng 244 lại được khai báo là `void rwsbrk()` (không tham số).
- Trên các bộ công cụ GCC mới (GCC 14+ mặc định trên Ubuntu 24.04+), cảnh báo `-Wincompatible-pointer-types` được nâng lên thành lỗi nghiêm trọng (kết hợp với cờ `-Werror` trong Makefile) khiến build bị hủy.

### Cách khắc phục
Sửa khai báo hàm `rwsbrk` trong `user/usertests.c` (khoảng dòng 244) thêm tham số `char *s`:

```diff
-void
-rwsbrk()
+void
+rwsbrk(char *s)
 {
   int fd, n;
   ...
```

---

## 3. Lỗi QEMU bị treo / đứng vô tận ở 100% CPU khi boot (`make qemu`)

### Hiện tượng lỗi
Chạy lệnh `make qemu`, terminal chỉ in ra một dòng lệnh khởi chạy QEMU rồi con trỏ đứng yên nhấp nháy mãi mãi. Không thấy dòng `xv6 kernel is booting` xuất hiện, đồng thời CPU máy ngốn 100% (hoặc ~300% cho 3 nhân ảo):
```text
$ make qemu
qemu-system-riscv64 -machine virt -bios none -kernel kernel/kernel -m 128M -smp 3 -nographic -global virtio-mmio.force-legacy=false -drive file=fs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0
[con trỏ đứng im mãi mãi...]
```

### Nguyên nhân
1. **Lỗi tập lệnh mở rộng Zcb:** Trình biên dịch GCC mới (`gcc-riscv64-linux-gnu`) tự động kích hoạt phần mở rộng Zcb (nén lệnh) theo mặc định. Lệnh `mul a0, a0, a1` trong `kernel/entry.S` bị nén thành `c.mul` (mã máy `0x9d4d`). Máy ảo QEMU virt không bật Zcb nên xem đây là ngoại lệ **Illegal Instruction** ngay tại lệnh thứ 5 khi CPU vừa bật. Do lúc này kernel chưa kịp cài bảng vector ngắt (`mtvec`), CPU bị rơi vào vòng lặp double-trap vô tận tại địa chỉ `0x0`.
2. **Thiếu cờ và rule cho Assembler:** Trong `Makefile` ban đầu, thiếu biến `ASFLAGS = $(CFLAGS)` và thiếu rule biên dịch cho các file hợp ngữ `$K/%.o: $K/%.S`, khiến file `.S` của kernel bị biên dịch với các thiết lập mặc định không tương thích (thiếu `-mno-relax`, `-mcmodel=medany`).

### Cách khắc phục
1. Mở file `Makefile`, thêm cấu hình kiến trúc chuẩn và gán cờ assembler:
   ```makefile
   CFLAGS += $(XCFLAGS)
   CFLAGS += -march=rv64gc -mabi=lp64d
   ASFLAGS = $(CFLAGS)
   CFLAGS += -MD
   ```
2. Thêm rule biên dịch cho file assembly trong kernel (ngay dưới rule `$K/%.o: $K/%.c`):
   ```makefile
   $K/%.o: $K/%.c
   	$(CC) $(CFLAGS) $(EXTRAFLAG) -c -o $@ $<

   $K/%.o: $K/%.S
   	$(CC) $(CFLAGS) $(EXTRAFLAG) -c -o $@ $<
   ```
   *(Lưu ý: Thụt đầu dòng bằng phím Tab thật)*.
3. Dọn dẹp và chạy lại:
   ```bash
   make clean
   make qemu
   ```

---

## 4. Lỗi gõ nhầm ký tự rác khi chỉnh sửa code

### Hiện tượng lỗi
Một chữ cái vô nghĩa xuất hiện trơ trọi giữa file (ví dụ chữ `d` đứng một mình ở dòng 18 trong `kernel/sysfile.c`), gây lỗi cú pháp không xác định khi biên dịch.

### Nguyên nhân
Thao tác bàn phím trong quá trình dùng phím tắt hoặc chuyển đổi giữa chế độ gõ tiếng Việt / chế độ lệnh vô tình chèn thêm ký tự vào file.

### Cách phòng tránh & khắc phục
- Trước khi chạy lệnh build hoặc commit, luôn chạy lệnh sau trong terminal để kiểm tra chính xác những gì mình đã thay đổi:
  ```bash
  git diff
  ```
- Nếu thấy dòng nào lạ không phải do mình chủ động viết, hãy xóa hoặc hoàn tác ngay.
