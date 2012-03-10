#ifndef __LIBTHREADS_H__
#define __LIBTHREADS_H__

#include <ucontext.h>

struct thread {
	void (*start_routine);
	void *arg;
	ucontext_t context;
	void *stack;
	int index;
	int completed;
};

int thread_create(struct thread *t, void (*start_routine), void *arg);
void thread_join(struct thread *t);
struct thread *thread_current(void);

#endif /* __LIBTHREADS_H__ */
