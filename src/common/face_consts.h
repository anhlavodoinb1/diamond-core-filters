/*
 *
 *
 *                          Diamond 1.0
 * 
 *            Copyright (c) 2002-2004, Intel Corporation
 *                         All Rights Reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *
 *    * Neither the name of Intel nor the names of its contributors may
 *      be used to endorse or promote products derived from this software 
 *      without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



/* the number of filter stages in the face detector. we'd like more
 * than one, but this keep coming back to bite us, so use 1 for
 * now. */
#define NUM_FACE_DETECTORS 1

#define STRIDE_DEFAULT 32

#define PDISTANCE_DEFAULT 0.25
#define NDISTANCE_DEFAULT 0.8

#define MAX_NAME 128
#define MAX_PATH 256

#define MAX_SCAPE 64
#define MAX_FILT (MAX_SCAPE - 12)
/* fudge factor of 12 to cover utility+face filters */

/* #define NUM_LAP_PYR_LEVELS 4 */
/* XXX #define TEXTURE_NUM_CHANNELS 3 */
#define FILTER_TYPE_DEFAULT 0

#define MAX_ALBUMS 32
#define MAX_STRING 1024

/* max number of regexes in search */
#define MAX_REGEX 32

/* max number of regions that can be selected in zoomed window */
#define MAX_SELECT 32


/* dimension of a patch (assume square) */
#define MIN_TEXTURE_SIZE 32
#define STD_COLOR_SIZE   16