#ifndef DCL_DUALLNKLIST_H
#define DCL_DUALLNKLIST_H

class CDCL_DUALLNK_LIST;

class CDCL_DUALLNK_NODE
{
	friend class CDCL_DUALLNK_LIST;
public:
	CDCL_DUALLNK_NODE();
	CDCL_DUALLNK_NODE* GetNextNode();
    CDCL_DUALLNK_NODE* GetPrevNode();
private:	
	CDCL_DUALLNK_NODE*	lpNextNode;
	CDCL_DUALLNK_NODE*	lpPrevNode;
};

class CDCL_DUALLNK_LIST
{
public:
	unsigned int GetNumNodes();
	int AddNode(CDCL_DUALLNK_NODE* lpNewNode);
	int DeleteNode(CDCL_DUALLNK_NODE* lpNode);
	CDCL_DUALLNK_NODE* GetFirstNode();
	CDCL_DUALLNK_NODE* GetLastNode();
	void Free();

	CDCL_DUALLNK_LIST();
	~CDCL_DUALLNK_LIST();
private:
	unsigned int		nNumNodes;
	CDCL_DUALLNK_NODE*	lpFirstNode;
	CDCL_DUALLNK_NODE*	lpLastNode;
};

#endif