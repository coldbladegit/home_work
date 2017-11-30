#ifndef _CB_VECTOR_H
#define _CB_VECTOR_H

typedef void (__cdecl *FREE_ELEM_FUNC) (void *);
typedef bool (__cdecl *IS_EQUAL) (void *, int);

typedef struct _CB_VECTOR {
    int size;
    int capacity;
    void **ppDatas;
    int (*add_element) (_CB_VECTOR *, void *);
    int (*add_element_to) (_CB_VECTOR *, void *, int);
    int (*remove_element) (_CB_VECTOR *, void **);
    int (*remove_element_from) (_CB_VECTOR *, int, void **);
    void (*remove_all) (_CB_VECTOR *, FREE_ELEM_FUNC);
    void *(*search_element) (_CB_VECTOR *, int, IS_EQUAL);
} CB_VECTOR;

int new_cb_vector(CB_VECTOR **pp);

void delete_cb_vector(CB_VECTOR *p);

#endif//_CB_VECTOR_H