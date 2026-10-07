#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static int
parse_mask(char *s, int *mask)
{
  int i, value, digit;

  value = 0;
  if (s[0] == '\0')
    return -1;

  for (i = 0; s[i] != '\0'; i++) {
    if (s[i] < '0' || s[i] > '9')
      return -1;
    digit = s[i] - '0';
    if (value > (0x7fffffff - digit) / 10)
      return -1;
    value = value * 10 + digit;
  }

  *mask = value;
  return 0;
}

int main(int argc, char *argv[]) {
  int i, mask;
  char *nargv[MAXARG];

  // 1. Kiểm tra đối số đầu vào
  if (argc < 3 || parse_mask(argv[1], &mask) < 0) {
    fprintf(2, "Usage: %s mask command\n", argv[0]);
    exit(1);
  }

  // 2. Chặn lỗi tràn mảng (cần 1 slot cho phần tử NULL kết thúc)
  if (argc - 2 >= MAXARG) {
    fprintf(2, "%s: too many arguments\n", argv[0]);
    exit(1);
  }

  // 3. Gọi syscall trace nạp mask vào kernel
  if (trace(mask) < 0) {
    fprintf(2, "%s: trace failed\n", argv[0]);
    exit(1);
  }

  // 4. Sao chép danh sách tham số của lệnh con
  for (i = 2; i < argc && i < MAXARG; i++) {
    nargv[i - 2] = argv[i];
  }
  nargv[i - 2] = 0;

  // 5. Thực thi lệnh đích
  exec(nargv[0], nargv);

  // 6. Nếu exec thất bại (ví dụ không tìm thấy file)
  fprintf(2, "exec %s failed\n", nargv[0]);
  exit(1);
}
