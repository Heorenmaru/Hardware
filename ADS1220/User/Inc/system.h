#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

#include "main.h"
#include "annotation.h"
#include "build_info.h"

#define EPOCH                   2017U
#define CALC_DATE(year,month)   ((year - EPOCH) * 12 + month)


typedef enum {
    eStatusError = false,
    eStatusOk = true,
} Status;
// Полный UID
// 01 23 45 67 89 AB CD EF
typedef struct packed_ordered(8,lsb) {
    uint64_t id       : 24U;
    uint64_t devType  :  8U;
    uint64_t boardRev :  5U;
    uint64_t monDate  : 12U;
    uint64_t reserved : 21U;
} Uid;

#endif //__SYSTEM_H__
