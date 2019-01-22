/* Stellarium Web Engine - Copyright (c) 2018 - Noctua Software Ltd
 *
 * This program is licensed under the terms of the GNU AGPL v3, or
 * alternatively under a commercial licence.
 *
 * The terms of the AGPL v3 license can be found in the main directory of this
 * repository.
 */

#include "bayer.h"

#include "uthash.h"
#include "utils/utils.h"

#include <assert.h>

static const unsigned char DATA[7329];

static const char *GREEK[][3] = {
    {"α", "alf", "Alpha"},
    {"β", "bet", "Beta"},
    {"γ", "gam", "Gamma"},
    {"δ", "del", "Delta"},
    {"ε", "eps", "Epsilon"},
    {"ζ", "zet", "Zeta"},
    {"η", "eta", "Eta"},
    {"θ", "tet", "Theta"},
    {"ι", "iot", "Iota"},
    {"κ", "kap", "Kappa"},
    {"λ", "lam", "Lambda"},
    {"μ", "mu" , "Mu"},
    {"ν", "nu" , "Nu"},
    {"ξ", "xi" , "Xi"},
    {"ο", "omi", "Omicron"},
    {"π", "pi" , "Pi"},
    {"ρ", "rho", "Rho"},
    {"σ", "sig", "Sigma"},
    {"τ", "tau", "Tau"},
    {"υ", "ups", "Upsilon"},
    {"φ", "phi", "Phi"},
    {"χ", "chi", "Chi"},
    {"ψ", "psi", "Psi"},
    {"ω", "ome", "Omega"}
};

static const char *CSTS[] = {
    "Aql", "And", "Scl", "Ara", "Lib", "Cet", "Ari", "Sct", "Pyx", "Boo",
    "Cae", "Cha", "Cnc", "Cap", "Car", "Cas", "Cen", "Cep", "Com", "CVn",
    "Aur", "Col", "Cir", "Crt", "CrA", "CrB", "Crv", "Cru", "Cyg", "Del",
    "Dor", "Dra", "Nor", "Eri", "Sge", "For", "Gem", "Cam", "CMa", "UMa",
    "Gru", "Her", "Hor", "Hya", "Hyi", "Ind", "Lac", "Mon", "Lep", "Leo",
    "Lup", "Lyn", "Lyr", "Ant", "Mic", "Mus", "Oct", "Aps", "Oph", "Ori",
    "Pav", "Peg", "Pic", "Per", "Equ", "CMi", "LMi", "Vul", "UMi", "Phe",
    "Psc", "PsA", "Vol", "Pup", "Ret", "Sgr", "Sco", "Ser", "Sex", "Men",
    "Tau", "Tel", "Tuc", "Tri", "TrA", "Aqr", "Vir", "Vel"};

typedef struct {
    UT_hash_handle  hh;

    int             hip;
    uint8_t         cst;
    uint8_t         bayer;
    uint8_t         bayer_n;
} entry_t;

static entry_t *g_data = NULL;        // Global array.
static entry_t *g_entries = NULL;     // Global hash.

static void bayer_init(void)
{
    struct {int hip; uint8_t cst, bayer, bayer_n; char pad;} *data;
    int size, nb, i;

    memcpy(&size, DATA, 4);
    data = malloc(size);
    z_uncompress(data, size, DATA + 4, ARRAY_SIZE(DATA) - 4);

    nb = size / sizeof(*data);
    g_data = calloc(nb + 1, sizeof(*g_data));
    for (i = 0; i < nb; i++) {
        assert(data[i].hip);
        g_data[i].hip = data[i].hip;
        g_data[i].cst = data[i].cst;
        g_data[i].bayer = data[i].bayer;
        g_data[i].bayer_n = data[i].bayer_n;
        HASH_ADD_INT(g_entries, hip, &g_data[i]);
    }
    free(data);
}

bool bayer_get(int hip, char cons[4], int *bayer, int *n)
{
    entry_t *e;
    if (!g_entries) bayer_init();
    HASH_FIND_INT(g_entries, &hip, e);
    if (!e) {
        *bayer = 0;
        return false;
    }
    if (cons)
        memcpy(cons, CSTS[(e->cst - 1)], 4);
    *bayer = e->bayer;
    if (n) *n = e->bayer_n;
    return true;
}

bool bayer_iter(int i, int *hip, char cons[4], int *bayer, int *n,
                const char *greek[3])
{
    const entry_t *e;
    if (!g_data) bayer_init();
    e = &g_data[i];
    if (!e->hip) return false;
    *hip = e->hip;
    memcpy(cons, CSTS[(e->cst - 1)], 4);
    *bayer = e->bayer;
    *n = e->bayer_n;
    greek[0] = GREEK[e->bayer - 1][0];
    greek[1] = GREEK[e->bayer - 1][1];
    greek[2] = GREEK[e->bayer - 1][2];
    return true;
}

// Binary data of the form:
//
// hip      4 bytes
// cst      1 byte (1-indexed)
// bayer    1 byte (1-indexed)
// bayer_n  1 byte (for double stars)
// padding  1 byte
//
// Generated from The BSC5P database
// http://heasarc.gsfc.nasa.gov/W3Browse/star-catalog/bsc5p.html
// using tools/makebayer.py

