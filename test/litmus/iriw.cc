#include <stdio.h>
#include <threads.h>
#include <atomic>

std::atomic_int x;
std::atomic_int y;

static void a(void *obj)
{
	x.store(1, std::memory_order_release);
}

static void b(void *obj)
{
	y.store(1, std::memory_order_release);
}

static void c(void *obj)
{
	printf("x1: %d\n", x.load(std::memory_order_acquire));
	printf("y1: %d\n", y.load(std::memory_order_acquire));
}

static void d(void *obj)
{
	printf("y2: %d\n", y.load(std::memory_order_acquire));
	printf("x2: %d\n", x.load(std::memory_order_acquire));
}

int user_main(int argc, char **argv)
{
	thrd_t t1, t2, t3, t4;

	atomic_init(&x, 0);
	atomic_init(&y, 0);

	printf("Main thread: creating 4 threads\n");
	thrd_create(&t1, (thrd_start_t)&a, NULL);
	thrd_create(&t2, (thrd_start_t)&b, NULL);
	thrd_create(&t3, (thrd_start_t)&c, NULL);
	thrd_create(&t4, (thrd_start_t)&d, NULL);

	thrd_join(t1);
	thrd_join(t2);
	thrd_join(t3);
	thrd_join(t4);
	printf("Main thread is finished\n");

	return 0;
}
