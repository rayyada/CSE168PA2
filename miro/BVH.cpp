#include "BVH.h"
#include "Ray.h"
#include "Console.h"
#include "TriangleMesh.h"

int BVH::boxInts = 0;
int BVH::triInts = 0;

void BVH::setRays(int i)
{
	numRays = i;
}
void BVH::setBoxI(int i)
{
	rayBoxIntersections = i;
}
void BVH::setTriI(int i)
{
	rayTriangleIntersections = i;
}
int BVH::getRays()
{
	return numRays;
}
int BVH::getBoxI()
{
	return boxInts;
}
int BVH::getTriI()
{
	return triInts;
}

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
	//worldBox->setContainer(containerTemp);
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
		}
	}
	std::cout << "BVH Nodes: " << nodes->size() << " BVH Node Leafs: " << checkLeaf <<  std::endl;
	
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
	//std::cout << "STOP!\n";
	// Check if the current node can continue splitting or should stop. 
	int triangleThreshhold = 8;
	int currentCapacity = rightIndex - leftIndex;

	// We want 1-8 triangles per box, so stop whenever the node reaches said limit and set them as leaf nodes
	if ((rightIndex - leftIndex) <= 100) //  || (other termination criteria))
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
		// Since checking every single possible combination takes too long, cut it to 15 different cuts for faster BVH creation
		for (int i = leftIndex + 1; i < rightIndex; i = i + ceil((float)(rightIndex - leftIndex) / 16))
		{
			// leftIndex = 2, rightIndex = 7
			// i = 3
			// 0 | 1 2 3 4
			// 2 3 4 5 | 6
			// Get bounding box min/max, create temporary boxes for temp left/right children, calculate SAH, store min costs
			BBMinMax(leftMin, leftMax, leftIndex, i );
			BBMinMax(rightMin, rightMax, i , rightIndex);
			numItemsLeft = i - leftIndex ;
			numItemsRight = rightIndex- i;
			Box tempBoxLeft = Box(leftMin, leftMax, false);
			Box tempBoxRight = Box(rightMin, rightMax, false);
			tempCostX = SAHCalculate(currentBox, &tempBoxLeft, &tempBoxRight, numItemsLeft, numItemsRight);
			//minCostX = std::min(minCostX, tempCostX);
			if (minCostX > tempCostX && i != rightIndex-1)
			{
				minCostX = tempCostX;
				splitX = i ;
			}
			//std::cout << "tempCostX: " << tempCostX << " minCostX: " << minCostX << " numItemsLeft: " << numItemsLeft << " numItemsRight: " << numItemsRight << std::endl;
		}

		std::sort(boxes->begin() + leftIndex, boxes->begin() + (rightIndex - 1), yCompare);
		for (int i = leftIndex + 1; i < rightIndex; i = i + ceil((float)(rightIndex - leftIndex) / 16))
		{
			BBMinMax(leftMin, leftMax, leftIndex, i );
			BBMinMax(rightMin, rightMax, i, rightIndex);
			numItemsLeft = i - leftIndex;
			numItemsRight = rightIndex - i;
			Box tempBoxLeft = Box(leftMin, leftMax, false);
			Box tempBoxRight = Box(rightMin, rightMax, false);
			tempCostY = SAHCalculate(currentBox, &tempBoxLeft, &tempBoxRight, numItemsLeft, numItemsRight);
			//minCostX = std::min(minCostX, tempCostX);
			if (minCostY > tempCostY && i != rightIndex - 1)
			{
				minCostY = tempCostY;
				splitY = i;
			}
			//std::cout << "tempCostX: " << tempCostY << " minCostX: " << minCostY << " numItemsLeft: " << numItemsLeft << " numItemsRight: " << numItemsRight << std::endl;
		}

		std::sort(boxes->begin() + leftIndex, boxes->begin() + (rightIndex - 1), zCompare);
		for (int i = leftIndex + 1; i < rightIndex ; i = i+ ceil((float)(rightIndex - leftIndex) / 16))
		{
			BBMinMax(leftMin, leftMax, leftIndex, i);
			BBMinMax(rightMin, rightMax, i, rightIndex);
			numItemsLeft = i - leftIndex;
			numItemsRight = rightIndex - i;
			Box tempBoxLeft = Box(leftMin, leftMax, false);
			Box tempBoxRight = Box(rightMin, rightMax, false);
			tempCostZ = SAHCalculate(currentBox, &tempBoxLeft, &tempBoxRight, numItemsLeft, numItemsRight);
			//minCostZ = std::min(minCostZ, tempCostZ);
			if (minCostZ > tempCostZ && i != rightIndex - 1)
			{
				minCostZ = tempCostZ;
				splitZ = i;
			}
		}

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
		}
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
	//std::cout << "CALL!\n";
	
	bool hit = false;
    HitInfo tempMinHit;
    minHit.t = MIRO_TMAX;

	std::stack<BVHNode*> stack;
	Box* currentBox = root->getBox();
	Box* primaryBox;
	BVHNode* currentNode = root;
	BVHNode * leftChildNode;
	BVHNode * rightChildNode;
	
	float tmin = 0.0f, tmax = 0.0f, leftTMin = 0.0f, leftTMax = 0.0f, rightTMin = 0.0f, rightTMax = 0.0f;
	bool leftHit, rightHit;
	//First check if the worldbox was intersected
		boxInts++;
		while (true)
		{
			//Get children nodes if current node is not a leaf node
			if (!currentNode->isLeaf())
			{
				leftChildNode = nodes->at(currentNode->getIndex());
				// right child is always +1 to the index of the left child
				rightChildNode = nodes->at(currentNode->getIndex() + 1);
				/*
				leftHit = leftChildNode->getBox()->intersectHelper(minHit, ray, leftTMin, leftTMax);
				rightHit = rightChildNode->getBox()->intersectHelper(minHit, ray, rightTMin, rightTMax);
				*/
				leftHit = leftChildNode->getBox()->intersect(minHit, ray, tMin, tMax);
				rightHit = rightChildNode->getBox()->intersect(minHit, ray, tMin, tMax);

				// Both children were hit
				if (leftHit && rightHit)
				{
					/*
					if (leftTMax < rightTMin)
					{
						currentNode = leftChildNode;
						boxInts++;
						continue;
					}
					else if (rightTMax < leftTMin)
					{
						currentNode = rightChildNode;
						boxInts++;
						continue;
					}

					if (leftTMin < rightTMin)
					{
						currentNode = leftChildNode;
						stack.push(rightChildNode);
					}
					else
					{
						currentNode = rightChildNode;
						stack.push(leftChildNode);
					}
					*/
					currentNode = leftChildNode;
					stack.push(rightChildNode);
					boxInts++;
					boxInts++;
					continue;
				}
				//One child was hit
				else if (leftHit)
				{
					currentNode = leftChildNode;
					boxInts++;
					continue;
				}
				else if (rightHit)
				{
					currentNode = rightChildNode;
					boxInts++;
					continue;
				}
				//Neither were hit
				else
				{
					// Do nothing
				}
			}
			else
			{
				for (int i = currentNode->getIndex(); i < currentNode->getIndex() + currentNode->getNObjs(); i++)
				{
					//std::cout << "boxes from index " << currentNode->getIndex() << " to " << currentNode->getIndex() + currentNode->getNObjs() << std::endl;
					primaryBox = boxes->at(i);
					//if (primaryBox->intersect(minHit, ray, tMin, tMax))
					//{
					if (primaryBox->getPrimary()->intersect(tempMinHit, ray, tMin, tMax))
					{
						triInts++;
						hit = true;
						if (tempMinHit.t < minHit.t)
						{
							minHit = tempMinHit;
						}
					}
					//}
				}
			}
			//Pop stack and see if other nodes need to be checked
			if (stack.empty())
			{
				//std::cout << boxInts << std::endl;
				return hit;
			}

			currentNode = stack.top();
			stack.pop();
		}

	/*
    for (size_t i = 0; i < m_objects->size(); ++i)
    {
		if (worldBox->intersect(minHit, ray, tMin, tMax))
		{

			if ((*m_objects)[i]->intersect(tempMinHit, ray, tMin, tMax))
			{
				hit = true;
				if (tempMinHit.t < minHit.t)
				minHit = tempMinHit;
				//std::cout << "HELLO!\n";
			}
		}
    }
    */
	//std::cout << boxInts;
    return hit;
}

