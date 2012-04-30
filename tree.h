#ifndef __TREE_H__
#define __TREE_H__

#include <set>
#include <map>
#include "threads.h"

typedef thread_id_t tree_t;
#define TREE_T_NONE	THREAD_ID_T_NONE

/*
 * An n-ary tree
 *
 * A tree with n possible branches from each node - used for recording the
 * execution paths we've executed / backtracked
 */
class TreeNode {
public:
	TreeNode(TreeNode *par);
	~TreeNode();
	bool hasBeenExplored(tree_t id) { return children.find(id) != children.end(); }
	TreeNode * exploreChild(tree_t id);
	tree_t getNextBacktrack();

	/* Return 1 if already in backtrack, 0 otherwise */
	int setBacktrack(tree_t id);
	TreeNode * getRoot();
	static int getTotalNodes() { return TreeNode::totalNodes; }
private:
	TreeNode *parent;
	std::map<tree_t, class TreeNode *> children;
	std::set<tree_t> backtrack;
	static int totalNodes;
};

#endif /* __TREE_H__ */
