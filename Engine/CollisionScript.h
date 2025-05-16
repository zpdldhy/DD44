#pragma once

class CollisionScript
{
public:
	virtual void QueryOnly() abstract;
	virtual void QueryAndPhysics() abstract;
};