static const unsigned char DATA[7329] __attribute__((aligned(4))) = {
    216,47,0,0, // uncompressed size
    120,218,45,218,119,124,85,85,214,198,241,125,79,104,6,145,0,138,130,68,
    138,65,106,16,136,148,208,65,73,16,8,45,84,69,5,236,34,162,34,150,177,
    97,193,54,130,98,239,5,43,58,96,1,81,20,21,44,216,113,236,206,188,58,163,
    40,58,118,177,143,51,50,239,119,29,248,131,207,143,115,238,57,123,61,207,
    218,109,237,155,123,194,153,133,84,40,164,244,211,89,152,165,180,245,116,
    44,74,169,209,124,172,147,210,128,99,177,110,74,21,199,97,189,148,118,
    140,231,234,167,180,228,124,108,144,210,219,167,226,14,41,125,18,44,78,
    105,78,60,215,48,165,205,167,224,142,41,205,140,118,26,165,116,69,188,
    183,83,74,127,142,120,141,83,90,127,6,150,164,116,203,66,108,146,210,245,
    167,97,211,148,234,159,139,205,82,122,45,116,236,156,82,159,179,113,151,
    148,158,252,19,54,79,105,94,220,223,53,165,19,79,196,221,10,169,229,188,
    96,150,238,162,61,227,163,218,179,153,255,223,219,30,139,10,105,103,26,
    50,62,222,163,41,227,99,14,13,25,31,115,197,204,248,152,236,94,198,199,
    181,47,20,82,198,199,240,23,145,143,163,226,218,59,19,227,57,109,116,162,
    57,227,163,97,11,212,198,160,181,62,119,111,70,180,203,199,53,218,204,
    60,187,62,218,227,227,146,61,144,143,105,165,200,71,191,208,229,95,135,
    184,207,199,230,151,188,207,199,205,45,93,239,150,210,199,222,37,55,213,
    223,80,72,69,244,223,178,30,245,195,171,47,35,253,77,218,248,92,219,143,
    38,20,107,41,205,69,244,175,246,76,145,255,255,197,189,34,250,255,237,
    221,162,226,66,90,26,247,139,179,212,138,230,162,134,133,180,32,103,150,
    38,242,87,196,207,82,26,139,248,104,214,26,233,255,41,174,253,235,64,111,
    17,221,109,198,22,82,29,122,138,107,80,155,205,130,218,252,231,24,164,
    231,141,253,176,110,33,125,62,60,152,165,139,227,154,134,39,134,34,93,
    151,28,128,116,77,25,141,116,13,137,247,229,245,221,241,40,103,59,214,
    34,29,109,227,90,252,209,227,80,252,166,147,112,231,66,170,205,153,165,
    27,119,47,164,186,6,232,138,156,89,106,91,134,244,148,119,70,122,10,173,
    145,158,121,123,161,252,28,220,9,233,168,83,142,116,148,118,71,58,174,
    107,135,116,140,235,138,116,252,184,55,210,209,41,218,165,227,111,109,
    80,142,62,107,133,59,21,210,242,210,96,150,142,107,143,250,249,135,136,
    67,95,223,46,40,63,171,35,126,140,207,62,41,213,147,167,3,189,91,143,174,
    165,61,144,174,67,187,35,93,71,4,233,122,190,45,210,245,147,119,234,209,
    117,177,113,84,143,174,150,238,213,163,107,230,0,164,235,220,222,72,87,
    171,158,72,215,192,189,81,219,19,59,34,93,31,116,13,102,105,100,103,164,
    107,83,196,147,191,241,113,223,24,170,223,13,233,188,91,191,214,163,243,
    188,50,20,243,236,208,183,75,33,13,111,28,204,82,179,38,193,162,244,85,
    206,58,169,71,232,51,46,111,218,75,27,252,188,208,14,249,89,16,52,48,127,
    169,68,126,14,160,175,62,63,245,250,35,237,195,232,170,207,207,215,157,
    144,159,107,246,68,126,142,141,118,248,89,31,215,252,204,160,179,62,63,
    93,229,163,62,45,91,186,160,241,87,196,103,125,154,230,84,32,31,23,244,
    66,154,26,230,44,164,198,251,32,63,13,7,34,63,223,28,90,72,13,232,107,
    124,20,210,247,201,76,148,239,49,71,32,125,207,7,233,219,117,6,210,55,
    107,54,210,55,250,158,148,118,240,94,179,187,209,123,75,150,161,247,122,
    220,143,222,123,233,1,244,222,143,119,161,247,14,14,122,111,204,74,228,
    107,183,248,156,159,151,227,58,198,77,73,33,153,102,169,214,184,48,205,
    210,29,114,91,172,189,251,246,68,237,45,107,129,218,155,103,141,42,214,
    222,20,193,139,181,55,179,25,106,111,124,188,47,79,71,55,70,13,205,206,
    153,165,195,226,190,246,175,52,158,139,119,44,164,107,115,102,233,249,
    142,216,168,144,118,232,20,204,210,91,198,109,177,252,61,25,113,228,111,
    66,196,241,238,162,208,33,127,39,53,71,121,123,197,224,44,142,113,160,
    83,139,141,131,95,140,219,98,235,208,97,161,179,121,172,43,104,29,154,
    17,62,172,67,197,147,228,154,175,195,131,124,117,153,142,124,117,221,206,
    253,38,32,127,13,166,32,95,223,198,115,244,190,31,215,250,117,235,45,230,
    182,247,103,124,137,222,63,251,101,244,222,188,215,208,130,114,125,206,
    44,157,244,41,202,207,56,11,218,142,218,89,164,95,118,148,159,199,110,
    69,249,217,125,21,202,79,139,207,80,222,151,61,141,198,207,138,117,65,
    235,216,19,40,222,161,31,162,113,211,238,47,246,44,113,199,136,47,61,169,
    183,254,110,36,238,173,193,216,199,180,223,72,188,207,111,66,57,217,198,
    44,61,114,7,138,123,112,80,220,67,238,69,113,167,111,231,214,21,40,254,
    67,116,53,226,243,176,27,145,142,249,203,49,246,53,227,162,145,126,120,
    198,56,106,164,15,63,206,153,165,89,15,33,93,251,222,135,198,113,191,184,
    223,36,75,67,226,186,105,33,221,20,215,77,179,244,167,208,221,180,40,53,
    141,118,244,211,212,208,97,253,123,59,103,150,110,191,29,245,233,218,156,
    89,250,115,232,208,111,83,66,191,126,171,206,89,72,187,94,143,250,239,
    137,11,10,105,39,235,229,71,57,179,244,239,11,81,62,54,231,204,210,23,
    183,163,188,148,221,137,242,242,201,109,40,47,215,223,140,250,161,246,
    26,148,143,87,130,242,209,229,38,148,135,85,75,81,30,254,184,3,229,161,
    206,221,40,15,239,70,187,242,240,147,122,96,39,121,120,41,167,61,230,207,
    104,92,166,237,108,114,9,202,199,29,65,227,243,208,120,207,248,220,115,
    49,242,125,202,162,96,33,205,136,107,227,180,232,6,52,78,151,95,139,252,
    78,186,28,249,29,118,5,242,249,217,201,214,0,253,189,250,65,228,175,230,
    54,228,227,217,87,145,238,159,228,191,49,221,231,26,39,141,181,119,255,
    181,168,157,149,207,162,247,103,211,110,186,164,47,189,91,226,223,40,154,
    74,228,101,15,251,113,137,188,172,182,238,149,104,239,40,94,75,228,229,
    27,222,74,228,229,110,154,74,180,223,163,28,181,223,65,78,74,252,123,201,
    255,75,98,30,136,85,18,251,73,60,47,47,159,70,28,255,127,208,255,75,228,
    97,114,80,30,142,124,181,144,74,196,252,228,53,148,135,94,246,249,18,121,
    120,206,103,37,250,191,42,244,232,255,89,180,150,136,121,86,43,164,191,
    44,244,201,67,15,235,106,137,207,138,204,123,195,43,175,133,130,7,155,
    124,134,89,250,122,139,156,243,51,224,115,228,103,166,185,215,132,159,
    165,214,161,38,252,60,102,125,104,194,207,112,147,166,9,63,147,182,122,
    142,135,147,247,112,205,207,150,15,92,243,115,185,117,163,9,15,23,5,105,
    255,250,55,247,245,111,157,223,131,89,90,249,127,168,35,30,207,153,165,
    115,222,71,30,42,255,133,60,92,251,13,242,183,250,123,228,239,113,107,
    93,19,254,238,111,24,204,210,213,113,205,223,214,208,205,223,203,76,52,
    225,239,59,253,111,154,164,238,183,32,63,191,170,199,76,147,116,217,67,
    200,207,165,219,217,118,57,242,181,234,126,228,171,185,113,210,148,175,
    205,198,89,83,190,38,62,134,124,61,122,30,242,245,83,60,199,215,127,140,
    231,166,124,221,96,60,55,229,107,254,50,212,79,151,171,187,154,234,167,
    110,79,33,31,127,141,56,124,124,21,181,137,190,109,29,237,211,191,133,
    127,195,53,93,242,31,164,175,138,207,102,244,45,249,37,165,157,205,191,
    171,115,102,105,225,119,232,243,54,51,82,50,141,211,139,199,161,235,13,
    65,250,79,61,16,233,191,57,72,255,123,214,217,93,232,255,32,62,167,255,
    243,105,72,127,58,9,233,255,38,218,161,127,231,67,144,254,22,199,34,253,
    229,115,144,254,1,199,32,253,63,197,251,244,191,63,19,233,127,117,22,242,
    112,224,108,212,15,191,6,249,121,254,80,148,255,5,135,163,252,247,61,5,
    173,39,83,207,8,102,169,60,103,81,90,126,102,176,78,250,232,172,96,221,
    212,243,236,96,189,52,122,65,176,126,154,115,78,176,65,154,121,110,112,
    135,84,124,148,190,224,247,107,113,154,243,219,129,158,230,242,51,220,
    188,109,206,119,145,120,205,249,190,55,238,243,123,204,9,200,239,92,62,
    155,243,185,148,175,230,124,238,199,79,243,168,27,142,68,99,117,122,206,
    44,29,195,111,115,126,107,229,163,57,191,165,241,190,9,125,81,206,44,61,
    19,159,243,251,169,121,193,78,26,161,110,100,39,245,183,159,178,147,90,
    198,53,29,147,130,116,212,85,239,177,147,186,219,15,217,73,203,226,154,
    158,175,223,176,247,123,255,134,119,208,251,215,255,13,189,255,195,123,
    232,253,11,227,58,214,157,127,162,247,71,127,130,222,175,180,47,237,230,
    253,77,65,253,55,236,239,200,87,183,120,158,175,119,163,61,227,124,233,
    220,66,178,133,167,191,6,181,127,142,243,83,11,237,247,138,107,237,255,
    162,126,105,161,253,35,230,160,246,175,82,247,180,48,23,255,145,51,75,
    71,168,135,90,136,51,45,104,46,77,205,153,165,79,212,65,45,196,25,22,207,
    201,223,87,234,134,150,226,60,208,1,197,185,78,29,220,82,156,23,187,161,
    56,99,123,160,56,103,170,111,91,218,31,183,49,75,221,229,171,37,63,135,
    196,251,226,220,220,19,249,153,162,126,110,201,207,90,245,117,75,113,154,
    169,43,90,138,51,188,18,245,211,228,156,89,234,21,212,79,157,212,209,45,
    245,211,111,17,215,184,60,61,218,49,46,127,237,141,250,169,113,47,52,46,
    23,245,65,227,114,176,117,107,247,208,107,254,236,78,239,141,95,32,189,
    237,191,69,122,87,197,231,244,150,126,133,242,242,75,220,167,179,155,121,
    216,202,252,219,244,35,122,239,38,247,91,69,61,238,253,86,222,59,204,243,
    173,188,119,180,250,164,149,247,214,169,47,90,121,239,130,205,168,176,
    252,49,103,150,6,196,251,124,190,245,51,242,217,218,124,110,197,231,238,
    65,107,85,105,206,44,189,105,191,50,84,210,43,39,163,3,232,235,57,179,
    116,248,69,40,238,34,231,213,82,113,167,90,55,74,197,109,117,61,138,59,
    200,185,182,84,220,53,206,205,165,242,58,39,222,19,175,191,115,109,169,
    120,79,198,243,226,117,179,62,149,138,247,243,149,104,158,159,97,95,46,
    149,207,3,157,155,75,173,187,13,46,8,102,105,129,245,172,212,248,111,116,
    103,48,75,91,110,69,249,93,108,29,45,149,223,71,35,174,252,62,28,215,177,
    31,58,111,151,154,247,7,57,143,151,154,247,245,67,143,241,216,212,126,
    93,106,192,159,158,51,75,85,252,217,18,210,197,246,231,61,228,243,60,186,
    246,112,30,56,63,167,115,223,101,200,95,249,165,200,223,59,65,254,102,
    7,249,251,61,222,227,239,222,32,127,173,162,61,254,218,214,202,169,118,
    223,177,14,180,214,238,35,53,40,95,31,28,141,218,91,108,221,104,173,189,
    69,214,195,214,177,255,30,143,198,253,178,19,131,89,218,199,250,215,58,
    234,242,201,168,189,73,214,177,214,242,181,209,252,111,45,79,39,88,103,
    90,203,199,225,167,4,179,116,149,28,154,218,233,110,231,215,54,226,181,
    152,130,226,221,123,60,138,119,253,25,24,231,235,106,20,239,21,227,182,
    13,253,79,27,167,109,196,121,198,60,104,19,231,52,227,177,141,120,87,218,
    239,218,136,119,93,149,251,198,125,127,231,229,54,198,253,240,156,206,
    169,147,81,63,205,56,26,141,251,163,231,161,113,191,248,28,212,47,135,
    157,130,250,229,177,19,80,191,28,119,12,234,151,125,14,65,253,50,62,168,
    95,38,79,64,253,50,125,59,215,56,39,183,177,239,175,48,79,218,234,135,
    5,125,131,89,26,182,15,242,177,161,31,242,113,177,121,213,150,143,189,
    43,144,143,246,241,60,31,85,113,173,6,232,209,43,104,255,141,231,248,57,
    48,218,225,103,211,0,228,231,206,82,53,181,124,237,123,16,202,215,220,
    17,40,95,155,246,197,200,215,16,20,231,32,231,192,118,226,44,116,94,107,
    39,206,174,206,135,237,204,163,13,206,121,237,228,235,50,231,210,118,218,
    191,51,218,209,254,65,250,171,157,246,159,211,255,237,244,211,238,227,
    80,158,206,27,141,198,243,197,53,193,44,173,217,15,229,235,235,104,79,
    77,48,181,79,48,75,239,228,44,74,93,156,27,219,217,128,255,85,17,228,63,
    238,219,128,79,28,16,172,147,62,9,125,205,234,166,31,163,157,102,245,82,
    175,225,193,250,169,99,117,176,65,218,18,126,154,17,24,58,236,231,75,114,
    58,87,143,9,214,73,199,56,207,182,211,15,123,171,179,218,233,135,253,236,
    207,237,228,127,249,84,148,255,159,205,31,71,168,52,252,79,40,63,109,23,
    160,252,76,50,142,246,148,159,58,241,185,252,188,31,215,242,115,212,185,
    40,63,187,169,143,247,212,15,173,251,169,229,226,123,3,250,203,188,191,
    40,168,63,175,200,153,165,235,228,185,76,59,245,248,42,211,206,201,242,
    80,166,157,239,249,46,139,121,16,207,153,7,111,228,44,74,7,236,141,113,
    192,237,30,116,206,112,190,46,139,126,237,134,106,173,27,226,115,7,186,
    113,206,235,101,242,255,90,123,148,255,143,202,80,158,71,202,79,153,113,
    249,166,124,149,25,151,115,135,161,113,249,142,254,43,147,135,121,131,
    208,134,254,125,206,44,181,26,28,44,74,183,59,159,151,201,203,226,136,
    35,47,63,92,102,172,145,241,204,149,200,215,250,211,81,94,254,113,9,198,
    126,166,142,104,207,79,199,133,24,245,206,60,140,117,66,29,209,94,94,54,
    94,138,241,253,65,188,79,127,243,120,207,184,185,208,124,110,79,247,29,
    167,34,221,199,168,67,218,27,55,75,150,160,249,181,224,42,228,99,167,197,
    200,199,31,87,32,31,11,47,68,62,102,93,142,230,215,37,215,32,63,233,6,
    212,175,23,158,143,116,255,79,191,154,226,105,147,126,222,43,234,149,42,
    164,251,32,113,108,153,233,36,241,59,184,255,135,231,59,184,127,233,5,
    200,79,151,243,144,159,251,230,35,63,67,232,239,192,207,69,234,161,14,
    252,236,171,30,234,192,207,175,103,111,59,43,236,71,127,7,126,134,198,
    125,126,70,203,79,7,235,198,81,57,179,244,118,206,162,116,245,105,168,
    206,126,236,79,65,235,123,180,103,126,172,137,248,230,71,243,120,159,223,
    33,113,205,231,20,121,234,192,231,135,23,33,63,229,111,201,49,221,215,
    188,137,241,125,199,38,164,251,44,251,96,71,186,191,182,175,117,164,123,
    214,255,144,238,205,246,252,142,116,215,117,30,239,24,231,96,231,210,142,
    81,55,59,231,118,164,251,185,231,144,238,19,54,32,221,43,213,63,29,245,
    195,29,65,253,240,175,165,168,31,54,220,133,214,221,214,119,7,213,39,241,
    190,254,168,116,190,237,168,160,255,54,103,150,154,57,183,117,164,247,
    233,181,168,95,154,62,137,250,165,207,71,168,95,26,170,143,58,26,87,123,
    255,21,249,121,251,190,66,82,130,164,223,31,65,126,122,57,127,118,226,
    231,106,245,121,39,103,155,103,115,102,233,187,71,145,175,207,158,192,
    168,11,86,35,95,111,61,137,124,253,172,174,239,196,87,251,184,230,171,
    79,180,27,223,243,58,71,116,178,71,92,157,51,75,83,163,61,254,166,216,
    71,59,241,87,120,174,144,127,7,188,248,129,66,254,221,207,125,57,179,52,
    42,218,231,239,203,149,200,223,238,171,130,246,155,53,104,125,218,235,
    241,96,150,58,108,231,141,57,139,210,219,17,159,239,227,156,55,58,241,
    125,218,254,133,212,89,221,242,96,127,228,175,179,253,167,51,127,55,198,
    125,253,85,127,56,70,189,51,24,249,90,22,228,43,217,199,58,243,181,120,
    60,242,245,172,125,160,51,95,207,111,231,147,246,187,206,252,61,85,139,
    250,237,169,120,158,175,51,131,124,253,253,0,228,235,166,136,99,60,157,
    58,26,249,249,204,62,208,217,184,58,213,190,208,89,63,37,251,70,103,122,
    255,136,118,233,93,167,254,235,172,159,198,218,111,58,235,159,61,173,219,
    74,198,52,192,58,213,133,254,106,235,84,23,250,251,91,215,187,208,255,
    156,117,169,11,221,235,173,67,93,232,158,211,3,233,253,221,58,213,133,
    190,219,172,115,93,232,155,100,61,235,66,95,239,71,212,252,218,27,103,
    220,118,213,222,51,127,160,246,246,55,190,186,106,239,240,123,81,62,218,
    223,143,177,207,221,131,177,190,58,247,119,149,143,234,53,24,223,227,4,
    227,251,155,103,80,156,51,95,64,113,94,223,136,226,236,234,156,219,85,
    30,14,248,7,54,142,218,161,144,186,202,195,166,104,95,30,150,70,60,121,
    88,30,122,244,231,99,171,131,89,250,212,184,237,106,223,248,125,93,48,
    75,243,95,65,121,185,229,165,96,33,173,127,57,152,165,181,230,97,87,235,
    230,119,57,179,116,183,115,99,87,227,122,222,10,148,183,157,173,191,229,
    124,110,166,189,156,207,157,172,195,229,124,254,187,51,242,121,140,188,
    149,243,57,90,62,203,195,103,59,180,254,127,148,51,75,21,242,86,206,239,
    76,251,112,121,212,229,242,89,30,253,207,83,57,191,223,148,35,191,107,
    229,181,156,223,135,173,255,229,198,111,147,78,193,44,181,182,191,149,
    243,247,194,30,200,223,138,54,193,44,93,170,94,115,52,72,253,213,157,221,
    232,58,225,102,140,243,247,61,72,87,149,243,114,55,186,46,88,130,116,61,
    169,14,236,198,195,145,55,34,61,123,93,129,241,253,157,249,213,141,158,
    167,238,217,198,171,213,175,221,232,105,249,32,210,211,255,46,148,247,
    127,155,207,221,228,125,139,121,211,77,222,135,62,92,72,134,88,122,217,
    60,219,59,198,147,245,220,81,35,93,105,253,236,238,250,45,251,70,119,122,
    222,176,190,119,167,231,45,235,115,119,122,222,221,206,71,111,68,186,206,
    81,95,58,218,164,122,135,161,247,214,169,47,123,120,175,158,243,101,15,
    239,45,84,95,246,240,252,117,206,153,61,60,255,130,115,99,15,62,190,85,
    111,246,224,99,213,193,200,71,125,245,102,15,250,27,57,79,247,144,215,
    45,113,63,254,46,20,215,124,212,53,190,148,138,169,205,199,40,206,181,
    207,163,253,124,234,187,40,78,217,19,40,206,173,214,209,158,226,60,176,
    30,227,92,21,159,139,115,128,115,97,207,56,39,174,66,113,110,49,206,122,
    138,83,109,93,236,41,206,143,79,161,56,43,227,190,190,189,202,184,238,
    41,111,11,159,70,121,187,215,184,235,41,111,151,188,143,250,243,98,231,
    205,158,230,109,181,243,103,79,243,182,171,117,186,167,241,121,183,113,
    216,211,188,157,253,56,26,135,39,27,215,61,141,195,85,106,126,83,60,85,
    42,246,43,232,223,193,121,174,66,158,22,218,204,43,232,143,205,177,130,
    254,27,21,133,21,113,78,102,182,130,254,42,235,65,5,253,199,170,209,42,
    232,191,40,62,119,14,189,52,103,150,22,182,69,62,110,85,236,84,240,177,
    33,218,51,199,150,231,204,82,11,157,83,97,159,107,153,51,75,95,57,244,
    85,240,53,86,177,94,193,215,241,14,239,21,124,189,97,78,85,240,181,194,
    24,173,240,239,193,156,89,122,32,116,241,183,44,218,243,76,199,184,54,
    239,134,25,60,21,124,222,110,81,170,208,198,146,174,193,44,53,136,107,
    126,203,86,170,181,249,61,235,176,66,82,10,167,58,234,244,94,124,23,31,
    139,124,247,85,143,247,178,175,156,146,51,75,141,143,196,186,133,212,60,
    103,150,246,13,218,51,135,231,204,82,19,117,124,47,249,248,119,80,62,94,
    119,142,239,37,31,155,14,70,253,121,72,180,43,15,99,103,160,60,252,236,
    92,222,75,127,126,155,179,144,62,181,46,41,197,211,131,47,162,124,175,
    212,47,189,229,123,168,254,238,45,206,226,156,89,218,67,127,247,22,103,
    177,254,235,45,206,86,251,99,111,113,30,48,15,149,248,169,198,188,235,
    195,71,191,171,145,143,183,205,183,62,113,94,118,238,235,163,189,151,227,
    190,254,219,18,247,227,251,76,243,181,143,122,253,163,155,130,89,106,31,
    237,104,239,112,231,183,62,244,45,249,94,46,180,123,162,243,112,95,237,
    54,118,254,233,171,221,51,127,67,237,182,112,142,238,171,221,101,206,213,
    125,233,188,45,167,115,197,127,49,190,87,183,222,245,165,115,173,58,163,
    175,250,116,122,42,160,253,223,62,223,55,214,167,160,124,204,166,199,150,
    151,118,177,127,87,138,179,230,21,52,127,102,189,22,204,242,181,167,178,
    168,40,125,171,223,43,197,125,112,5,138,123,190,245,191,146,159,13,111,
    160,120,202,165,84,41,94,31,231,243,74,62,214,235,195,74,249,223,215,58,
    83,41,94,83,235,84,165,125,254,155,197,193,44,141,119,110,174,228,115,
    209,99,104,94,237,231,80,90,105,125,236,210,38,152,165,231,140,155,74,
    227,239,169,107,209,248,59,67,29,80,105,94,77,183,62,85,26,119,187,232,
    207,202,93,98,253,64,227,109,171,250,161,210,188,234,107,252,87,26,103,
    15,183,44,36,91,100,90,107,191,238,199,215,50,251,106,63,94,30,144,212,
    126,180,173,201,153,165,163,212,14,253,248,249,98,20,242,211,32,254,102,
    192,207,235,180,246,227,231,199,184,207,207,55,138,186,126,230,215,198,
    46,193,44,149,56,15,42,25,210,187,206,135,253,181,191,223,68,212,254,23,
    6,83,127,121,42,214,126,127,237,126,60,16,181,219,117,4,106,247,75,237,
    245,215,238,150,97,168,221,190,251,97,252,93,84,99,253,227,239,179,234,
    137,254,250,229,123,117,66,127,249,185,57,158,151,159,43,70,162,249,185,
    100,59,167,49,215,95,126,102,215,160,252,76,156,134,242,243,83,220,151,
    159,173,209,158,252,172,137,107,249,121,79,39,247,151,159,177,242,208,
    95,126,38,89,111,7,68,125,109,125,29,64,255,111,179,144,254,145,214,237,
    1,244,191,112,4,210,127,228,81,24,245,67,124,78,255,177,113,223,184,157,
    179,157,199,111,231,233,57,179,116,78,144,175,207,172,235,3,98,254,197,
    53,95,27,230,34,95,199,157,128,124,253,112,34,198,239,18,212,255,3,212,
    215,203,167,5,179,52,48,104,28,124,51,37,152,165,135,114,22,165,93,226,
    185,146,58,169,81,254,121,221,84,123,64,176,94,106,24,250,229,225,145,
    35,81,30,102,205,64,121,216,55,124,200,195,144,136,111,125,218,154,51,
    75,59,132,111,117,65,215,227,131,244,30,138,242,178,41,222,151,151,203,
    46,46,36,93,150,254,99,188,14,148,151,115,237,191,3,227,251,138,243,80,
    94,158,118,222,28,40,47,151,31,129,242,178,68,125,55,48,190,199,178,158,
    12,212,175,111,30,136,81,15,204,198,248,59,172,245,109,32,255,43,226,61,
    99,106,200,57,193,44,45,159,133,145,135,67,80,30,42,174,71,253,218,46,
    222,183,254,46,82,7,12,228,107,190,117,97,32,95,117,78,68,190,26,133,46,
    190,190,182,78,12,228,235,144,197,65,235,119,196,163,191,139,250,118,16,
    253,127,91,139,217,182,26,112,16,253,21,119,184,142,239,187,213,241,131,
    232,222,67,253,60,136,238,121,247,35,221,167,170,23,6,209,125,64,60,71,
    119,95,235,193,32,186,143,53,63,7,233,183,29,227,115,122,7,199,253,157,
    98,157,68,122,103,197,251,244,118,204,89,72,237,212,217,131,232,238,160,
    158,24,68,119,207,103,144,238,123,158,69,186,223,181,190,12,50,46,47,15,
    93,241,187,18,235,204,32,249,63,196,57,205,84,77,107,140,155,193,116,95,
    50,27,233,158,110,156,12,246,236,31,250,117,48,221,143,59,39,14,86,135,
    205,156,30,204,210,123,198,229,224,248,77,134,241,49,152,254,122,219,89,
    28,247,249,184,97,18,242,113,187,115,221,96,62,46,80,167,12,230,227,215,
    129,198,134,120,223,247,197,24,255,234,185,33,226,109,116,254,31,18,223,
    87,142,64,249,170,170,70,113,223,86,199,13,145,175,47,212,207,67,196,235,
    88,137,226,140,14,138,243,207,145,24,127,167,84,143,15,17,231,52,231,249,
    33,226,172,142,118,228,171,99,92,203,87,31,117,250,16,249,106,26,113,229,
    233,117,231,249,33,242,244,127,206,243,67,228,233,252,214,40,63,95,168,
    19,135,200,207,227,67,81,126,62,142,56,250,119,119,251,201,80,186,103,
    217,55,134,210,189,255,117,72,247,13,198,207,80,186,151,92,133,116,47,
    180,223,12,141,253,223,57,221,82,147,166,47,66,207,175,13,122,190,237,
    101,104,29,156,151,51,75,175,230,44,74,47,57,71,15,243,254,230,171,49,
    206,59,241,60,223,109,237,143,251,122,127,247,147,11,201,146,149,126,85,
    223,58,26,165,91,213,13,195,221,159,166,14,24,174,221,65,99,145,142,219,
    36,119,184,118,254,103,29,31,174,157,141,54,201,225,218,249,151,117,116,
    120,236,19,214,177,225,244,181,141,107,227,166,215,118,94,219,37,152,109,
    251,206,76,251,229,198,76,85,212,157,45,81,251,247,239,142,49,30,226,111,
    63,218,255,52,62,215,126,141,92,87,105,191,240,98,33,85,121,247,174,13,
    184,195,182,191,5,84,233,159,123,253,191,170,97,33,253,80,28,52,255,244,
    81,149,127,159,201,113,149,119,95,139,235,56,151,25,143,85,250,103,142,
    190,138,191,5,238,251,178,107,253,99,88,165,42,253,179,42,226,249,247,
    176,115,64,149,126,250,209,57,162,74,27,11,227,90,63,125,232,217,42,253,
    212,184,189,26,144,254,143,205,195,234,200,155,243,102,53,253,47,139,81,
    77,127,156,237,170,227,251,112,239,86,199,239,162,182,115,13,127,213,124,
    156,108,191,171,166,125,79,126,170,105,111,25,215,180,175,139,107,121,
    59,36,158,163,249,85,231,135,106,250,207,17,191,90,219,245,156,35,170,
    105,159,91,138,124,244,148,183,106,62,90,68,60,218,222,164,185,218,255,
    123,208,88,29,231,224,230,219,126,187,245,108,220,143,243,191,123,213,
    234,182,29,183,243,169,156,89,26,19,159,239,90,148,254,43,63,213,222,125,
    223,60,183,165,165,137,246,227,17,252,237,99,190,143,224,239,224,184,207,
    223,4,235,195,8,254,218,197,231,124,205,191,23,249,154,122,39,242,117,
    146,117,102,4,95,31,254,5,227,119,107,214,151,17,241,253,164,124,141,160,
    251,56,235,201,8,58,191,136,251,52,28,188,220,88,143,223,55,220,142,177,
    159,95,128,234,148,135,114,102,233,166,139,81,220,19,157,63,246,23,119,
    129,115,203,254,226,118,185,13,227,251,126,231,201,253,197,189,124,1,138,
    123,194,173,104,63,63,55,103,150,234,222,96,239,243,252,28,243,101,164,
    251,207,159,142,242,218,210,60,24,41,175,171,175,66,121,93,39,206,72,250,
    106,196,25,25,121,93,140,242,154,180,59,146,222,159,175,197,248,222,210,
    58,48,42,206,37,230,253,40,122,251,238,143,242,115,151,245,96,84,252,142,
    43,62,167,115,147,243,227,40,245,91,195,1,193,44,221,54,22,233,221,103,
    12,198,223,81,227,61,122,231,91,39,70,233,255,121,39,21,210,104,237,238,
    16,204,172,157,57,179,212,255,0,212,254,40,251,201,104,237,239,175,62,
    26,173,253,97,115,144,175,206,234,225,209,177,127,219,135,70,219,175,231,
    231,204,210,247,103,97,180,127,17,242,253,93,206,44,45,152,137,250,229,
    143,233,168,95,166,169,179,70,199,249,33,167,125,211,126,51,218,249,225,
    230,156,89,250,49,226,200,207,147,115,81,126,214,205,67,181,212,219,57,
    179,244,104,188,215,52,254,238,136,242,244,98,220,215,175,23,171,227,71,
    27,91,63,135,15,251,242,214,156,234,254,249,193,162,180,82,93,63,58,254,
    62,114,54,26,119,167,171,107,148,60,233,83,135,166,26,254,55,231,204,210,
    80,231,162,26,190,63,176,56,214,240,253,157,69,175,70,94,59,237,27,116,
    94,49,88,107,248,127,91,221,86,35,175,109,213,107,53,234,226,173,57,179,
    180,88,29,87,35,191,247,88,191,106,248,158,16,239,219,175,79,200,153,165,
    157,212,117,177,198,92,24,241,140,135,215,213,119,53,252,174,113,126,170,
    225,119,183,96,147,216,79,144,207,19,21,17,53,124,174,27,131,124,174,138,
    247,141,139,198,65,126,38,57,4,213,56,251,45,206,169,253,242,66,242,104,
    122,40,24,251,145,67,247,24,253,249,83,39,228,171,183,115,213,24,190,230,
    78,69,253,121,140,126,30,195,79,83,249,31,163,63,119,205,153,165,125,172,
    183,99,244,231,37,221,48,126,31,17,237,241,243,106,92,235,199,203,13,202,
    49,177,255,201,195,24,62,218,243,61,134,143,191,203,223,24,62,14,138,56,
    124,84,168,23,199,240,81,220,1,213,254,237,67,135,115,95,143,206,193,162,
    116,97,206,58,233,162,156,117,83,113,196,109,86,47,141,201,89,63,245,137,
    184,205,26,164,221,34,110,252,125,69,126,98,13,25,22,159,203,195,127,131,
    242,80,28,241,244,235,23,206,229,82,159,190,177,207,140,141,239,25,157,
    191,199,242,191,52,174,249,255,214,249,125,44,255,83,212,3,164,167,251,
    213,1,227,60,55,72,157,55,206,115,227,205,151,113,81,183,218,191,198,197,
    239,106,206,192,248,253,151,122,113,92,253,248,59,0,198,239,17,213,129,
    227,228,103,133,250,98,156,252,124,161,222,24,39,63,79,170,19,198,201,
    207,254,230,223,56,249,57,86,125,57,78,126,90,170,11,199,201,203,242,90,
    49,98,189,83,47,142,23,247,27,241,198,139,251,185,231,199,199,223,101,
    198,163,184,181,158,31,47,238,94,195,81,220,171,227,190,254,185,37,103,
    150,94,162,123,188,248,55,142,67,241,203,204,239,241,226,95,101,61,24,
    47,254,255,197,181,248,111,13,66,241,91,5,245,207,127,227,121,58,90,132,
    14,253,179,108,34,58,187,172,204,153,165,211,232,31,111,188,125,30,207,
    201,119,203,208,37,223,27,131,242,93,119,20,202,119,243,208,45,223,189,
    205,111,93,159,54,170,67,39,168,3,54,231,204,210,110,230,251,4,62,94,140,
    207,249,40,53,31,39,240,49,233,84,140,186,94,61,59,129,238,15,140,183,
    9,116,175,85,15,76,160,123,209,25,72,119,119,117,242,4,186,79,49,207,39,
    208,217,211,121,81,137,156,86,197,159,124,204,215,71,115,102,249,111,66,
    106,179,162,180,143,125,177,86,30,127,121,16,229,177,231,70,172,187,173,
    214,171,245,239,104,251,77,109,252,94,38,158,215,127,87,241,86,171,141,
    6,188,212,198,239,74,228,166,214,190,254,124,73,48,75,27,237,55,181,116,
    28,231,157,90,58,230,70,59,37,219,126,67,92,75,207,116,251,228,196,248,
    126,179,3,234,199,21,234,172,137,226,95,21,20,127,150,125,123,162,248,
    247,216,215,39,202,219,76,235,201,36,207,47,48,31,39,197,247,161,101,232,
    249,238,234,153,73,158,191,206,248,159,228,249,110,214,165,73,98,117,83,
    196,78,82,159,30,102,254,79,162,183,110,220,167,247,150,120,159,222,166,
    246,179,201,218,219,235,86,140,223,69,60,128,218,251,222,254,57,89,123,
    127,191,188,144,255,54,250,248,135,208,250,117,241,74,228,101,165,243,
    237,100,237,213,198,251,218,59,120,21,106,111,104,188,167,31,22,94,129,
    250,161,161,122,48,126,235,212,238,118,228,127,228,50,148,163,113,242,
    59,89,30,214,235,143,201,242,208,76,158,38,155,223,183,168,231,39,155,
    223,163,31,11,102,105,90,196,147,227,178,117,104,252,252,39,40,15,247,
    4,213,3,211,214,7,179,116,127,206,162,212,92,61,50,217,58,246,85,206,44,
    61,243,191,252,171,241,116,243,199,200,223,207,91,144,191,19,127,193,248,
    221,205,111,200,223,75,198,194,20,249,250,248,75,215,241,189,223,239,200,
    95,212,128,83,248,171,112,38,156,194,223,100,123,200,20,254,118,105,129,
    252,149,125,228,57,254,190,255,39,198,247,6,159,33,127,255,248,20,249,
    107,240,3,242,55,241,15,140,253,198,216,152,98,94,156,17,237,240,117,232,
    206,200,87,215,208,197,215,14,161,203,88,122,229,67,52,47,154,220,164,
    70,81,79,220,126,15,210,59,127,37,198,111,205,30,64,58,126,125,13,233,
    232,120,55,138,123,193,83,168,189,254,171,81,59,255,15,104,239,202,179,
};
