#include "ebi-memory-pool.h"

typedef struct node_s node_s;

struct node_s {
    node_s * psPrev;
    node_s * psNext;
    size_t zAskSize;
};

typedef struct {
    size_t zTotal;
    size_t zUsage;
} info_s;

struct pool_s {
    node_s sRootNode;
    info_s sPoolInfo;
};

pool_s * 
poolFormat(
    void * const cpvPool,
    const size_t czBoundarySize
) {
    pool_s * psThis = NULL;

    if ( 
        NULL == cpvPool ||
        czBoundarySize < sizeof(pool_s)
    ) {
        return NULL;
    }

    psThis = (pool_s *)cpvPool;
    psThis->sRootNode.psPrev = NULL;
    psThis->sRootNode.psNext = NULL;
    psThis->sRootNode.zAskSize = sizeof(psThis->sPoolInfo);
    psThis->sPoolInfo.zTotal = czBoundarySize;
    psThis->sPoolInfo.zUsage = sizeof(node_s) + psThis->sRootNode.zAskSize;

    return psThis;
}

void *
poolMalloc(
    pool_s * const cpsThis,
    const size_t czAskSize
) {
    node_s * psRefs = NULL;
    node_s * psCurr = NULL;
    size_t zInterval = 0;

    if ( 
        NULL == cpsThis ||
        cpsThis->sPoolInfo.zTotal < cpsThis->sPoolInfo.zUsage + czAskSize
    ) {
        return NULL;
    }

/* looking for an avalible space, in which ( interval >= asked size ) */
    for ( psCurr = &cpsThis->sRootNode; NULL != psCurr->psNext; psCurr = psCurr->psNext )
    {
        zInterval &= 0;
        zInterval += (size_t)( (char *)( psCurr->psNext ) - (char *)( psCurr ) );
        zInterval -= (size_t)( sizeof(node_s) + psCurr->zAskSize ) ;
        if ( zInterval >= sizeof(node_s) + czAskSize )
        {
            goto __found;
        }
    }

/* check if the lastest space is enough */
    if ( NULL == psCurr->psNext )
    {
        zInterval &= 0;
        zInterval += cpsThis->sPoolInfo.zTotal;
        zInterval -= (size_t)( (char *)( psCurr ) - (char *)( cpsThis ) );
        zInterval -= (size_t)( sizeof(node_s) + psCurr->zAskSize ) ;
        if ( zInterval >= sizeof(node_s) + czAskSize )
        {
            goto __found;
        }
    }

    return NULL;

__found:
    psRefs = ( (char *)( psCurr + 1 ) + psCurr->zAskSize );
    psRefs->psPrev = psCurr;
    psRefs->psNext = psCurr->psNext;
    psRefs->psPrev->psNext = psRefs;
    psRefs->psNext->psPrev = psRefs;
    psRefs->zAskSize = czAskSize;
    cpsThis->sPoolInfo.zUsage += sizeof(node_s) + czAskSize;

    return (void *)( psRefs + 1 );
}

void
poolReturn(
    void * pvRefs
) {
    node_s * psRefs = NULL;
    node_s * psCurr = NULL;
    pool_s * psThis = NULL;

    if ( NULL == pvRefs )
    {
        return;
    }

    psRefs = (char *)( pvRefs ) - sizeof(node_s);

/* remove this node */
    if ( NULL != psRefs->psPrev )
    {
        psRefs->psPrev->psNext = psRefs->psNext;
    }

    if ( NULL != psRefs->psNext )
    {
        psRefs->psNext->psPrev = psRefs->psPrev;
    }

/* looking for the root node */
    for ( psCurr = psRefs; NULL != psCurr->psPrev; psCurr = psCurr->psPrev )
    {

    }

    if ( NULL == psCurr->psPrev )
    {
        psThis = (pool_s *)( psCurr );
        psThis->sPoolInfo.zUsage -= ( sizeof(node_s) + psRefs->zAskSize );
    }
}

size_t
poolTotal(
    const pool_s * const cpcsThis
) {
    return ( NULL != cpcsThis ) ? ( cpcsThis->sPoolInfo.zTotal ) : ( 0 ) ;
}

size_t
poolUsage(
    const pool_s * const cpcsThis
) {
    return ( NULL != cpcsThis ) ? ( cpcsThis->sPoolInfo.zUsage ) : ( 0 ) ;
}
