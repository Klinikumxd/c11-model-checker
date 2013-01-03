#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <inttypes.h>
#include <vector>

#include "snapshot-interface.h"
#include "snapshot.h"
#include "common.h"
#include "mymemory.h"

/* MYBINARYNAME only works because our pathname usually includes 'model' (e.g.,
 * /.../model-checker/test/userprog.o) */
#define MYBINARYNAME "model"
#define MYLIBRARYNAME "libmodel.so"
#define MAPFILE "/proc/self/maps"

struct stackEntry {
	stackEntry(snapshot_id id, int idx) : snapshotid(id), index(idx) { }
	snapshot_id snapshotid;
	int index;
	MEMALLOC
};

class SnapshotStack {
 public:
	SnapshotStack();
	~SnapshotStack();
	int backTrackBeforeStep(int seq_index);
	void snapshotStep(int seq_index);

	MEMALLOC
 private:
	std::vector<struct stackEntry, ModelAlloc<struct stackEntry> > stack;
};

static SnapshotStack *snapshotObject;

#ifdef MAC
/** The SnapshotGlobalSegments function computes the memory regions
 *	that may contain globals and then configures the snapshotting
 *	library to snapshot them.
 */
static void SnapshotGlobalSegments()
{
	int pid = getpid();
	char buf[9000], execname[100];
	FILE *map;

	sprintf(execname, "vmmap -interleaved %d", pid);
	map = popen(execname, "r");

	if (!map) {
		perror("popen");
		exit(EXIT_FAILURE);
	}

	/* Wait for correct part */
	while (fgets(buf, sizeof(buf), map)) {
		if (strstr(buf, "==== regions for process"))
			break;
	}

	while (fgets(buf, sizeof(buf), map)) {
		char regionname[200] = "";
		char type[23];
		char smstr[23];
		char r, w, x;
		char mr, mw, mx;
		int size;
		void *begin, *end;

		//Skip out at the end of the section
		if (buf[0] == '\n')
			break;

		sscanf(buf, "%22s %p-%p [%5dK] %c%c%c/%c%c%c SM=%3s %200s\n", type, &begin, &end, &size, &r, &w, &x, &mr, &mw, &mx, smstr, regionname);

		if (w == 'w' && (strstr(regionname, MYBINARYNAME) || strstr(regionname, MYLIBRARYNAME))) {
			size_t len = ((uintptr_t)end - (uintptr_t)begin) / PAGESIZE;
			if (len != 0)
				snapshot_add_memory_region(begin, len);
		}
	}
	pclose(map);
}
#else

static void get_binary_name(char *buf, size_t len)
{
	if (readlink("/proc/self/exe", buf, len) == -1) {
		perror("readlink");
		exit(EXIT_FAILURE);
	}
}

/** The SnapshotGlobalSegments function computes the memory regions
 *	that may contain globals and then configures the snapshotting
 *	library to snapshot them.
 */
static void SnapshotGlobalSegments()
{
	char buf[9000];
	char binary_name[800];
	FILE *map;

	map = fopen(MAPFILE, "r");
	if (!map) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}
	get_binary_name(binary_name, sizeof(binary_name));
	while (fgets(buf, sizeof(buf), map)) {
		char regionname[200] = "";
		char r, w, x, p;
		void *begin, *end;

		sscanf(buf, "%p-%p %c%c%c%c %*x %*x:%*x %*u %200s\n", &begin, &end, &r, &w, &x, &p, regionname);
		if (w == 'w' && (strstr(regionname, binary_name) || strstr(regionname, MYLIBRARYNAME))) {
			size_t len = ((uintptr_t)end - (uintptr_t)begin) / PAGESIZE;
			if (len != 0)
				snapshot_add_memory_region(begin, len);
			DEBUG("%55s: %18p - %18p\t%c%c%c%c\n", regionname, begin, end, r, w, x, p);
		}
	}
	fclose(map);
}
#endif

SnapshotStack::SnapshotStack() : stack()
{
	SnapshotGlobalSegments();
}

SnapshotStack::~SnapshotStack()
{
}

/** This method returns to the last snapshot before the inputted
 * sequence number.  This function must be called from the model
 * checking thread and not from a snapshotted stack.
 * @param seqindex is the sequence number to rollback before.
 * @return is the sequence number we actually rolled back to.
 */
int SnapshotStack::backTrackBeforeStep(int seqindex)
{
	int i;
	for (i = (int)stack.size() - 1; i >= 0; i++)
		if (stack[i].index <= seqindex)
			break;
		else
			stack.pop_back();

	ASSERT(i >= 0);
	snapshot_roll_back(stack[i].snapshotid);
	return stack[i].index;
}

/** This method takes a snapshot at the given sequence number. */
void SnapshotStack::snapshotStep(int seqindex)
{
	stack.push_back(stackEntry(seqindex, take_snapshot()));
}

void snapshot_stack_init()
{
	snapshotObject = new SnapshotStack();
}

void snapshot_record(int seq_index)
{
	snapshotObject->snapshotStep(seq_index);
}

int snapshot_backtrack_before(int seq_index)
{
	return snapshotObject->backTrackBeforeStep(seq_index);
}
