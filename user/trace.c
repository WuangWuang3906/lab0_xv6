#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// TODO[Dung]: Review this provided wrapper before completing the user task.
// Check mask/command arguments, MAXARG, the final NULL in nargv, and exec errors.
int main(int argc, char *argv[]) {
  int i;
  char *nargv[MAXARG];

  // 1. Kiểm tra đối số đầu vào
  if (argc < 3 || (argv[1][0] < '0' || argv[1][0] > '9')) {
    fprintf(2, "Usage: %s mask command\n", argv[0]);
    exit(1);
  }

  // 2. Chặn lỗi tràn mảng (cần 1 slot cho phần tử NULL kết thúc)
  if (argc - 2 >= MAXARG) {
    fprintf(2, "%s: too many arguments\n", argv[0]);
    exit(1);
  }

  // 3. Gọi syscall trace nạp mask vào kernel
  if (trace(atoi(argv[1])) < 0) {
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
