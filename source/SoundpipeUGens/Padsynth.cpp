/*
    Example implementation of the PADsynth basic algorithm
    By: Nasca O. Paul, Tg. Mures, Romania

    Ported to pure C by Paul Batchelor

    Ported to SuperCollider by Xavier Riley

    This implementation and the algorithm are released under Public Domain
    Feel free to use it into your projects or your products ;-)

    This implementation is tested under GCC/Linux, but it's
    very easy to port to other compiler/OS.
*/

#include <stdlib.h>
#include <math.h>

#include "sndfile.h"
#include "sndfile.hh"

extern "C" {
#include "soundpipe.h"
}

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

#include "SC_PlugIn.h"

// InterfaceTable contains pointers to functions in the host (server).
static InterfaceTable *ft;

typedef struct user_data {
		sp_ftbl *ft, *amps;
		sp_osc *osc;
		SPFLOAT fc;
} UserData;

// declare struct to hold unit generator state
struct Padsynth : public Unit
{
  float freq;
  float bw;
  int   num_harmonics;
	UserData ud;
	sp_data *sp;
};

// declare unit generator functions
static void Padsynth_next_a(Padsynth *unit, int inNumSamples);
static void Padsynth_next_k(Padsynth *unit, int inNumSamples);
static void Padsynth_Ctor(Padsynth* unit);
static void Padsynth_Dtor(Padsynth* unit);


//////////////////////////////////////////////////////////////////

// Ctor is called to initialize the unit generator.
// It only executes once.

// A Ctor usually does 3 things.
// 1. set the calculation function.
// 2. initialize the unit generator state variables.
// 3. calculate one sample of output.
void Padsynth_Ctor(Padsynth* unit)
{
    int i;

    // if the frequency argument is audio rate
    SETCALC(Padsynth_next_a);

		// 2. initialize the unit generator state variables.
    unit->freq          = IN0(0);
    unit->bw            = IN0(1);
    unit->num_harmonics = IN0(2);

    sp_create(&unit->sp);
    sp_ftbl_create(unit->sp, &unit->ud.amps, unit->num_harmonics);

    sp_ftbl_create(unit->sp, &unit->ud.ft, 262144);
    // just creates a buffer of length ...
    sp_osc_create(&unit->ud.osc);

    unit->sp->sr = SAMPLERATE;
    //sp->len = ud.ft->size;
    // This would generate a 5 second sample
    //sp->len = sp->sr * 5;
    // we want an infinite generator so set len to zero
    unit->sp->len = 0;

    unit->ud.amps->tbl[0] = 0.0;

    for(i = 1; i < unit->ud.amps->size; i++){
        unit->ud.amps->tbl[i] = 1.0 / i;
        if((i % 2) == 0) unit->ud.amps->tbl[i] *= 2.0;
    }

    /* Discovered empirically. multiply frequency by this constant. */
    unit->ud.fc = 1 / (6.0 * unit->freq);

    sp_gen_padsynth(unit->sp, unit->ud.ft, unit->ud.amps, unit->freq, unit->bw);

    sp_osc_init(unit->sp, unit->ud.osc, unit->ud.ft, 0.0);
    unit->ud.osc->freq = unit->freq * unit->ud.fc;

    // 3. calculate one sample of output.
    Padsynth_next_a(unit, 1);
}


//////////////////////////////////////////////////////////////////

// The calculation function executes once per control period
// which is typically 64 samples.

// calculation function for an audio rate frequency argument
void Padsynth_next_a(Padsynth *unit, int inNumSamples)
{
    // get the pointer to the output buffer
    float *out = OUT(0);

    // get the pointer to the input buffer
    unit->ud.osc->freq = unit->freq * unit->ud.fc;

    // perform a loop for the number of samples in the control period.
    // If this unit is audio rate then inNumSamples will be 64 or whatever
    // the block size is. If this unit is control rate then inNumSamples will
    // be 1.
    for (int i=0; i < inNumSamples; ++i)
    {
        // write the output
        out[i] = unit->ud.ft->tbl[unit->sp->pos % unit->ud.ft->size];
        unit->sp->pos++;
    }
}

void Padsynth_Dtor(Padsynth* unit)
{
    sp_osc_destroy(&unit->ud.osc);
    sp_ftbl_destroy(&unit->ud.amps);
    sp_ftbl_destroy(&unit->ud.ft);
}


// the entry point is called by the host when the plug-in is loaded
PluginLoad(Padsynth)
{
    // InterfaceTable *inTable implicitly given as argument to the load function
    ft = inTable; // store pointer to InterfaceTable

    DefineSimpleUnit(Padsynth);
}
