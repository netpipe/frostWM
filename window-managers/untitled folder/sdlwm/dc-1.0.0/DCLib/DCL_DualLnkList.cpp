#include "DCL_DualLnkList.h"

CDCL_DUALLNK_NODE::CDCL_DUALLNK_NODE():lpNextNode(0),lpPrevNode(0) {}
CDCL_DUALLNK_LIST::CDCL_DUALLNK_LIST():lpFirstNode(0),lpLastNode(0),nNumNodes(0) {}

CDCL_DUALLNK_LIST::~CDCL_DUALLNK_LIST()
{
	Free();
}

CDCL_DUALLNK_NODE* CDCL_DUALLNK_LIST::GetFirstNode()
{
	return lpFirstNode;
}

CDCL_DUALLNK_NODE* CDCL_DUALLNK_LIST::GetLastNode()
{
	return lpLastNode;
}

CDCL_DUALLNK_NODE* CDCL_DUALLNK_NODE::GetNextNode()
{
	return lpNextNode;
}

CDCL_DUALLNK_NODE* CDCL_DUALLNK_NODE::GetPrevNode()
{
	return lpPrevNode;
}

void CDCL_DUALLNK_LIST::Free()
{
	CDCL_DUALLNK_NODE*	lpCurNode=lpFirstNode;
	CDCL_DUALLNK_NODE*	lpNxtNode=0;
	for(unsigned int i=0;i<nNumNodes;i++)
	{
		lpNxtNode=lpCurNode->lpNextNode;
		delete lpCurNode;
		lpCurNode=lpNxtNode;
	}
}

int CDCL_DUALLNK_LIST::DeleteNode(CDCL_DUALLNK_NODE* lpNode)
{
	if(lpNode==lpFirstNode) lpFirstNode=lpNode->lpNextNode;
	if(lpNode==lpLastNode) lpLastNode=lpNode->lpPrevNode;
	
	if(lpNode->lpNextNode) lpNode->lpNextNode->lpPrevNode=lpNode->lpPrevNode;
	if(lpNode->lpPrevNode) lpNode->lpPrevNode->lpNextNode=lpNode->lpNextNode;
	
	nNumNodes--;
}

unsigned int CDCL_DUALLNK_LIST::GetNumNodes()
{
	return nNumNodes;
}

int CDCL_DUALLNK_LIST::AddNode(CDCL_DUALLNK_NODE* lpNewNode)
{
	if(nNumNodes==0)
	{
		lpFirstNode=lpNewNode;
		lpLastNode=lpNewNode;
	}
	else
	{
		lpLastNode->lpNextNode=lpNewNode;
		lpNewNode->lpPrevNode=lpLastNode;
		lpLastNode=lpNewNode;
	}
	nNumNodes++;
}