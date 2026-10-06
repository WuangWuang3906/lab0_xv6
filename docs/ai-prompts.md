# Prompt chung cho các thành viên

Dùng cùng tài liệu và Issue để AI có cùng bối cảnh. Thay các phần trong dấu <...> bằng nội dung task. Nếu AI không tự đọc repo, dán hoặc đính kèm các file liên quan; chỉ một đường dẫn trên máy bạn không giúp AI ở máy khác thấy nội dung.

## 1. Đọc hiểu trước khi code

```text
Tôi là <Quang/Dũng/Khanh>, đang làm Lab0 trace trong repo lab06_xv6.
Hãy đọc AGENTS.md, docs/lab0-overview.md, docs/work-plan.md,
Issue <số/link và nội dung> và các file thuộc task.

Giải thích bằng tiếng Việt dễ hiểu:
- Mục tiêu phần tôi làm và nó nằm ở đâu trong luồng trace.
- Mã hiện có đang làm gì; TODO nào thuộc về tôi.
- API/tên biến phải dùng chung và phần phụ thuộc người khác.
- Những lỗi dễ gặp và cách quan sát kết quả.

Lượt này chỉ đọc và giải thích. Chỉ rõ điều lấy từ PDF và điều là
quy ước nhóm; nếu thiếu nội dung file, nói rõ file cần được cung cấp.
```

## 2. Thực hiện một task

```text
Tôi làm Issue <số/link>. Nội dung task:
<dán mục tiêu, files, giao diện, tiêu chí hoàn tất và phụ thuộc>.

Đọc AGENTS.md và các tài liệu liên quan trước khi sửa.
Nguồn yêu cầu là docs/assignment/Lab0_System_Call.pdf;
chỉ làm trace theo PDF. Dùng đúng giao diện trong docs/work-plan.md.

Hãy thực hiện phần được giao trên nhánh <tên nhánh>.
Chỉ sửa <danh sách file>. Tôn trọng TODO và file của người khác.
Nếu muốn sửa file khác, nêu lý do để nhóm thống nhất trước.
Không làm cả đồ án để giải quyết một phụ thuộc còn chưa merge.

Giữ style xv6. Sửa user/usys.pl, không sửa usys.S được sinh ra.
Kiểm tra phần có thể chạy trong môi trường hiện tại; nếu task còn
chờ phần khác, ghi rõ kết quả và phần chưa thể kiểm tra.

Kết thúc hãy báo:
1. File và hành vi đã thay đổi, với lý do.
2. Lệnh đã chạy và kết quả thực tế.
3. Phụ thuộc/điểm chưa chắc còn lại.
4. Giải thích code để tôi hiểu và có thể demo bằng lời của mình.
```

## 3. Review một PR

```text
Hãy review PR <link hoặc diff> của Issue <số/nội dung>.
Đối chiếu PDF, AGENTS.md và docs/work-plan.md.

Kiểm tra phạm vi files, giao diện giữa user/kernel, mask đúng process,
vị trí in log và giá trị trả về, argv/exec nếu PR thuộc phía user,
vòng đời process nếu PR thuộc phần mask.

Trước tiên báo lỗi có ảnh hưởng hành vi, ghi file/vị trí và tình huống
xảy ra. Phân biệt lỗi bắt buộc sửa với gợi ý dễ đọc. Không bổ sung
yêu cầu MIT ngoài PDF. Nếu thiếu code/context hoặc chưa chạy kiểm tra,
nói rõ giới hạn đó. Lượt này chỉ review, không sửa code.
```

## 4. Quang rà tích hợp cuối

```text
Rà phần trace đã tích hợp trên nhánh syscall.
Đọc PDF và docs/verification.md, so với diff từ tag lab0-base.
Kiểm tra toàn bộ đường đi user -> stub -> handler -> process mask ->
exec -> dispatcher/log. Chỉ phạm vi trace theo PDF.

Thực hiện các kiểm tra phù hợp có trong checklist, lưu lệnh và kết quả.
Tìm TODO còn lại, phần giao diện chưa nối, dữ liệu process bị rò và
trường hợp wrapper không truyền đủ argv. Không sửa README gốc hay
chương trình grep để khớp các số byte mẫu của snapshot khác.

Báo yêu cầu nào đã có bằng chứng, yêu cầu nào còn thiếu, và các lỗi
cụ thể cần sửa trước khi nộp. Chưa tạo gói nộp nếu code chưa sạch.
```

Quang có thể dùng AI hỗ trợ chung cho nhóm: chuẩn hóa bối cảnh, giải thích, review và tổng hợp tài liệu. Mỗi thành viên vẫn đọc diff, hiểu và chịu trách nhiệm phần mình. Không cần chia sẻ tài khoản hoặc mật khẩu AI.
