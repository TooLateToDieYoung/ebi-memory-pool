#ifndef __EBI_MEMORY_POOL_H
#define __EBI_MEMORY_POOL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stddef.h>

typedef struct pool_s pool_s;

pool_s * 
poolFormat(
    void * const cpvPool,
    const size_t czBoundarySize
);

void *
poolMalloc(
    pool_s * const cpsThis,
    const size_t czAskSize
);

void
poolReturn(
    void * pvRefs
);

size_t
poolTotal(
    const pool_s * const cpcsThis
);

size_t
poolUsage(
    const pool_s * const cpcsThis
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EBI_MEMORY_POOL_H */