bool BVH::intersectHelper(HitInfo& minHit, const Ray& ray, float tMin, float tMax)
{
	std::stack<BVHNode*> stack;
	Box* currentBox = root->getBox();
	Box* primaryBox;
	BVHNode* currentNode = root;
	BVHNode * leftChildNode;
	BVHNode * rightChildNode;
	bool hit = false;
	HitInfo tempMinHit;
	bool leftHit, rightHit;

	//First check if the worldbox was intersected
	if (currentBox->intersect(minHit, ray, tMin, tMax))
	{
		while (true)
		{
			//Get children nodes if current node is not a leaf node
			if (!currentNode->isLeaf())
			{
				leftChildNode = nodes->at(currentNode->getIndex());
				// right child is always +1 to the index of the left child
				rightChildNode = nodes->at(currentNode->getIndex() + 1);
				leftHit = leftChildNode->getBox()->intersect(minHit, ray, tMin, tMax);
				rightHit = rightChildNode->getBox()->intersect(minHit, ray, tMin, tMax);


				// Both children were hit
				if (leftHit && rightHit)
				{
					currentNode = leftChildNode;
					//StackItem tempstack;
					//tempstack.ptr = rightChildNode;
					stack.push(rightChildNode);
				}
				//One child was hit
				else if (leftHit)
				{
					currentNode = leftChildNode;
				}
				else if (rightHit)
				{
					currentNode = rightChildNode;
				}
				//Neither were hit
				else
				{
					// Do nothing
				}
			}
			else
			{
				for (int i = currentNode->getIndex(); i < currentNode->getIndex() + currentNode->getNObjs(); i++)
				{
					primaryBox = boxes->at(i);
					//if (primaryBox->intersect(minHit, ray, tMin, tMax))
					//{
					if (primaryBox->getPrimary()->intersect(minHit, ray, tMin, tMax))
					{
						hit = true;
						if (tempMinHit.t < minHit.t)
						{
							minHit = tempMinHit;
						}
					}
					//}
				}
			}

			//Pop stack and see if other nodes need to be checked
			if (stack.empty())
			{
				return hit;
			}

			currentNode = stack.top();
		}
	}
	else
	{
		return false;
	}

}

void BVH::drawBox(Vector3 min, Vector3 max) {
	glColor3f(1, 1, 1);
	glPushMatrix();
	glTranslatef((max.x - min.x) /2, (max.y - min.y) / 2, (max.z - min.z) / 2);
	glutWireCube(max.x);
	glPopMatrix();
}