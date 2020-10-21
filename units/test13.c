#include "../ubench.h/ubench.h"
#if 0
UBENCH(dbj_test_13, do_nothing) {
  static char a[8 * 1024 * 1024];
  static char b[8 * 1024 * 1024];
  UBENCH_DO_NOTHING(a);
  memcpy(b, a, sizeof(a));
  UBENCH_DO_NOTHING(b);
}

struct dbj_fixture_13 {
  char *data;
};

UBENCH_F_SETUP(dbj_fixture_13) {
  const int size = 128 * 1024 * 1024;
  ubench_fixture->data = (char *)(malloc(size));
  memset(ubench_fixture->data, ' ', size - 2);
  ubench_fixture->data[size - 1] = '\0';
  ubench_fixture->data[size / 2] = 'f';
}

UBENCH_F_TEARDOWN(dbj_fixture_13) { free(ubench_fixture->data); }

UBENCH_F(dbj_fixture_13, strchr) {
  UBENCH_DO_NOTHING(strchr(ubench_fixture->data, 'f'));
}

UBENCH_F(dbj_fixture_13, strrchr) {
  UBENCH_DO_NOTHING(strrchr(ubench_fixture->data, 'f'));
}

#endif // 0