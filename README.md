# lab06_xv6

Đồ án **Lab0 / Project 0 - System Call**, nhóm Quang, Dũng và Khanh. Tên repo là `lab06_xv6` theo tên nhóm chọn; bài làm vẫn là Lab0 trong tài liệu của thầy.

Mục tiêu: thêm `trace(mask)` vào xv6 để kernel ghi tên và giá trị trả về của những system call được mask chọn; chương trình `trace` bật theo dõi rồi chạy một chương trình khác, ví dụ `grep`.

**Yêu cầu gốc:** [Lab0_System_Call.pdf](docs/assignment/Lab0_System_Call.pdf). Nhóm chỉ làm `trace` theo PDF. Bảng điểm ghi `trace: 10`; điểm thực tế còn phụ thuộc kết quả và phần hiểu code khi demo.

## 1. Đọc gì và bắt đầu ở đâu?

| Tài liệu | Dùng để làm gì? |
| --- | --- |
| [Giải thích Lab0](docs/lab0-overview.md) | Hiểu kernel, system call, `grep`, `trace` và mask bằng lời đơn giản. |
| [Phân công và giao diện](docs/work-plan.md) | Biết phần của từng người, file sở hữu và các tên đã thống nhất. |
| [Quy tắc làm nhóm](CONTRIBUTING.md) | Tạo nhánh, commit, gửi pull request và review. |
| [Prompt dùng AI](docs/ai-prompts.md) | Mẫu đọc hiểu, thực hiện task và review cho cả nhóm. |
| [Kiểm tra theo PDF](docs/verification.md) | Các lệnh và kết quả cần quan sát sau khi hoàn thiện `trace`. |
| [Nộp bài](docs/submission.md) | Report, patch, source sạch và tên ZIP. |
| [Mẫu report](docs/report-template.md) | Mẫu viết ngắn; mỗi người điền phần đóng góp của mình. |
| [AGENTS.md](AGENTS.md) | Quy tắc dành cho AI làm việc trong repo. |

