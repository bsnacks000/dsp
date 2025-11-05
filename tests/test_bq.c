#include "test_bq.h"
#include <dsp/bq.h>

MunitResult test_dfII_init(const MunitParameter params[], void* data);
MunitResult test_dfII_tick(const MunitParameter params[], void* data);

// bq nonresonant - first order filters / butterworth / linkwitz-riley
MunitResult test_bq_non_resonant_init(const MunitParameter params[], void* data);

MunitResult test_bq_non_resonant_tick_block_lpf(const MunitParameter params[],
                                                void* data);

MunitResult test_bq_non_resonant_tick_block_hpf(const MunitParameter params[],
                                                void* data);

MunitResult test_bq_non_resonant_tick_block_apf(const MunitParameter params[],
                                                void* data);

MunitResult test_bq_non_resonant_tick_block_butterworth(const MunitParameter params[],
                                                        void* data);

MunitResult test_bq_non_resonant_tick_block_linkwitzriley(const MunitParameter params[],
                                                          void* data);

// bq resonant - 2nd order filters + smith-angell reson
MunitResult test_bq_resonant_init(const MunitParameter params[], void* data);

MunitResult test_bq_resonant_tick_block_lpf(const MunitParameter params[], void* data);

MunitResult test_bq_resonant_tick_block_hpf(const MunitParameter params[], void* data);

MunitResult test_bq_resonant_tick_block_bpf(const MunitParameter params[], void* data);

MunitResult test_bq_resonant_tick_block_bsf(const MunitParameter params[], void* data);

MunitResult test_bq_resonant_tick_block_reson(const MunitParameter params[],
                                              void* data);

MunitResult test_bq_resonant_tick_block_apf(const MunitParameter params[], void* data);

// parametric filters
MunitResult test_bq_para_eq_init(const MunitParameter params[], void* data);

MunitResult test_bq_para_eq_tick_block_lshlf(const MunitParameter params[], void* data);

MunitResult test_bq_para_eq_tick_block_hshlf(const MunitParameter params[], void* data);

MunitResult test_bq_para_eq_tick_block_peak(const MunitParameter params[], void* data);
