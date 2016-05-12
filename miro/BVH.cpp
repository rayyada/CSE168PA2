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

		// create box for each primary, give pointer to primary for the box, then push box into a vector we can iterate through for creating the BVH
		Box* tempBox = new Box(tempMin, tempMax, true);
		tempBox->calcMidpoint();
		tempBox->insertPrimary(pObject);
		containerTemp->push_back(tempBox);
		boxes->push_back(tempBox);
	}
	// create the world bounding box with the newfound worldmin/worldmax
	worldBox = new Box(worldMin, worldMax, false);
	worldBox->setContainer(containerTemp);
	//std::cout << "minx: " << min.x << "miny: " << min.y << "minx: " << min.z << "maxx: " << max.x << "maxy: " << max.y << "maxx: " << max.z << std::endl;
	root->setBox(worldBox);
	root->makeNode(0, 1);	
	nodes->push_back(root);

	int leftIndex = 0;
	int rightIndex = m_objects->size();
	build_recursive(leftIndex, rightIndex, root, 0);
	int checkLeaf = 0;
	for (int i = 0; i < nodes->size(); i++)
	{
		BVHNode * tempNode = nodes->at(i);
		if (tempNode->isLeaf())
		{
			checkLeaf++;
			std::cout << "leaf leftIndex: " << tempNode->getIndex() << " numObjs: " << tempNode->getNObjs() << std::endl;
		}
		Box * tempBox = tempNode->getBox();
		//std::cout << "Node NObjs: " << tempNode->getNObjs() << " Node getIndex: " << tempNode->getIndex() << " Box SA: " << tempBox->getSA() << std::endl;
	}
	std::cout << "nodes size: " << nodes->size() << " numLeafs: " << checkLeaf <<  std::endl;
	
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

	// We want 1-8 triangles per box, so stop whenever the node reaches said limit and set them as leaf nodes
	if ((rightIndex - leftIndex) <= 8) //  || (other termination criteria))
	{
		//std::cout << "STOP!\n";
		node->makeLeaf(leftIndex, (rightIndex - leftIndex));
	}

	// Otherwise continue looping, creating children nodes until we reach said threshold
	else 
	{

		//std::vector<Box*>* containerLeft = new std::vector<Box*>();
		//std::vector<Box*>* containerRight = new std::vector<Box*>();

		Vector3 leftMin, leftMax, rightMin, rightMax;

		// Find largest dimension, then split the primaries into left/right boxes from the midpoint of the largest dimension
		char largestDimensionChar;
		float dimensionMidpoint;
		float minCost = 100000000.0f, minCostX = 100000000.0f, minCostY = 100000000.0f, minCostZ = 100000000.0f;
		float tempCostX, tempCostY, tempCostZ;
		float splitX, splitY, splitZ;
		
		int numItemsLeft, numItemsRight;
		Box* currentBox = node->getBox();
		int splitIndex = -1;
		//largestDimensionChar = currentBox->largestDimensionChar();
		// Sort the boxes by the axis of the largest dimension, then find the index where we need to split. 
		// In this algo, we use the midpoint of the parent node to determine which side a box belongs to

		// SAH Look through X-Axis for lowest cost split
		// sort by the midpoint of the boxes that contains the primary

		std::sort(boxes->begin() + leftIndex, boxes->begin() + (rightIndex - 1), xCompare);
		// end loop at rightIndex - 1 so both boxes have at least one object in them when comparing. Also prevents out of scope memory segfaults
		for (int i = leftIndex; i < rightIndex - 1; i++)
		{
			// Get bounding box min/max, create temporary boxes for temp left/right children, calculate SAH, store min costs
			BBMinMax(leftMin, leftMax, leftIndex, i + 1);
			BBMinMax(rightMin, rightMax, i + 1, rightIndex);
			numItemsLeft = i + 1 - leftIndex;
			numItemsRight = rightIndex - 1 - i;
			Box tempBoxLeft = Box(leftMin, leftMax, false);
			Box tempBoxRight = Box(rightMin, rightMax, false);
			tempCostX = SAHCalculate(currentBox, &tempBoxLeft, &tempBoxRight, numItemsLeft, numItemsRight);
			minCostX = std::min(minCostX, tempCostX);
			if (minCostX == tempCostX)
			{
				splitX = i + 1;
			}
			//std::cout << "tempCostX: " << tempCostX << " minCostX: " << minCostX << " numItemsLeft: " << numItemsLeft << " numItemsRight: " << numItemsRight << std::endl;
		}

		std::sort(boxes->begin() + leftIndex, boxes->begin() + (rightIndex - 1), yCompare);
		for (int i = leftIndex; i < rightIndex-1; i++)
		{
			BBMinMax(leftMin, leftMax, leftIndex, i + 1);
			BBMinMax(rightMin, rightMax, i+1, rightIndex);
			numItemsLeft = i + 1 - leftIndex;
			numItemsRight = rightIndex - i;
			Box tempBoxLeft = Box(leftMin, leftMax, false);
			Box tempBoxRight = Box(rightMin, rightMax, false);
			tempCostY = SAHCalculate(currentBox, &tempBoxLeft, &tempBoxRight, numItemsLeft, numItemsRight);
			minCostY = std::min(minCostY, tempCostY);
			if (minCostY == tempCostY)
			{
				splitY = i + 1;
			}
			//std::cout << "tempCostX: " << tempCostY << " minCostX: " << minCostY << " numItemsLeft: " << numItemsLeft << " numItemsRight: " << numItemsRight << std::endl;
		}

		std::sort(boxes->begin() + leftIndex, boxes->begin() + (rightIndex - 1), zCompare);
		for (int i = leftIndex; i < rightIndex - 1; i++)
		{
			BBMinMax(leftMin, leftMax, leftIndex, i + 1);
			BBMinMax(rightMin, rightMax, i + 1, rightIndex);
			numItemsLeft = i + 1 - leftIndex;
			numItemsRight = rightIndex - i;
			Box tempBoxLeft = Box(leftMin, leftMax, false);
			Box tempBoxRight = Box(rightMin, rightMax, false);
			tempCostZ = SAHCalculate(currentBox, &tempBoxLeft, &tempBoxRight, numItemsLeft, numItemsRight);
			minCostZ = std::min(minCostZ, tempCostZ);
			if (minCostZ == tempCostZ)
			{
				splitZ = i + 1;
			}
			//std::cout << "tempCostX: " << tempCostZ << " minCostX: " << minCostZ << " numItemsLeft: " << numItemsLeft << " numItemsRight: " << numItemsRight << std::endl;
		}

		//std::cout << "minCostX: " << minCostX << " minCostY: " << minCostY << " minCostZ: " << minCostZ << " leftIndex: " << leftIndex << " rightIndex: " << rightIndex << std::endl;

		// Determine which axis had the least cost and proceed from there
		minCost = std::min(std::min(minCostX, minCostY), minCostZ);
		if (minCost == minCostX)
		{
			// Sort again now that we know which axis to use 
			std::sort(boxes->begin() + leftIndex, boxes->begin() + (rightIndex - 1), xCompare);
			BBMinMax(leftMin, leftMax, leftIndex, splitX);
			BBMinMax(rightMin, rightMax, splitX, rightIndex);

			// Create boxes and nodes 
			Box* leftBox = new Box(leftMin, leftMax, false);
			Box* rightBox = new Box(rightMin, rightMax, false);
			BVHNode* leftNode = new BVHNode();
			BVHNode* rightNode = new BVHNode();

			// Gives current node attributes so it knows where to find it's child nodes
			node->makeNode(nodes->size(), (rightIndex - leftIndex));
			leftNode->setBox(leftBox);
			rightNode->setBox(rightBox);
			nodes->push_back(leftNode);
			nodes->push_back(rightNode);

			build_recursive(leftIndex, splitX, leftNode, depth + 1);
			build_recursive(splitX, rightIndex, rightNode, depth + 1);
			/*
			// SAH test. If cost < number of objects in parent node, continue iterating. else, set children as leaf nodes and return
			if (minCost < node->getNObjs())
			{
				//std::cout << "minCost: " << minCost << " leftIndex: " << leftIndex << " rightIndex: " << rightIndex << std::endl;

				build_recursive(leftIndex, splitX, leftNode, depth + 1);
				build_recursive(splitX, rightIndex, rightNode, depth + 1);
			}
			else
			{
				//std::cout << "LEAF minCost: " << minCost << " leftIndex: " << leftIndex << " rightIndex: " << rightIndex << std::endl;
				leftNode->makeLeaf(leftIndex, (rightIndex - leftIndex));
				rightNode->makeLeaf(splitX, (rightIndex - splitX));
			}
			*/
			//std::cout << "leftBox SA: " << leftBox->getSA() << " numItemsLeft: " << splitX - leftIndex << " rightBox SA : " << rightBox->getSA() << " numItemsRight: " << rightIndex - (splitX) << std::endl;
			//std::cout << "minCost: " << SAHCalculate(currentBox, leftBox, rightBox, splitX - leftIndex, rightIndex - splitX) << std::endl;
		}
		else if (minCost == minCostY)
		{
			std::sort(boxes->begin() + leftIndex, boxes->begin() + (rightIndex - 1), yCompare);
			BBMinMax(leftMin, leftMax, leftIndex, splitY);
			BBMinMax(rightMin, rightMax, splitY, rightIndex);
			Box* leftBox = new Box(leftMin, leftMax, false);
			Box* rightBox = new Box(rightMin, rightMax, false);
			BVHNode* leftNode = new BVHNode();
			BVHNode* rightNode = new BVHNode();
			node->makeNode(nodes->size(), (rightIndex - leftIndex));
			leftNode->setBox(leftBox);
			rightNode->setBox(rightBox);
			nodes->push_back(leftNode);
			nodes->push_back(rightNode);
			build_recursive(leftIndex, splitY, leftNode, depth + 1);
			build_recursive(splitY, rightIndex, rightNode, depth + 1);
			/*
			if (minCost < node->getNObjs())
			{
				//std::cout << "minCost: " << minCost << " leftIndex: " << leftIndex << " rightIndex: " << rightIndex << std::endl;
				build_recursive(leftIndex, splitY, leftNode, depth + 1);
				build_recursive(splitY, rightIndex, rightNode, depth + 1);
			}
			else
			{
				//std::cout << "LEAF minCost: " << minCost << " leftIndex: " << leftIndex << " rightIndex: " << rightIndex << std::endl;
				leftNode->makeLeaf(leftIndex, (rightIndex - leftIndex));
				rightNode->makeLeaf(splitY, (rightIndex - splitY));
			}
			*/
		}
		else
		{
			std::sort(boxes->begin() + leftIndex, boxes->begin() + (rightIndex - 1), zCompare);
			BBMinMax(leftMin, leftMax, leftIndex, splitZ);
			BBMinMax(rightMin, rightMax, splitZ, rightIndex);
			Box* leftBox = new Box(leftMin, leftMax, false);
			Box* rightBox = new Box(rightMin, rightMax, false);
			BVHNode* leftNode = new BVHNode();
			BVHNode* rightNode = new BVHNode();
			node->makeNode(nodes->size(), (rightIndex - leftIndex));
			leftNode->setBox(leftBox);
			rightNode->setBox(rightBox);
			nodes->push_back(leftNode);
			nodes->push_back(rightNode);
			build_recursive(leftIndex, splitZ, leftNode, depth + 1);
			build_recursive(splitZ, rightIndex, rightNode, depth + 1);
			/*
			if (minCost < node->getNObjs())
			{
				//std::cout << "minCost: " << minCost << " leftIndex: " << leftIndex << " rightIndex: " << rightIndex << std::endl;
				build_recursive(leftIndex, splitZ, leftNode, depth + 1);
				build_recursive(splitZ, rightIndex, rightNode, depth + 1);
			}
			else
			{
				//std::cout << "LEAF minCost: " << minCost << " leftIndex: " << leftIndex << " rightIndex: " << rightIndex << std::endl;
				leftNode->makeLeaf(leftIndex, (rightIndex - leftIndex));
				rightNode->makeLeaf(splitZ, (rightIndex - splitZ));
			}
			*/
		}
		/*
		// SAH Look through Y-Axis for lowest cost split
		std::sort(boxes->begin()+ leftIndex, boxes->begin()+ (rightIndex - 1), yCompare);
		dimensionMidpoint = currentBox->getMidpoint().y;
		for (int i = leftIndex; i < rightIndex; i++)
		{
			Box* tempBox = boxes->at(i);
			if (tempBox->getMidpoint().y > dimensionMidpoint)
			{
				splitIndex = i;
				break;
			}
			//std::cout << "box.y = " << tempBox->getMidpoint().x << std::endl;
		}

		// SAH Look through Z-Axis for lowest cost split
		std::sort(boxes->begin() + leftIndex, boxes->begin() + (rightIndex - 1), zCompare);
		dimensionMidpoint = currentBox->getMidpoint().z;
		for (int i = leftIndex; i < rightIndex; i++)
		{
			Box* tempBox = boxes->at(i);
			if (tempBox->getMidpoint().z > dimensionMidpoint)
			{
				splitIndex = i;
				break;
			}
			//std::cout << "box.z = " << tempBox->getMidpoint().x << std::endl;
		}


		// In case we don't find anything on the right side of the split
		if (splitIndex == -1)
		{
			splitIndex = rightIndex;
			std::cout << "HELLO!\n";
		}
		*/
		//std::cout << "boxes size: " << boxes->size() << std::endl;
		//std::cout << "dimensionMidpoint : " << dimensionMidpoint << std::endl;
		//std::cout << "boxes size: " << boxes->size() << " splitIndex : " << splitIndex << std::endl;
		/*
		BBMinMax(leftMin, leftMax, leftIndex, splitIndex);
		BBMinMax(rightMin, rightMax, splitIndex, rightIndex);

		// Setup nodes/boxes for left/right children nodes 
		Box* leftBox = new Box(leftMin, leftMax);
		Box* rightBox = new Box(rightMin, rightMax);
		BVHNode* leftNode = new BVHNode();
		BVHNode* rightNode = new BVHNode();
		//leftNode->makeNode(leftIndex, (splitIndex - leftIndex));
		//rightNode->makeNode(splitIndex, (rightIndex - splitIndex));
		leftNode->setBox(leftBox);
		rightNode->setBox(rightBox); 

		// Set leftchild index and number of objects inside current node
		node->makeNode(nodes->size(), (rightIndex-leftIndex));
		nodes->push_back(leftNode);
		nodes->push_back(rightNode);

		build_recursive(leftIndex, splitIndex, leftNode, depth + 1);
		build_recursive(splitIndex, rightIndex, rightNode, depth + 1);
		*/
		/*
		std::cout << "LEFT MIN X: " << leftMin.x << " Y: " << leftMin.y << " Z: " << leftMin.z << std::endl;
		std::cout << "LEFT MAX X: " << leftMax.x << " Y: " << leftMax.y << " Z: " << leftMax.z << std::endl;
		std::cout << "RIGHT MIN X: " << rightMin.x << " Y: " << rightMin.y << " Z: " << rightMin.z << std::endl;
		std::cout << "RIGHT MAX X: " << rightMax.x << " Y: " << rightMax.y << " Z: " << rightMax.z << std::endl;
		*/


	}
}

float BVH::SAHCalculate(Box * parent, Box * left, Box * right, int numElemLeft, int numElemRight)
{
	float parentSA = parent->getSA();
	float leftSA = left->getSA();
	float rightSA = right->getSA();
	return (((leftSA / parentSA) * numElemLeft) + ((rightSA / parentSA) * numElemRight));
}

/*
void BVHNode::makeNode(unsigned int left_index_, unsigned int n_objs)
{

}
*/
void BVH::BBMinMax(Vector3& min, Vector3& max, int start, int end)
{
	min = boxes->at(start)->getMin();
	max = boxes->at(start)->getMax();
	Vector3 minTemp, maxTemp;
	for (int i = start; i < end; i++)
	{
		Box* tempBox = boxes->at(i);
		minTemp = tempBox->getMin();
		maxTemp = tempBox->getMax();

		min.x = std::min(min.x, minTemp.x);
		min.y = std::min(min.y, minTemp.y);
		min.z = std::min(min.z, minTemp.z);

		max.x = std::max(max.x, maxTemp.x);
		max.y = std::max(max.y, maxTemp.y);
		max.z = std::max(max.z, maxTemp.z);
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