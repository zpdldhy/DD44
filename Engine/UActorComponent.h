#pragma once

class UActorComponent
{
	virtual void Init();
	virtual void Tick();
	virtual void PreRender();
	virtual void Render();
	virtual void PostRender();
	virtual void Destroy();
};

