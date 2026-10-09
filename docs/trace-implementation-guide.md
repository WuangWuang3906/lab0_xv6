# Hướng Dẫn Chi Tiết Triển Khai Syscall `trace` (Lab 0 - xv6)
> Tài liệu tổng hợp toàn bộ các bước triển khai kèm giải thích bản chất hệ điều hành (Lý do "Tại sao lại làm như vậy?") để phục vụ lập trình và trả lời vấn đáp (Demo Interviews).

---

## MỤC LỤC
1. [Bức Tranh Tổng Thể & Luồng Hoạt Động](#1-bức-tranh-tổng-thể--luồng-hoạt-động)
2. [BƯỚC 1: Phần Kernel Process (Lưu trữ & Nhận mask)](#2-bước-1-phần-kernel-process-lưu-trữ--nhận-mask)
3. [BƯỚC 2: Phần Dispatch & Intercept Syscall (Bắt & In Log)](#3-bước-2-phần-dispatch--intercept-syscall-bắt--in-log)
4. [BƯỚC 3: Phần User Space (Giao diện & Chương trình gọi lệnh)](#4-bước-3-phần-user-space-giao-diện--chương-trình-gọi-lệnh)
5. [BƯỚC 4: Build, Kiểm Thử & Các Câu Hỏi Vấn Đáp Thường Gặp](#5-bước-4-build-kiểm-thử--các-câu-hỏi-vấn-đáp-thường-gặp)

---

## 1. Bức Tranh Tổng Thể & Luồng Hoạt Động

### 1.1. Luồng thực thi khi người dùng gõ lệnh
Khi chạy: `$ trace 32 grep hello README`

```text
[NGƯỜI DÙNG GÕ LỆNH TRÊN TERMINAL]
                 │
                 ▼
[user/trace.c]
  1. Đọc số 32 từ argv[1] thành số nguyên mask.
  2. Tách "grep hello README" thành mảng nargv.
  3. Gọi syscall trace(32).
                 │
                 ▼
[xv6 KERNEL: sys_trace()]
  4. argint(0, &mask) đọc số 32 từ thanh ghi a0.
  5. myproc()->tracemask = 32 (lưu vào struct proc của tiến trình hiện tại).
  6. Return 0 báo thành công về User space.
                 │
                 ▼
[user/trace.c]
  7. Gọi exec("grep", nargv).
                 │
                 ▼
[xv6 KERNEL: sys_exec()]
  8. Nạp mã chương trình "grep" đè lên tiến trình hiện tại.
     *** ĐIỂM CỐT LÕI: struct proc giữ nguyên, p->tracemask = 32 VẪN CÒN NGUYÊN! ***
                 │
                 ▼
[Tiến trình grep bắt đầu chạy]
  9. grep gọi syscall read(...) (SYS_read = 5).
                 │
                 ▼
[xv6 KERNEL: hàm syscall()]
  10. Thực thi xong hàm read, kết quả trả về trong p->trapframe->a0.
  11. Kiểm tra bit: (p->tracemask & (1 << 5)) != 0.
      -> Thấy khớp (vì 32 = 1 << 5).
      -> In ra màn hình: "syscall read -> 1023".
                 │
                 ▼
[grep nhận dữ liệu đọc được và chạy tiếp]
```

---

## 2. BƯỚC 1: Phần Kernel Process (Lưu trữ & Nhận mask)

Mục tiêu bước này: Định nghĩa mã số cho syscall `trace`, tạo biến lưu `tracemask` trong Process Control Block và viết hàm `sys_trace()` để nhận tham số.

### 2.1. Sửa `kernel/syscall.h`
```c
// Thay #define SYS_hello 22 thành:
#define SYS_trace 22
```
* **Tại sao lại làm như vậy?**
  * Trong xv6, Kernel và phần cứng không quản lý syscall bằng tên chuỗi ký tự (như `"trace"`), mà dùng **số hiệu định danh (syscall number)**.
  * Syscall cuối cùng có sẵn trong xv6 gốc là `SYS_close` với số hiệu 21. Syscall mới thêm vào sẽ lấy số tiếp theo là **22**.
  * Khi gọi `trace()`, User space sẽ nạp số 22 này vào thanh ghi `a7` của CPU để Kernel biết cần điều phối tới hàm nào.

---

### 2.2. Sửa `kernel/proc.h`
Trong khai báo `struct proc`, thêm trường `tracemask`:
```c
struct proc {
  ...
  struct file *ofile[NOFILE];  // Open files
  struct inode *cwd;           // Current directory
  int tracemask;               // Mặt nạ bit theo dõi syscall của riêng process này
  char name[16];               // Process name (debugging)
};
```
* **Tại sao lại làm như vậy?**
  * `struct proc` chính là **Process Control Block (PCB)** — cấu trúc lưu toàn bộ thông tin riêng tư của từng tiến trình (PID, bảng trang, danh sách file mở,...).
  * Đề bài yêu cầu: *"The trace system call should enable tracing for the process that calls it, but should not affect other processes."* (Chỉ trace tiến trình gọi nó, không ảnh hưởng các tiến trình khác).
  * **Tại sao không dùng biến toàn cục (`int global_tracemask;`)?**  
    Nếu dùng biến toàn cục, khi bạn bật trace cho `grep`, các tiến trình chạy song song khác (như `sh` - Shell đang đợi gõ phím hay `init`) cũng sẽ bị trace lây, làm ngập rác terminal. Đặt `tracemask` vào `struct proc` đảm bảo mỗi tiến trình có cờ theo dõi hoàn toàn độc lập.

---

### 2.3. Sửa `kernel/proc.c`
Khởi tạo và reset `tracemask`:
* Trong hàm `allocproc()`:
  ```c
  p->tracemask = 0;
  ```
* Trong hàm `freeproc()`:
  ```c
  p->tracemask = 0;
  ```
* **Tại sao lại làm như vậy?**
  * **Trong `allocproc()`:** Bảng `proc[NPROC]` trong xv6 được tái sử dụng liên tục. Khi một tiến trình mới được sinh ra, ô nhớ của nó có thể chứa dữ liệu rác của tiến trình cũ. Ta phải gán bằng 0 để tiến trình mới mặc định không bị trace.
  * **Trong `freeproc()`:** Dọn dẹp sạch dữ liệu khi tiến trình kết thúc trước khi trả slot về trạng thái `UNUSED`.
  * **Tại sao KHÔNG reset trong `exec()`? (Câu hỏi vấn đáp then chốt):**  
    Chương trình `trace.c` gọi `trace(mask)` rồi gọi `exec("grep", ...)`. Lệnh `exec` tái sử dụng chính `struct proc` hiện tại mà không gọi `allocproc()`. Vì ta không reset `tracemask` trong `exec()`, giá trị mask **tự động được giữ nguyên sang `grep`**.

---

### 2.4. Sửa `kernel/sysproc.c`
Cài đặt hàm `sys_trace()`:
```c
uint64
sys_trace(void)
{
  int mask;
  argint(0, &mask);             // 1. Đọc số nguyên mask từ User space
  myproc()->tracemask = mask;  // 2. Lưu vào PCB của tiến trình hiện tại
  return 0;                    // 3. Trả về 0 báo thành công
}
```
* **Tại sao lại làm như vậy?**
  * **Tại sao không viết `sys_trace(int mask)`?**  
    Kernel và User space chạy ở 2 chế độ đặc quyền khác nhau với không gian địa chỉ riêng. User truyền tham số qua thanh ghi CPU (`a0`), được kernel lưu tạm vào `p->trapframe->a0`. Mọi hàm xử lý syscall trong xv6 đều có dạng `uint64 sys_xxx(void)`.
  * **Tại sao dùng `argint(0, &mask)`?**  
    Hàm trợ giúp này lấy giá trị 32-bit từ thanh ghi tham số thứ 0 (`trapframe->a0`) và ghi vào biến `mask`.
  * **`myproc()->tracemask = mask;`:**  
    `myproc()` trả về con trỏ tới `struct proc` của tiến trình đang thực thi trên CPU hiện tại. Gán dòng này để lưu cấu hình mask vào tiến trình.
  * **`return 0;`:**  
    Quy ước chuẩn của Hệ điều hành: syscall thực thi thành công trả về 0.

---

## 3. BƯỚC 2: Phần Dispatch & Intercept Syscall (Bắt & In Log)

Mục tiêu bước này: Đăng ký `sys_trace` vào bảng định tuyến syscall và đặt "chốt chặn" in log ngay tại trung tâm điều phối của mọi syscall.

### 3.1. Sửa `kernel/syscall.c` (Đăng ký hàm)
1. Khai báo prototype:
   ```c
   extern uint64 sys_trace(void);
   ```
2. Thêm vào mảng con trỏ hàm `syscalls[]`:
   ```c
   static uint64 (*syscalls[])(void) = {
     ...
     [SYS_close]   sys_close,
     [SYS_trace]   sys_trace,
   };
   ```
* **Tại sao lại làm như vậy?**
  * Mảng `syscalls[]` là bảng định tuyến (dispatch table). Khi nhận mã số 22 từ thanh ghi `a7`, kernel dùng trực tiếp `syscalls[22]()` để gọi đúng hàm `sys_trace()`.

---

### 3.2. Sửa `kernel/syscall.c` (Mảng tên syscall)
Tạo mảng ánh xạ từ số hiệu sang tên chữ:
```c
static char *syscall_names[] = {
  [SYS_fork]    "fork",
  [SYS_exit]    "exit",
  [SYS_wait]    "wait",
  [SYS_pipe]    "pipe",
  [SYS_read]    "read",
  [SYS_kill]    "kill",
  [SYS_exec]    "exec",
  [SYS_fstat]   "fstat",
  [SYS_chdir]   "chdir",
  [SYS_dup]     "dup",
  [SYS_getpid]  "getpid",
  [SYS_sbrk]    "sbrk",
  [SYS_sleep]   "sleep",
  [SYS_uptime]  "uptime",
  [SYS_open]    "open",
  [SYS_write]   "write",
  [SYS_mknod]   "mknod",
  [SYS_unlink]  "unlink",
  [SYS_link]    "link",
  [SYS_mkdir]   "mkdir",
  [SYS_close]   "close",
  [SYS_trace]   "trace",
};
```
* **Tại sao lại làm như vậy?**
  * Đề bài yêu cầu in ra: `syscall read -> 1023` (tên chữ `"read"`, không phải số 5). Mảng này giúp tra cứu tên từ chỉ số `num` một cách tức thì $O(1)$.

---

### 3.3. Sửa hàm `syscall(void)` trong `kernel/syscall.c`
Đặt chốt chặn in log:
```c
void
syscall(void)
{
  int num;
  struct proc *p = myproc();

  num = p->trapframe->a7;
  if(num > 0 && num < NELEM(syscalls) && syscalls[num]) {
    p->trapframe->a0 = syscalls[num](); // 1. Gọi syscall và lưu kết quả vào a0
    
    // 2. KIỂM TRA MẶT NẠ VÀ IN LOG:
    if ((p->tracemask >> num) & 1) {
      printf("syscall %s -> %d\n", syscall_names[num], p->trapframe->a0);
    }
  } else {
    printf("%d %s: unknown sys call %d\n", p->pid, p->name, num);
    p->trapframe->a0 = -1;
  }
}
```
* **Tại sao lại làm như vậy?**
  * **Tại sao đặt ở đây mà không đặt trong từng hàm `sys_read()`, `sys_write()`?**  
    Hàm `syscall()` là "nút cổ chai" duy nhất mà **mọi** system call đều bắt buộc phải đi qua khi CPU nhận lệnh `ecall`. Đặt log ở đây giúp theo dõi được tất cả các syscall chỉ với 1 đoạn code ngắn gọn.
  * **Toán tử `(p->tracemask >> num) & 1` nghĩa là gì?**  
    Dịch bit thứ `num` về vị trí bit 0 rồi AND với 1. Nếu bit đó bằng 1 (người dùng yêu cầu trace syscall này), biểu thức trả về `True`. (Cũng tương đương với `p->tracemask & (1 << num)`).
  * **Tại sao lấy kết quả trả về từ `p->trapframe->a0`?**  
    Theo quy ước gọi hàm của RISC-V, giá trị trả về của hàm được lưu vào thanh ghi `a0`. Khi hàm con chạy xong (`syscalls[num]()`), giá trị đó được gán vào `p->trapframe->a0` để chuyển tiếp về User space.

---

## 4. BƯỚC 3: Phần User Space (Giao diện & Chương trình gọi lệnh)

Mục tiêu bước này: Tạo cầu nối để User program có thể gọi hàm `trace()`, đưa lệnh `trace` vào hệ thống file của xv6.

### 4.1. Sửa `user/user.h`
Thêm khai báo nguyên mẫu hàm:
```c
int trace(int);
```
* **Tại sao lại làm như vậy?**
  * Để trình biên dịch C (gcc/clang) nhận biết hàm `trace` nhận 1 tham số kiểu `int` và trả về `int`. Nếu không có, mã nguồn `trace.c` sẽ báo lỗi `implicit declaration of function 'trace'`.

---

### 4.2. Sửa `user/usys.pl`
Thêm chỉ thị sinh stub:
```perl
entry("trace");
```
* **Tại sao lại làm như vậy?**
  * File Perl `usys.pl` khi build sẽ tự động sinh ra mã hợp ngữ RISC-V trong `user/usys.S`:
    ```assembly
    .global trace
    trace:
     li a7, SYS_trace   # Nạp số 22 vào thanh ghi a7
     ecall              # Bắn trap nhảy vào Kernel mode
     ret
    ```
  * Đây chính là "cây cầu" chuyển đổi từ lời gọi hàm C ở User space sang trap của phần cứng.

---

### 4.3. Sửa `Makefile`
Thêm vào danh sách `UPROGS`:
```make
UPROGS=\
    $U/_cat\
    $U/_echo\
    ...
    $U/_trace\
```
* **Tại sao lại làm như vậy?**
  * `UPROGS` là danh sách các chương trình người dùng được biên dịch và đóng gói vào ảnh đĩa hệ điều hành (`fs.img`). Nếu thiếu dòng này, file nhị phân `trace` sẽ không được tạo và bạn không thể gõ lệnh `trace` trong terminal xv6.

---

### 4.4. Hoàn thiện `user/trace.c`
```c
#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int i;
  char *nargv[MAXARG];

  // 1. Kiểm tra số lượng tham số: ít nhất phải có "trace <mask> <command>"
  if(argc < 3 || (argv[1][0] < '0' || argv[1][0] > '9')){
    fprintf(2, "Usage: %s mask command\n", argv[0]);
    exit(1);
  }

  // 2. Chuyển mask từ chuỗi sang số và gọi syscall trace
  if (trace(atoi(argv[1])) < 0) {
    fprintf(2, "%s: trace failed\n", argv[0]);
    exit(1);
  }

  // 3. Chuẩn bị mảng tham số cho lệnh cần chạy (bỏ đi "trace" và "mask")
  for(i = 2; i < argc && i < MAXARG; i++){
    nargv[i - 2] = argv[i];
  }
  nargv[argc - 2] = 0; // ĐIỀU KIỆN BẮT BUỘC: Phần tử cuối cùng của argv phải là NULL

  // 4. Gọi exec để thay thế chương trình hiện tại bằng lệnh mới
  exec(nargv[0], nargv);

  // Nếu exec thành công, dòng dưới sẽ không bao giờ chạy tới.
  // Nếu chạy tới đây nghĩa là exec bị lỗi (ví dụ không tìm thấy lệnh).
  fprintf(2, "exec %s failed\n", nargv[0]);
  exit(1);
}
```
* **Tại sao lại làm như vậy?**
  * **Kiểm tra `argc < 3`:** Đảm bảo người dùng cung cấp đủ mask và lệnh cần chạy.
  * **Tại sao phải có `nargv[argc - 2] = 0;`?**  
    Hàm `exec(path, argv)` trong POSIX và xv6 duyệt mảng con trỏ cho đến khi gặp con trỏ `NULL` (giá trị 0). Nếu thiếu số 0 kết thúc này, `exec` sẽ đọc tràn bộ nhớ ra ngoài mảng và bị crash.
  * **Xử lý lỗi `exec`:** Nếu lệnh gõ sai (như `trace 32 cmd_khong_ton_tai`), `exec` sẽ trả về -1. Cần in thông báo lỗi rõ ràng và thoát tiến trình với `exit(1)`.

---

## 5. BƯỚC 4: Build, Kiểm Thử & Các Câu Hỏi Vấn Đáp Thường Gặp

### 5.1. Lệnh build và kiểm tra
```bash
make clean
make qemu
```

Trong terminal xv6:
```bash
# Test 1: Chỉ trace hàm read (32 = 1 << 5)
$ trace 32 grep hello README
syscall read -> 1023
syscall read -> 966
syscall read -> 70
syscall read -> 0

# Test 2: Trace tất cả syscall (2147483647 = 0x7FFFFFFF)
$ trace 2147483647 grep hello README
syscall trace -> 0
syscall exec -> 3
syscall open -> 3
syscall read -> 1023
syscall read -> 966
syscall read -> 70
syscall read -> 0
syscall close -> 0

# Test 3: Lệnh thông thường không bị trace
$ grep hello README
```

---

### 5.2. Các câu hỏi vấn đáp (Demo Interviews) thường gặp

1. **Câu hỏi: Tại sao lệnh `trace 32` lại theo dõi được hàm `read`?**
   - **Trả lời:** Vì trong `kernel/syscall.h`, `SYS_read` có giá trị là 5. Phép dịch bit $1 \ll 5 = 2^5 = 32$. Khi bit thứ 5 bật, kernel so sánh `(p->tracemask & (1 << num))` và thấy khớp với số hiệu của `read`.

2. **Câu hỏi: Tại sao ta gọi `trace(mask)` trong `user/trace.c`, nhưng sang chương trình `grep` nó vẫn bị trace?**
   - **Trả lời:** Vì `exec()` không tạo ra tiến trình mới mà nạp đè mã nguồn của chương trình mới lên cùng một `struct proc` đang chạy. Trường `tracemask` trong `struct proc` vẫn được giữ nguyên qua lời gọi `exec`.

3. **Câu hỏi: Tại sao `argint()` lại cần con trỏ `&mask` thay vì trả về giá trị trực tiếp?**
   - **Trả lời:** Trong mã nguồn xv6 phiên bản này, hàm `argint(int n, int *ip)` có kiểu trả về là `void` và ghi giá trị đọc được vào vùng nhớ trỏ bởi `ip`. Giá trị được đọc từ thanh ghi `a0` đến `a5` tương ứng được lưu trong `trapframe` của tiến trình.

4. **Câu hỏi: Nếu một tiến trình gọi `fork()`, tiến trình con có bị trace không?**
   - **Trả lời:** Theo đúng đặc tả trong file PDF đề bài của môn học, phạm vi bài Lab chỉ yêu cầu trace tiến trình gọi nó và giữ qua `exec`, không yêu cầu kế thừa qua `fork`. Do đó trong `fork()` ta không sao chép `tracemask` sang tiến trình con.
