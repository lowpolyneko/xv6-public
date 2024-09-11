#include "user.h"

static inline uint64 __rdtscp(void) {
	uint32 hi, lo;
	__asm__ __volatile__ ("rdtscp" : "=a"(lo), "=d"(hi) :: "%rcx");
	return ( (uint64)lo)|( ((uint64)hi)<<32 );
}

int main(int argc, char *argv[]) {
	uint64 delta, time;

	time = __rdtscp();

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

	delta = __rdtscp() - time;

	/* stdout = 1 */
	printf(1, "%s took %u cycles to run.\n", argv[1], delta);

	exit();
}
