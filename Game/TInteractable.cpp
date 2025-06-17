#include "pch.h"
#include "TInteractable.h"
#include "TInteractScriptComponent.h"

void TInteractable::Init()
{
	// script set
	//interactionScript = 
}

void TInteractable::Interact()
{
	if(!interactionScript) 
	{ 
		return; 
	}
	interactionScript->Interact();
}

