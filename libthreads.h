#ifndef __LIBTHREADS_H__
#define __LIBTHREADS_H__

#include <stdio.h>
#include <ucontext.h>

#ifdef CONFIG_DEBUG
#define DBG() do { printf("Here: %s, L%d\n", __func__, __LINE__); } while (0)
#define DEBUG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define DBG()
#define DEBUG(fmt, ...)
#endif

struct thread {
	void (*start_routine);
	void *arg;
	ucontext_t context;
	void *stack;
	int index;
	int completed;
};

int thread_create(struct thread *t, void (*start_routine), void *arg);
void thread_start(struct thread *t);

#endif /* __LIBTHREADS_H__ */
