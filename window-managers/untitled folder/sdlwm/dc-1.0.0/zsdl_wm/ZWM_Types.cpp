#include <stdio.h>
#include <assert.h>
#include "ZWM_Debug.h"
#include "ZWM_Types.h"
#include "ZWM_Object.h"


/*#ifdef _DEBUG
	#define INTEGTEST(a,b) IntegrityTest(a,b)
#else
	#define INTEGTEST(a,b)
#endif*/

CZWM_LLIST::CZWM_LLIST():lpFirstNode(0),lpLastNode(0),nNumNodes(0)
{

}

/*void CZWM_LLIST::IntegrityTest(char* szFile,int nLine)
{
	static int nCnt=0;
	//ZWMDBG_Print("IT(%i|%i)\n",nCnt++,nLine);
	CZWM_LL_NODE* lpCur=lpFirstNode;
	for(unsigned int i=0;i<nNumNodes;i++)
	{
		if(lpCur==0)
		{
			ZWMDBG_Print("##IntTst %i (%s %i)\n",nCnt++,szFile,nLine);
			ZWMDBG_Print("##LL INTEGRITY UNSTABLE - Nodes: %i CurNode: %i\n",nNumNodes,i++);
			assert(false);
		}
		lpCur=lpCur->lpNextNode;
	}
}

void CZWM_LLIST::RecIntegrityTest(char* szFile,int nLine)
{
	static int nCnt=0;
	//ZWMDBG_Print("IT(%i|%i)\n",nCnt++,nLine);
	CZWM_WIDGET* lpCur=(CZWM_WIDGET*)lpFirstNode;
	for(unsigned int i=0;i<nNumNodes;i++)
	{
		if(lpCur==0)
		{
			ZWMDBG_Print("##IntTst %i (%s %i)\n",nCnt++,szFile,nLine);
			ZWMDBG_Print("##LL INTEGRITY UNSTABLE - Nodes: %i CurNode: %i\n",nNumNodes,i++);
			assert(false);
		}
		lpCur->llChildList.RecIntegrityTest(szFile,nLine);
		lpCur=(CZWM_WIDGET*)lpCur->lpNextNode;
	}
}*/


CZWM_LL_NODE::CZWM_LL_NODE():lpNextNode(0),lpPrevNode(0)
{
	
}

CZWM_LL_NODE* CZWM_LLIST::GetNode(unsigned int nIdx)
{
	if(nIdx>=nNumNodes) return 0;
	CZWM_LL_NODE* lpCur=lpFirstNode;
	for(unsigned int i=0;i<nIdx;i++)
	{
		lpCur=lpCur->lpNextNode;
	}
	return lpCur;
}

void CZWM_LLIST::AddNode(CZWM_LL_NODE* lpNode)
{
	if(lpFirstNode) 
	{
		lpLastNode->lpNextNode=lpNode;
		lpNode->lpPrevNode=lpLastNode;
		lpLastNode=lpNode;
	}
	else
	{
		lpFirstNode=lpNode;
		lpLastNode=lpNode;
	}
	nNumNodes++;
}

void CZWM_LLIST::DeleteNode(CZWM_LL_NODE* lpNode)
{
	if(lpNode==lpLastNode) lpLastNode=lpNode->lpPrevNode;
	if(lpNode==lpFirstNode) lpFirstNode=lpNode->lpNextNode;

	if(lpNode->lpNextNode) lpNode->lpNextNode->lpPrevNode=lpNode->lpPrevNode;
	if(lpNode->lpPrevNode) lpNode->lpPrevNode->lpNextNode=lpNode->lpNextNode;

	nNumNodes--;

	delete lpNode;
}

void CZWM_LLIST::MoveAfter(CZWM_LL_NODE* lpRef,CZWM_LL_NODE* lpNode)
{
	if(lpRef==lpLastNode)
	{
		MoveToBack(lpNode);
	}
	else if(lpRef==0)
	{
		MoveToFront(lpNode);
	}
	else
	{
		if(lpRef==lpNode) return;
		if(lpNode==lpFirstNode) lpFirstNode=lpNode->lpNextNode;

		if(lpNode->lpPrevNode) lpNode->lpPrevNode->lpNextNode=lpNode->lpNextNode;
		if(lpNode->lpNextNode) lpNode->lpNextNode->lpPrevNode=lpNode->lpPrevNode;
		lpNode->lpPrevNode=lpRef;
		lpNode->lpNextNode=lpRef->lpNextNode;
		if(lpRef->lpNextNode) lpRef->lpNextNode->lpPrevNode=lpNode;
		lpRef->lpNextNode=lpNode;
	}
}

void CZWM_LLIST::AddAfter(CZWM_LL_NODE* lpRef,CZWM_LL_NODE* lpNode)
{
	if(lpFirstNode==0)
	{
		lpFirstNode=lpNode;
		lpLastNode=lpNode;
	}
	else if(lpRef)
	{
		if(lpRef==lpNode) return;

		lpNode->lpPrevNode=lpRef;
		lpNode->lpNextNode=lpRef->lpNextNode;
		if(lpRef->lpNextNode) lpRef->lpNextNode->lpPrevNode=lpNode;
		lpRef->lpNextNode=lpNode;

		if(lpRef==lpLastNode) lpLastNode=lpNode;
	}
	else
	{
		lpNode->lpNextNode=lpFirstNode;
		lpFirstNode->lpPrevNode=lpNode;
		lpFirstNode=lpNode;
	}
	nNumNodes++;
}

void CZWM_LLIST::MoveToFront(CZWM_LL_NODE* lpNode)
{
	if(lpNode==0||lpNode==lpFirstNode) return;
	if(lpNode==lpLastNode)	lpLastNode=lpNode->lpPrevNode;

	if(lpNode->lpNextNode) lpNode->lpNextNode->lpPrevNode=lpNode->lpPrevNode;
	if(lpNode->lpPrevNode) lpNode->lpPrevNode->lpNextNode=lpNode->lpNextNode;

	lpFirstNode->lpPrevNode=lpNode;
	lpNode->lpNextNode=lpFirstNode;
	lpNode->lpPrevNode=0;
	lpFirstNode=lpNode;
}

void CZWM_LLIST::MoveToBack(CZWM_LL_NODE* lpNode)
{
	if(lpNode==0||lpNode==lpLastNode) return;
	if(lpNode==lpFirstNode)	lpFirstNode=lpNode->lpNextNode;

	if(lpNode->lpPrevNode) lpNode->lpPrevNode->lpNextNode=lpNode->lpNextNode;
	if(lpNode->lpNextNode) lpNode->lpNextNode->lpPrevNode=lpNode->lpPrevNode;

	lpLastNode->lpNextNode=lpNode;
	lpNode->lpPrevNode=lpLastNode;
	lpNode->lpNextNode=0;
	lpLastNode=lpNode;
}

void CZWM_LLIST::DeleteList()
{
	CZWM_LL_NODE*	lpCur=lpFirstNode;
	CZWM_LL_NODE*	lpNxt=0;
	for(unsigned int i=0;i<nNumNodes;i++)
	{
		lpNxt=lpCur->lpNextNode;
		delete lpCur;
		lpCur=lpNxt;
	}
	nNumNodes=0;
	lpFirstNode=0;
	lpLastNode=0;
}