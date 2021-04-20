#include "Physics.h"
#include "Components.h"
//Lisa for everything
Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    float dx 			= abs(int(a->getComponent<CTransform>().pos.x - b->getComponent<CTransform>().pos.x));
	float dy 			= abs(int(a->getComponent<CTransform>().pos.y - b->getComponent<CTransform>().pos.y));

    return Vec2(a->getComponent<CBoundingBox>().halfSize.x + b->getComponent<CBoundingBox>().halfSize.x - dx, a->getComponent<CBoundingBox>().halfSize.y + b->getComponent<CBoundingBox>().halfSize.y - dy);
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    float dx 			= abs(int(a->getComponent<CTransform>().prevPos.x - b->getComponent<CTransform>().prevPos.x));
	float dy 			= abs(int(a->getComponent<CTransform>().prevPos.y - b->getComponent<CTransform>().prevPos.y));

    return Vec2(a->getComponent<CBoundingBox>().halfSize.x + b->getComponent<CBoundingBox>().halfSize.x - dx, a->getComponent<CBoundingBox>().halfSize.y + b->getComponent<CBoundingBox>().halfSize.y - dy);
}

//I'm not sure if this function works correctly, you have some ideas how to make it better please change it
// should work now - Chris
bool Physics::IsInside(const Vec2& pos, std::shared_ptr<Entity> e)
{
	if(e->hasComponent<CAnimation>()) 
	{
		Vec2 halfSize = Vec2 (e->getComponent<CTransform>().pos.x/2,e->getComponent<CTransform>().pos.y/2);
		Vec2 delta= Vec2(e->getComponent<CTransform>().pos - pos).abs();
		if(delta.y-32 < (halfSize.y/2-10) && delta.x-32  < (halfSize.x/2)){
			return true;
		}
	}
    return false;
}

Intersect Physics::LineIntersect(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d)
{
	bool intersect = false;
	Vec2 intersection = Vec2(0, 0);

	float t = ((c - a) * (d - c)) / ((b - a) * (d - c));
	float u = ((c - a) * (b - a)) / ((b - a) * (d - c));

	if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
	{
        intersection = a + (b - a) * t;
		intersect = true;
	}

    return { intersect, intersection };
}

bool Physics::EntityIntersect(const Vec2& a, const Vec2& b, std::shared_ptr<Entity> e)
{
	Vec2 topC = Vec2(e->getComponent<CTransform>().pos.x - e->getComponent<CBoundingBox>().halfSize.x, e->getComponent<CTransform>().pos.y - e->getComponent<CBoundingBox>().halfSize.y);
	Vec2 topD = Vec2(e->getComponent<CTransform>().pos.x + e->getComponent<CBoundingBox>().halfSize.x, e->getComponent<CTransform>().pos.y - e->getComponent<CBoundingBox>().halfSize.y);

	Vec2 bottomC = Vec2(e->getComponent<CTransform>().pos.x - e->getComponent<CBoundingBox>().halfSize.x, e->getComponent<CTransform>().pos.y + e->getComponent<CBoundingBox>().halfSize.y);
	Vec2 bottomD = Vec2(e->getComponent<CTransform>().pos.x + e->getComponent<CBoundingBox>().halfSize.x, e->getComponent<CTransform>().pos.y + e->getComponent<CBoundingBox>().halfSize.y);

	Vec2 leftC = Vec2(e->getComponent<CTransform>().pos.x - e->getComponent<CBoundingBox>().halfSize.x, e->getComponent<CTransform>().pos.y - e->getComponent<CBoundingBox>().halfSize.y);
	Vec2 leftD = Vec2(e->getComponent<CTransform>().pos.x - e->getComponent<CBoundingBox>().halfSize.x, e->getComponent<CTransform>().pos.y + e->getComponent<CBoundingBox>().halfSize.y);

	Vec2 rightC = Vec2(e->getComponent<CTransform>().pos.x + e->getComponent<CBoundingBox>().halfSize.x, e->getComponent<CTransform>().pos.y - e->getComponent<CBoundingBox>().halfSize.y);
	Vec2 rightD = Vec2(e->getComponent<CTransform>().pos.x + e->getComponent<CBoundingBox>().halfSize.x, e->getComponent<CTransform>().pos.y + e->getComponent<CBoundingBox>().halfSize.y);

	if ( LineIntersect(a, b, topC, topD).result || LineIntersect(a, b, bottomC, bottomD).result || LineIntersect(a, b, leftC, leftD).result || LineIntersect(a, b, rightC, rightD).result)
	{
		return true;
	}

    return false;
}