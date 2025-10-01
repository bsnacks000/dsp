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

#include "blep.h"
#include "bq.h"
#include "curve.h"
#include "ddelay.h"
#include "distort.h"
#include "env.h"
#include "follow.h"
#include "line.h"
#include "maths.h"
#include "oscil.h"
#include "phasor.h"
#include "rc.h"
#include "sampler.h"
#include "svf.h"

// #include <stdio.h>

//  called once when the lib loads
PUBLIC int csoundModuleCreate(CSOUND* csound) {
    // printf("ModuleCreate called!\n");
    // fflush(stdout);
    int err;
    if ((err = blsaw_deck_init(csound)) != OK) {
        return csound->InitError(csound, "blsaw_deck_init: %d", err);
    }

    if ((err = smorph_deck_init(csound)) != OK) {
        return csound->InitError(csound, "smorph_deck_init: %d", err);
    }

    if ((err = chebsaw_tab_init(csound)) != OK) {
        return csound->InitError(csound, "cheby3_tab_init: %d", err);
    }

    return OK;
}

// called once before the lib is destroyed.
PUBLIC int csoundModuleDestroy(CSOUND* csound) {
    // printf("ModuleDestroy called!\n");
    blsaw_deck_destroy(csound);
    smorph_deck_destroy(csound);

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
    {"blsaw", sizeof(blsaw), 0, "a", "ai", (SUBR) blsaw_init, (SUBR) blsaw_vector, NULL,
     NULL},
    {"smorph", sizeof(smorph), 0, "a", "aai", (SUBR) smorph_init, (SUBR) smorph_vector,
     NULL, NULL},
    {"sampler", sizeof(sampler), 0, "a", "aii", (SUBR) sampler_init,
     (SUBR) sampler_vector, NULL, NULL},
    {"lline", sizeof(lline), 0, "a", "iii", (SUBR) lline_init, (SUBR) lline_vector,
     NULL, NULL},
    {"ccurve", sizeof(ccurve), 0, "a", "iiii", (SUBR) ccurve_init, (SUBR) ccurve_vector,
     NULL, NULL},
    {"chebsaw", sizeof(chebsaw), 0, "a", "a", (SUBR) chebsaw_init,
     (SUBR) chebsaw_vector, NULL, NULL},
    {"saturator", sizeof(saturator), 0, "a", "aai", (SUBR) saturator_init,
     (SUBR) saturator_vector, NULL, NULL},
    {"blepsaw", sizeof(ssaw), 0, "a", "ai", (SUBR) ssaw_init, (SUBR) ssaw_vector, NULL,
     NULL},
    {"blepsqr", sizeof(ssqr), 0, "a", "aai", (SUBR) ssqr_init, (SUBR) ssqr_vector, NULL,
     NULL},
    {"ar_line", sizeof(ar_line), 0, "a", "aaaaaaa", (SUBR) ar_line_init,
     (SUBR) ar_line_vector, NULL, NULL},
    {"adsr_line", sizeof(adsr_line), 0, "a", "aaaaaaaaa", (SUBR) adsr_line_init,
     (SUBR) adsr_line_vector, NULL, NULL},
    {NULL, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL},
};

// LINKAGE
PUBLIC int32_t csoundModuleInit(CSOUND* csound) {
    int32_t status = 0;
    for (OENTRY* oentry = &localops[0]; oentry->opname; oentry++) {
        status |= csound->AppendOpcode(csound, oentry->opname, oentry->dsblksiz,
                                       oentry->flags, oentry->outypes, oentry->intypes,
                                       oentry->init, oentry->perf, oentry->deinit);
    }
    return status;
}
