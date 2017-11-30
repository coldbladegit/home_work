#ifndef _CB_TREE_H
#define _CB_TREE_H

typedef struct _CB_TREE_NODE{
    void            *pData;
    _CB_TREE_NODE   *pLChild;
    _CB_TREE_NODE   *pRChild;
}CB_TREE_NODE;

typedef struct _CB_TREE {
    bool         isUseRoot;
    CB_TREE_NODE *pRoot;
    int (*new_node) (CB_TREE_NODE **, void *);
    void (*delete_node) (CB_TREE_NODE *);
    int (*search_node) (_CB_TREE *, CB_TREE_NODE **, void *, bool (__cdecl *is_equal ) (void *, void *));
    int (*remove_node) (_CB_TREE *, CB_TREE_NODE **, void *, bool (__cdecl *is_equal ) (void *, void *));
    bool (*is_leaf_node) (_CB_TREE_NODE *);
} CB_TREE;

/**
* @brief    �������ĸ��ڵ�
* @param    pData //���ݲ���
* @param    isUseRootNode //�Ƿ�ʹ�ø��ڵ�
* @return  �ɹ�����0,���򷵻��ڴ����ʧ�ܵĴ�����
**/
int cb_new_tree(CB_TREE **ppTree, void *pData, bool isUseRootNode);

/**
* @brief    �������Լ���Ӧ����
* @return  �ɹ�����0,���򷵻��ڴ����ʧ�ܵĴ�����
**/
int cb_delete_tree(CB_TREE *pTree);

#endif//_CB_TREE_H