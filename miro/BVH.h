#ifndef CSE168_BVH_H_INCLUDED
#define CSE168_BVH_H_INCLUDED

#include "Miro.h"
#include "Object.h"
#include "Box.h"
#include "Triangle.h"
#include <algorithm>

class BVHNode {
private:
	Box* box;
	bool leaf;
	unsigned int n_objs;
	unsigned int index; // if leaf == false: index to left child node,
						// else if leaf == true: index to first Intersectable in Objs vector
public:
	BVHNode();
	virtual ~BVHNode();
	void setBox(Box* boxxy) { box = boxxy; }
	void makeLeaf(unsigned int index_, unsigned int n_objs_) {  }
	void makeNode(unsigned int left_index_, unsigned int n_objs) {  }
	// n_objs in makeNode is for debug purposes only, and may be omitted later on
	bool isLeaf() { return leaf; }
	unsigned int getIndex() { return index; }
	unsigned int getNObjs() { return n_objs; }
	Box* getBox() { return box; };
};

class BVH
{
public:
    void build(Objects * objs);
	void build_recursive(int leftIndex, int rightIndex, BVHNode *node, int depth);
    bool intersect(HitInfo& result, const Ray& ray,
                   float tMin = 0.0f, float tMax = MIRO_TMAX) const;

	void drawBox(Vector3 min, Vector3 max);
protected:
    Objects * m_objects;
	std::vector<BVHNode*>* nodes;
	Box* worldBox;
	std::vector<Box*>* boxes;
	BVHNode* root;
};


#endif // CSE168_BVH_H_INCLUDED
