#include "BVH.h"
#include "Ray.h"
#include "Console.h"
#include "TriangleMesh.h"

BVHNode::BVHNode() {
}

BVHNode::~BVHNode() {}

bool xCompare(Box* left, Box* right) { return (left->getMidpoint().x < right->getMidpoint().x); }
bool yCompare(Box* left, Box* right) { return (left->getMidpoint().y < right->getMidpoint().y); }
bool zCompare(Box* left, Box* right) { return (left->getMidpoint().z < right->getMidpoint().z); }


/*
void build(const std::vector<Intersectable *> &objects)
• Create new vector for Intersectable pointer copies
• Create new vector for the nodes
• Create Root node
• worldBox = AABB(); // world bounding box
• For each intersectable[i] in objects
– worldBox.include(intersectable[i] bounding box)
– Objs.push_back(intersectable[i])
• EndFor
• Set world bounding box to root node
• build_recursive(0, Objs.size(), root, 0);
*/

void
BVH::build(Objects * objs)
{
    // construct the bounding volume hierarchy
    Objects::iterator it;
	// store vector for our objects into BVH
    m_objects = objs;

	// vector for the BVHNodes we'll create as we create the hierarchy
	nodes = new std::vector<BVHNode*>();

	root = new BVHNode();
	Vector3 worldMax, worldMin;
	// Set BB min/max to arbitrarily small/large numbers so they are written over.
	worldMax = { -10000.0f, -10000.0f, -10000.0f };
	worldMin = { 10000.0f, 10000.0f, 10000.0f };

	// Holds the boxes that are within the box. In this case, it holds all the boxes since we are dealing with the root node
	std::vector<Box*>* containerTemp = new std::vector<Box*>();

	boxes = new std::vector<Box*>();
	for (it = m_objects->begin(); it != m_objects->end(); it++)
	{
		Vector3 tempMin, tempMax;
		Object* pObject = *it;
		pObject->preCalc();

		// This will find the min/max of the world box
		pObject->calcBBMinMax(worldMin, worldMax, 1);

		// This will find min/max of current object box
		pObject->calcBBMinMax(tempMin, tempMax, 0);
		//std::cout << "MIN X: " << tempMin.x << " Y: " << tempMin.y << " Z: " << tempMax.z << std::endl;
		//std::cout << "MAX X: " << tempMax.x << " Y: " << tempMax.y << " Z: " << tempMax.z << std::endl;
		Box* tempBox = new Box(tempMin, tempMax);
		tempBox->calcMidpoint();
		containerTemp->push_back(tempBox);
		boxes->push_back(tempBox);
	}
	// create the world bounding box with the newfound worldmin/worldmax
	worldBox = new Box(worldMin, worldMax);
	worldBox->setContainer(containerTemp);
	//std::cout << "minx: " << min.x << "miny: " << min.y << "minx: " << min.z << "maxx: " << max.x << "maxy: " << max.y << "maxx: " << max.z << std::endl;
	root->setBox(worldBox);
	nodes->push_back(root);

	int leftIndex = 0;
	int rightIndex = m_objects->size();
	build_recursive(leftIndex, rightIndex, root, 0);
}

/*
void build_recursive(int left_index, int right_index, BVHNode *node, int depth)
• If((right_index – left_index) <= Threshold || (other termination criteria))
– Initiate current node as a leaf with primitives from Objs[left_index] to Objs[right_index]
• Else
– Split intersectables into left and right by finding a split_index
• Make sure that neither left nor right is completely empty
– Calculate bounding boxes of left and right sides
– Create two new nodes, leftNode and rightNode and assign bounding boxes
– Initiate current node as an interior node with leftNode and rightNode as children
– build_recursive(left_index, split_index, leftNode, depth + 1)
– build_recursive(split_index, right_index, rightNode, depth + 1)
• EndIf
*/

void BVH::build_recursive(int leftIndex, int rightIndex, BVHNode *node, int depth)
{
	// Check if the current node can continue splitting or should stop. 
	int triangleThreshhold = 8;
	int currentCapacity = rightIndex - leftIndex;
	if (currentCapacity <= triangleThreshhold) //  || (other termination criteria))
	{
		//std::cout << "hi!\n";
	}

	// Otherwise continue looping, creating children nodes until we reach said threshold
	else 
	{

		// Find largest dimension, then split the primaries into left/right boxes from the midpoint of the largest dimension
		char largestDimensionChar;
		float dimensionMidpoint;
		Box* currentBox = node->getBox();
		largestDimensionChar = currentBox->largestDimensionChar();

		// Sort the boxes by the axis of the largest dimension
		if (largestDimensionChar == 'x')
		{
			//std::cout << "X!\n";
			std::sort(boxes->begin(), boxes->begin()+rightIndex-1, xCompare);
			dimensionMidpoint = currentBox->getMidpoint().x;
		}
		else if (largestDimensionChar == 'y')
		{
			//std::cout << "Y!\n";
			std::sort(boxes->begin(), boxes->begin()+rightIndex - 1, yCompare);
			dimensionMidpoint = currentBox->getMidpoint().y;
		}
		else if (largestDimensionChar == 'z')
		{
			//std::cout << "Z!\n";
			std::sort(boxes->begin(), boxes->begin()+rightIndex - 1, zCompare);
			dimensionMidpoint = currentBox->getMidpoint().z;
		}
		//std::cout << "boxes size: " << boxes->size() << std::endl;
		for (int i = 0; i < boxes->size(); i++)
		{
			Box* tempBox = boxes->at(i);
			//std::cout << "box.x = " << tempBox->getMidpoint().x << std::endl;
		}
	}
}
bool
BVH::intersect(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
    // Here you would need to traverse the BVH to perform ray-intersection
    // acceleration. For now we just intersect every object.

    bool hit = false;
    HitInfo tempMinHit;
    minHit.t = MIRO_TMAX;
    
    for (size_t i = 0; i < m_objects->size(); ++i)
    {
        if ((*m_objects)[i]->intersect(tempMinHit, ray, tMin, tMax))
        {
            hit = true;
            if (tempMinHit.t < minHit.t)
                minHit = tempMinHit;
        }
    }
    
    return hit;
}

void BVH::drawBox(Vector3 min, Vector3 max) {
	glColor3f(1, 1, 1);
	glPushMatrix();
	glTranslatef((max.x - min.x) /2, (max.y - min.y) / 2, (max.z - min.z) / 2);
	glutWireCube(max.x);
	glPopMatrix();
}