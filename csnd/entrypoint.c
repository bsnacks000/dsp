/**
 * @brief - the OENTRY entrypoint using csnd7 API
 *
 * See for useful decription of fields:
 * https://github.com/csound-plugins/csound-plugins
 *
 * char* opname,
 * uint16 blksz,   -- sizeof struct
 * unit16 flags,    0 for our purposes always
 * char* outtypes -- see outtype description (usually a)
 * char* intypes -- see description (usually aki)
 * int32_t  (*init)(CSOUND *, void *p);
 * int32_t  (*perf)(CSOUND *, void *p);
 * int32_t  (*deinit)(CSOUND *, void *p);  -- can register cleanup function
 * void    *useropinfo; --- user opcode parameters (not sure what this for)
 */

#include <csdl.h>

#include "bq.h"
#include "ddelay.h"
#include "follow.h"
#include "maths.h"
#include "oscil.h"
#include "phasor.h"
#include "rc.h"
#include "svf.h"

// called once when the lib loads
int csoundModuleInit(CSOUND* csound) {
    (void) csound;
    return OK;
}

// called once before the lib is destroyed.
int csoundModuleDestroy(CSOUND* csound) {
    (void) csound;
    return OK;
}

static OENTRY localops[] = {
    {"mult", sizeof(mult), 0, "a", "aa", NULL, (SUBR) mult_vector, NULL, NULL},
    {"ddelay2", sizeof(ddelay2), 0, "a", "aaai", (SUBR) ddelay2_init,
     (SUBR) ddelay2_vector, NULL, NULL},
    {"svflp", sizeof(svflp), 0, "a", "aaaa", (SUBR) svflp_init, (SUBR) svflp_vector,
     NULL, NULL},
    {"svfmm", sizeof(svfmm), 0, "aaaa", "aaaa", (SUBR) svfmm_init, (SUBR) svfmm_vector,
     NULL, NULL},
    {"rclp", sizeof(rclp), 0, "a", "aa", (SUBR) rclp_init, (SUBR) rclp_vector, NULL,
     NULL},
    {"rcmm", sizeof(rcmm), 0, "aaa", "aa", (SUBR) rcmm_init, (SUBR) rcmm_vector, NULL,
     NULL},
    {"bqres", sizeof(bqres), 0, "a", "akki", (SUBR) bqres_init, (SUBR) bqres_vector,
     NULL, NULL},
    {"bqnres", sizeof(bqnres), 0, "a", "aki", (SUBR) bqnres_init, (SUBR) bqnres_vector,
     NULL, NULL},
    {"bqpara", sizeof(bqpara), 0, "a", "akkki", (SUBR) bqpara_init,
     (SUBR) bqpara_vector, NULL, NULL},
    {"ftoscil3", sizeof(ftoscil3), 0, "a", "aii", (SUBR) ftoscil3_init,
     (SUBR) ftoscil3_vector, NULL, NULL},
    {"oftoscil3", sizeof(oftoscil3), 0, "a", "aiio", (SUBR) oftoscil3_init,
     (SUBR) oftoscil3_vector, NULL, NULL},
    {"dphasor", sizeof(dphasor), 0, "a", "a", (SUBR) dphasor_init,
     (SUBR) dphasor_vector, NULL, NULL},
    {"envfol", sizeof(envfol), 0, "a", "aaa", (SUBR) envfol_init, (SUBR) envfol_vector,
     NULL, NULL},
    {"blsaw", sizeof(blsaw), 0, "a", "aii", (SUBR) blsaw_init, (SUBR) blsaw_vector,
     NULL, NULL},
};

LINKAGE
