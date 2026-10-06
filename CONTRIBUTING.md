# Cách làm việc trong nhóm

## Một task, một chủ nhiệm, một nhánh

Issue là nơi giao việc và cập nhật tiến độ. Ghi người phụ trách, file sở hữu, yêu cầu PDF, giao diện dùng chung, tiêu chí hoàn tất và phụ thuộc. Không tạo thêm bảng TODO có trạng thái khác với Issues.

`syscall` là nhánh tích hợp. Quang dùng `wuang`; Dũng dùng `dung/trace-user-command`; Khanh dùng `khanh/trace-process-mask`. Các nhánh này phục vụ task hiện tại; sau khi merge có thể xóa và tạo nhánh khác cho task tiếp theo. `main` giữ commit ban đầu và không dùng để nhận PR của bài.

Labels tiến độ: `todo` -> `in-progress` -> `needs-review`; khi có trở ngại dùng `blocked` và viết rõ cần gì để tiếp tục. Khi thực sự hoàn tất thì đóng Issue. Nếu build còn chờ task khác, ghi phụ thuộc và mở Draft PR.

## Bắt đầu và cập nhật nhánh

Trước khi tạo nhánh, commit phần đang làm hoặc xử lý thay đổi có chủ đích; không đổi nhánh khi chưa biết những thay đổi đó thuộc về đâu.

```bash
git status
git fetch origin
git switch syscall
git pull --ff-only origin syscall
git switch -c dung/trace-user-command
```

Quang dùng `git switch wuang` thay cho lệnh tạo nhánh cuối. Khanh dùng tên nhánh riêng của mình. Khi nhánh chung đã nhận code mới, từ nhánh task đang làm:

```bash
git fetch origin
git merge origin/syscall
```

Nếu có conflict, đọc các phần xung đột và trao đổi với người sở hữu file. Sau khi sửa, `git add <file-da-sua>` rồi `git commit`. Có thể `git merge --abort` để quay về trạng thái trước lần merge. Không ghi đè file hoặc force-push để né conflict.

## Commit và pull request

Dùng commit ngắn, nói điều đã thay đổi: `feat: add trace user command`, `feat: store per-process trace mask`, `fix: terminate exec argument list`, `docs: explain trace examples`.

Trước commit, đọc `git diff`, stage các file được giao bằng `git add <file...>`, rồi đọc `git diff --cached`. Push lên nhánh của mình. Không push trực tiếp code task lên `syscall`.

Mở PR trên GitHub với base `syscall`, compare nhánh task. Điền mẫu sẵn có:

- Issue được giải quyết; dùng `Closes #<số>` khi PR giải quyết toàn bộ Issue, hoặc `Refs #<số>` nếu mới làm một phần.
- Hành vi và các file thay đổi.
- Lệnh đã chạy và output quan sát được; nếu chưa chạy, ghi chưa chạy.
- Phần còn phụ thuộc, điều chưa chắc và cách giải thích khi demo.

Quang review PR của Dũng/Khanh; PR của Quang cần Dũng hoặc Khanh review. Người review kiểm tra phạm vi PDF, giao diện, xử lý lỗi và việc tác giả hiểu code. Số file hoặc dòng code không đại diện đầy đủ cho khối lượng đóng góp.

Các phần user/kernel có phụ thuộc hai chiều lúc build: nhóm có thể xem cả ba Draft PR trước, thử tích hợp cùng nhau, rồi merge theo thứ tự đã thống nhất. Không bắt một thành viên sửa toàn bộ phần của người khác để PR của mình build độc lập.

Trước merge, các góp ý đã được xử lý, phạm vi file đúng và cả nhóm biết trạng thái kiểm tra. Kết luận toàn bài chạy đúng chỉ sau khi các phần đã tích hợp và checklist PDF đã được thực hiện.

## Format code và tài liệu

Giữ style của xv6, tên API đã thống nhất và newline LF. Không tự chạy formatter toàn repo. Không sửa `user/usys.S` bằng tay. Không đổi `README` gốc vì đó là dữ liệu trong ví dụ grep.

Tài liệu Markdown dùng tiếng Việt rõ ràng; tên biến/hàm/file giữ theo code. Report ngắn, không chép source code. Không commit file được build/sinh ra hoặc gói nộp bài.

## Dùng AI và cập nhật tiến độ

Mỗi người đưa cho AI cùng quy tắc trong `AGENTS.md` và task của mình. Với AI không tự đọc repo, đính kèm hoặc dán phần tài liệu/code liên quan. Dùng mẫu trong `docs/ai-prompts.md`.

Mỗi lần cập nhật Issue, dùng ba dòng:

```text
Đã làm: ...
Tiếp theo: ...
Đang vướng/cần hỗ trợ: ...
```

Mỗi thành viên tự viết phần giải thích của mình và chỉ ra vị trí code. Cả ba cần hiểu luồng `trace` -> stub -> syscall kernel -> lưu mask -> exec -> log, vì demo đánh giá từng người.

Xem tham khảo [GitHub Flow](https://docs.github.com/en/get-started/using-github/github-flow) và [mẫu Issue/PR](https://docs.github.com/en/communities/using-templates-to-encourage-useful-issues-and-pull-requests/about-issue-and-pull-request-templates).
