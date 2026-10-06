# Chuẩn bị nộp Lab0

Đối chiếu trang 1 của [PDF gốc](assignment/Lab0_System_Call.pdf). Nộp trực tiếp Moodle của môn; PDF không nhận nộp qua email hay hình thức khác. Nhóm gồm Quang, Dũng và Khanh, không quá 3 người.

## Bộ nộp

Sắp ba mã sinh viên tăng dần. Ví dụ trong PDF là `2312001`, `2312002`, `2312003`; thay bằng mã thực của nhóm trước khi tạo file.

```text
StudentID1_StudentID2_StudentID3.zip
├── StudentID1_StudentID2_StudentID3_Report.pdf
├── StudentID1_StudentID2_StudentID3.patch
└── xv6_source.zip
```

Tên ZIP ngoài và tên report theo PDF. Tên source ZIP ở đây là quy ước nhóm; PDF yêu cầu source xv6 đã make clean nhưng không chỉ định tên riêng cho source ZIP.

Report cần ngắn, nêu điều nhóm hiểu, đóng góp của từng người, vấn đề còn tồn tại hoặc cách đã thử chưa thành công nếu có. Không chép source code vào report. Dùng [report-template.md](report-template.md) làm khung, rồi xuất thành PDF.

## Tạo patch khi code đã commit

Lệnh `git diff` không có mốc chỉ lấy thay đổi chưa commit; nếu toàn bộ bài đã commit thì nó có thể rỗng. Vì vậy repo đã có tag `lab0-base` chỉ tới mã MIT ban đầu. Lệnh dưới lấy toàn bộ thay đổi code từ mốc đó đến nhánh chung.

Sau khi các PR đã merge, ở terminal Ubuntu:

```bash
git fetch origin --tags
git switch syscall
git pull --ff-only origin syscall
git status --short
make clean
```

`git status --short` phải không còn thay đổi chưa commit cần nộp. Nếu còn, xử lý và merge chúng trước; git archive bên dưới chỉ xuất code đã commit trên syscall.

Thay mã ví dụ rồi chạy:

```bash
submission_ids=2312001_2312002_2312003
mkdir -p "submissions/$submission_ids"
git diff lab0-base..syscall -- kernel user Makefile > "submissions/$submission_ids/$submission_ids.patch"
git archive --format=zip --output="submissions/$submission_ids/xv6_source.zip" syscall
```

Patch chỉ chứa phần mã nguồn/build liên quan, không đưa tài liệu điều phối vào diff code. Git archive xuất file được Git theo dõi nên không kèm build/cache/.git; make clean vẫn là bước dọn theo PDF. Không di chuyển tag lab0-base.

Đặt report PDF đúng tên vào cùng thư mục. Có thể mở thư mục này từ Windows, nén đúng ba file thành ZIP ngoài và đặt tên theo ba mã tăng dần. Thư mục submissions và ZIP đã được .gitignore gốc loại khỏi commit.

## Checklist trước khi gửi Moodle

- [ ] Các ví dụ PDF và phần tính riêng của mask đã được kiểm tra sau tích hợp.
- [ ] TODO[Quang]/TODO[Dung]/TODO[Khanh] của bài đã hoàn thiện và xử lý.
- [ ] Report PDF ngắn, đủ tên/mã/đóng góp và không chứa source code.
- [ ] Patch không rỗng và bao gồm toàn bộ thay đổi code cần nộp.
- [ ] ZIP source mở được, có mã hoàn chỉnh đã commit và không có output build.
- [ ] ZIP ngoài/report dùng đúng mã thực theo thứ tự tăng dần.
- [ ] Cả ba đã đọc phần người khác ở mức đủ giải thích luồng trace khi demo.
- [ ] Người được phân công nộp đã kiểm tra file tải lên Moodle và trạng thái nộp.

Hạn nộp và lịch demo không có trong PDF này; lấy từ thông báo môn/Moodle, không tự suy đoán. PDF đánh giá hiểu code và đóng góp từng người, đồng thời cảnh báo các bài giống nhau có thể bị 0 toàn bộ phần thực hành.
