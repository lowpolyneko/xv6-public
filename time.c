#include "user.h"

// static inline uint64 __rdtsc(void) {
// 	uint32 hi, lo;
// 	__asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
// 	return ( (uint64)lo)|( ((uint64)hi)<<32 );
// }

int main(int argc, char *argv[]) {
	uint64 delta, time;

	if (argc < 2) {
		/* stdout = 1 */
		printf(1, "specify command to run.");
		exit();
	}

	time = __builtin_ia32_rdtsc();

	if (fork()) {
		/* parent */
		wait();
	} else {
		/* child */
		exec(argv[1], argv + 2);
	}

	delta = __builtin_ia32_rdtsc() - time;
	printf(1, "%s took %d cycles to run.", argv[1], delta);

	return 0;
}
