#ifndef _ZWM_TYPES_H
#define _ZWM_TYPES_H

class CZWM_LL_NODE
{
public:
	CZWM_LL_NODE();

	CZWM_LL_NODE*	lpNextNode;
	CZWM_LL_NODE*	lpPrevNode;
};

class CZWM_LLIST
{
public:
	CZWM_LLIST();

	CZWM_LL_NODE*	lpFirstNode;
	CZWM_LL_NODE*	lpLastNode;
	unsigned int	nNumNodes;

	void AddNode(CZWM_LL_NODE* lpNode);
	void DeleteNode(CZWM_LL_NODE* lpNode);
	void DeleteList();
	void MoveToBack(CZWM_LL_NODE* lpNode);
	void MoveToFront(CZWM_LL_NODE* lpNode);
	void MoveAfter(CZWM_LL_NODE* lpRef,CZWM_LL_NODE* lpNode);
	void AddAfter(CZWM_LL_NODE* lpRef,CZWM_LL_NODE* lpNode);
	CZWM_LL_NODE* GetNode(unsigned int nIdx);
	//void IntegrityTest(char* szFile,int nLine);
	//void RecIntegrityTest(char* szFile,int nLine);
};

#endif