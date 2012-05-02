#include <stdio.h>

#include "libthreads.h"
#include "libatomic.h"

static void a(atomic_int *obj)
{
	int i;
	int ret;

	for (i = 0; i < 10; i++) {
		printf("Thread %d, loop %d\n", thrd_current(), i);
		switch (i % 4) {
		case 1:
			ret = atomic_load(obj);
			printf("Read value: %d\n", ret);
			break;
		case 3:
			atomic_store(obj, i);
			printf("Write value: %d\n", i);
			break;
		}
	}
}

void user_main()
{
	thrd_t t1, t2;
	atomic_int obj;

	atomic_init(&obj, 0);

	printf("Thread %d: creating 2 threads\n", thrd_current());
	thrd_create(&t1, (thrd_start_t)&a, &obj);
	thrd_create(&t2, (thrd_start_t)&a, &obj);

	thrd_join(t1);
	thrd_join(t2);
	printf("Thread %d is finished\n", thrd_current());
}
