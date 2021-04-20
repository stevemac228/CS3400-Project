#include "Node.h"
#include <vector>

Node::Node() {}

Node::Node(bool startNode)
	: startNode(startNode)
{

}

Node::Node(Vec2 state, Vec2 action, int g, float h)
	: state(state)
	, action(action)
	, g(g)
	, h(h)
{

}

bool operator < (const Node& lhs, const Node& rhs)
{
	return lhs.g + lhs.h < rhs.g + rhs.h;
}
bool operator > (const Node& lhs, const Node& rhs)
{
	return lhs.g + lhs.h > rhs.g + rhs.h;
}