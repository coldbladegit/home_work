#ifndef _CFG_MGR_H
#define _CFG_MGR_H

typedef struct _CONFIG_MGR {
    int (*read_config) (void *);
    int (*write_config) (void *);
} CONFIG_MGR;

int new_config_mgr(CONFIG_MGR **ppMgr);

void delete_config_mgr(CONFIG_MGR *pMgr);

#endif//_CFG_MGR_H