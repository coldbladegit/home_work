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
* @brief    创建树的根节点
* @param    pData //数据部分
* @param    isUseRootNode //是否使用根节点
* @return  成功返回0,否则返回内存分配失败的错误码
**/
int cb_new_tree(CB_TREE **ppTree, void *pData, bool isUseRootNode);

/**
* @brief    销毁树以及对应数据
* @return  成功返回0,否则返回内存分配失败的错误码
**/
int cb_delete_tree(CB_TREE *pTree);

#endif//_CB_TREE_H