/*
 * This test performs some relaxes, release, acquire opeations on a single
 * atomic variable. It can give some rough idea of release sequence support but
 * probably should be improved to give better information.
 */

#include <stdio.h>

#include "libthreads.h"
#include "librace.h"
#include "stdatomic.h"

atomic_int x;

static void a(void *obj)
{
	atomic_store_explicit(&x, 1, memory_order_release);
	atomic_store_explicit(&x, 42, memory_order_relaxed);
}

static void b(void *obj)
{
	int r = atomic_load_explicit(&x, memory_order_acquire);
	printf("r = %u\n", r);
}

static void c(void *obj)
{
	atomic_store_explicit(&x, 2, memory_order_relaxed);
}

void user_main()
{
	thrd_t t1, t2, t3;

	atomic_init(&x, 0);

	printf("Thread %d: creating 3 threads\n", thrd_current());
	thrd_create(&t1, (thrd_start_t)&a, NULL);
	thrd_create(&t2, (thrd_start_t)&b, NULL);
	thrd_create(&t3, (thrd_start_t)&c, NULL);

	thrd_join(t1);
	thrd_join(t2);
	thrd_join(t3);
	printf("Thread %d is finished\n", thrd_current());
}