Trước khi code, đọc Chương 2 và mục 4.3, 4.4 Chương 4 của sách xv6 như PDF yêu cầu. Sau đó đọc task của mình trong [GitHub Issues](https://github.com/WuangWuang3906/lab06_xv6/issues).

## 2. Trạng thái bộ khung

Repo chứa mã MIT `xv6-labs-2023`, lấy từ nhánh `syscall`, cùng tài liệu làm nhóm và các comment `TODO[Quang]`, `TODO[Dung]`, `TODO[Khanh]` tại vị trí cần làm.

Đây là **bộ khung để nhóm hoàn thiện**, chưa có system call `trace` hoạt động. Starter đã có `user/trace.c`; Dũng cần đọc, kiểm tra và hoàn thiện đoạn đó. Chương trình `_trace` chưa được thêm vào `UPROGS`, nên lúc bắt đầu xv6 vẫn boot bình thường nhưng chưa chạy được lệnh `trace`.

`syscall` là nhánh chung để tích hợp. Nhánh `wuang` đã được tạo cho Quang. Dũng và Khanh tạo nhánh riêng khi nhận task. Nhánh `main` ban đầu được giữ lại để bảo toàn commit GitHub đã có; nhóm không dùng nó làm nhánh tích hợp.

Tag `lab0-base` đánh dấu mã MIT trước khi nhóm thêm bộ khung, dùng làm mốc tạo patch. Không xóa hoặc di chuyển tag này.

Kiểm tra bộ khung ngày 06/10/2026: build kernel và fs.img thành công; QEMU boot đến shell, chạy được `echo lab06_ready` và `grep hello README`. Các link nội bộ và bản PDF đã được đối chiếu. Đây là kiểm tra starter, chưa phải kiểm tra tính năng trace hoàn chỉnh.

## 3. Windows, WSL và GitHub liên quan thế nào?

GitHub mở trong trình duyệt Windows để xem code, tạo Issue và review pull request. Mã xv6 nằm trong Ubuntu/WSL; các lệnh Git, build và QEMU bên dưới chạy trong **terminal Ubuntu**, không chạy trực tiếp ở PowerShell.

Nên đặt source trong thư mục Linux như `~/lab06_xv6`. Máy Quang đã có môi trường và bản repo ở đường dẫn này; Quang không cần clone thêm một bản.

### Thành viên chưa có môi trường

Trong PowerShell có quyền quản trị, cài WSL nếu chưa có:

```powershell
wsl --install -d Ubuntu
```

Khởi động lại nếu Windows yêu cầu, mở Ubuntu và tạo tài khoản Linux. Trong Ubuntu, cài các gói để build và chạy:

```bash
sudo apt update
sudo apt install build-essential git perl qemu-system-misc gcc-riscv64-unknown-elf binutils-riscv64-unknown-elf
```

Nếu cần debug bằng GDB, cài thêm `sudo apt install gdb-multiarch`. Không cần GDB chỉ để chạy `make qemu`.

Kiểm tra các chương trình đã có:

```bash
git --version
make --version
riscv64-unknown-elf-gcc --version
qemu-system-riscv64 --version
```

### Clone repo cho Dũng và Khanh

Quang cần mời hai bạn vào repo riêng tư trước. Mỗi bạn tự xác thực bằng tài khoản GitHub của mình nếu Git yêu cầu.

```bash
cd ~
git clone --branch syscall https://github.com/WuangWuang3906/lab06_xv6.git
cd lab06_xv6
git remote -v
git branch --show-current
```

Kết quả nhánh là `syscall`, remote `origin` trỏ tới repo nhóm. Nếu Git chưa có danh tính, cấu hình tên/email của bạn trong repo này; email dùng email đã liên kết với GitHub hoặc email noreply của tài khoản:

```bash
git config user.name "Tên của bạn"
git config user.email "email-cua-ban@example.com"
```

Trên máy Quang, `origin` cũng trỏ tới GitHub nhóm; nguồn MIT được giữ với tên remote `upstream`.

## 4. Build và chạy xv6 lần đầu

Trong Ubuntu, tại thư mục repo:

```bash
cd ~/lab06_xv6
make qemu
```

Đợi dòng `xv6 kernel is booting` và dấu nhắc `$`. Thử trong **shell của xv6**:

```text
echo hello
ls
grep hello README
```

`grep` chỉ in dòng chứa `hello`; không có dòng khớp thì không có output. Thoát QEMU bằng **Ctrl+A, thả phím, rồi bấm X**. Khi đã về terminal Ubuntu, `make clean` dọn các file build.

Có thể chỉnh code bằng VS Code trên Windows với hỗ trợ WSL: mở thư mục `~/lab06_xv6` qua WSL, rồi dùng terminal Ubuntu của cửa sổ đó. Editor là lựa chọn của mỗi người; lệnh build vẫn chạy trong WSL.

## 5. Ai làm phần nào?

| Người | Công việc code | File sở hữu |
| --- | --- | --- |
| [Quang - Issue #1](https://github.com/WuangWuang3906/lab06_xv6/issues/1) / `wuang` | Nối dispatcher, tra tên system call, kiểm tra mask và in kết quả khi system call trả về. Điều phối tích hợp. | `kernel/syscall.c` |
| [Dũng - Issue #2](https://github.com/WuangWuang3906/lab06_xv6/issues/2) | Hoàn thiện lệnh `trace`, khai báo user, stub và thêm chương trình vào build. | `user/trace.c`, `user/user.h`, `user/usys.pl`, `Makefile` |
| [Khanh - Issue #3](https://github.com/WuangWuang3906/lab06_xv6/issues/3) | Số hiệu system call, mask riêng của process, vòng đời mask và `sys_trace()`. | `kernel/syscall.h`, `kernel/proc.h`, `kernel/proc.c`, `kernel/sysproc.c` |

Quang có phần kernel cụ thể. Cả ba cùng kiểm tra và tập demo; mỗi người viết phần đóng góp/giải thích của mình trong report. Xem [work-plan.md](docs/work-plan.md) trước khi sửa code để dùng cùng giao diện.

Tìm TODO của nhóm bằng Git, không cần cài công cụ tìm kiếm khác:

```bash
git grep -n 'TODO\[' -- kernel user Makefile
```

## 6. Nhận task, làm trên nhánh riêng và gửi bài

Cập nhật nhánh chung trước khi tạo nhánh làm việc; trước khi đổi nhánh, bảo đảm thay đổi của mình đã được commit:

```bash
git fetch origin
git switch syscall
git pull --ff-only origin syscall
```

Quang dùng nhánh đã có:

```bash
git switch wuang
```

Dũng hoặc Khanh tạo nhánh từ `syscall`, mỗi người chỉ chạy lệnh dành cho mình:

```bash
git switch -c dung/trace-user-command
```

```bash
git switch -c khanh/trace-process-mask
```

Cập nhật Issue sang `in-progress`, làm đúng phần được giao, rồi xem `git diff`. Các task phụ thuộc nhau nên một nhánh có thể chưa build được riêng; ghi rõ phần còn chờ, không tự hoàn thiện file của người khác. Dùng Draft pull request để xin góp ý sớm.

Khi có thay đổi, stage từng file được giao. Ví dụ cho Quang:

```bash
git add kernel/syscall.c
git diff --cached
git commit -m "feat: add syscall trace logging"
git push -u origin wuang
```

Trên GitHub, mở pull request: **base `syscall`**, compare nhánh vừa push. Điền mẫu sẵn có, ghi Issue liên quan và kết quả kiểm tra thực tế. Quang xem PR của Dũng/Khanh; PR của Quang cần một thành viên khác xem. Cả nhóm phối hợp tích hợp trước khi kết luận `trace` chạy đúng.

Nhánh `wuang` ban đầu giống `syscall`; chỉ mở PR sau khi có thay đổi. Khi cần lấy phần đã merge từ nhóm, ở nhánh làm việc chạy `git fetch origin` rồi `git merge origin/syscall`; xem [CONTRIBUTING.md](CONTRIBUTING.md) nếu có conflict.

## 7. Khi nào coi là hoàn tất?

Chỉ hoàn tất khi tất cả phần đã nối được, các ví dụ [kiểm tra theo PDF](docs/verification.md) cho kết quả đúng, TODO của bài đã được xử lý, mỗi người giải thích được code mình làm và bộ nộp bài đủ report/patch/source sạch.

Giữ file `README` gốc: `grep hello README` trong xv6 đọc file đó, không đọc `README.md` này. Số byte trả về có thể khác snapshot trong PDF vì nội dung `README` của source MIT khác. Không sửa kết quả `read` hoặc file mẫu để ép các con số giống nhau.

`make grade` và `grade-lab-syscall` đi kèm starter là bộ chấm của MIT, có thêm PID, trace con và `sysinfo`; chúng không phải thang điểm Lab0 của lớp. Checklist của nhóm bám PDF của thầy.

## 8. Điều phối nhóm

Mỗi đầu việc có một Issue làm nguồn TODO chính. Labels: `todo`, `in-progress`, `needs-review`, `blocked`; Issue đóng là đã xong. Khi cập nhật tiến độ, ghi ba ý: đã làm gì, sẽ làm gì tiếp, đang vướng gì.

Trưởng nhóm theo dõi phụ thuộc và tổ chức review. Nếu một người gặp khó, cả nhóm giải thích/hỗ trợ rồi để người phụ trách hiểu và hoàn thiện phần đó. Mỗi thành viên chịu trách nhiệm về code do AI hỗ trợ sinh ra.

Mã nguồn và giấy phép xv6 gốc được giữ trong repo: [README gốc](README), [LICENSE](LICENSE). Hướng dẫn tham khảo của MIT: [lab syscall 2023](https://pdos.csail.mit.edu/6.1810/2023/labs/syscall.html).
