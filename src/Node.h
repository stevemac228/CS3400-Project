#pragma once
#include "Vec2.h"
#include <vector>

class Node
{
public:

	Vec2 state = Vec2(0, 0);
	Vec2 action = Vec2(0, 0);
	std::vector<Vec2> parent = std::vector<Vec2>();
	bool startNode = false;
	int g = 0;
	int h = 0;

	Node();
	Node(bool startNode);
	Node(Vec2 state, Vec2 action, int g, float h);
	
	friend bool operator < (const Node& lhs, const Node& rhs);
	friend bool operator > (const Node& lhs, const Node& rhs);
};

