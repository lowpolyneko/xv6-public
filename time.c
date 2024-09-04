#include "user.h"

int main(int argc, char *argv[]) {
	uint64 delta, time;

	time = __builtin_ia32_rdtsc();

	if (fork()) {
		/* parent */
		wait();
	} else {
		/* child */
		if (argc < 2)
			exec(0, 0);
		else
			exec(argv[1], argv + 2);
	}

	delta = __builtin_ia32_rdtsc() - time;

	/* stdout = 1 */
	printf(1, "%s took %d cycles to run.\n", argv[1], delta);

	exit();
}
