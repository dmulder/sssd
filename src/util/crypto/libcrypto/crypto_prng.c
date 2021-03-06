/*
    Copyright (C) Red Hat 2019

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <openssl/rand.h>

#include "util/util_errors.h"
#include "util/crypto/sss_crypto.h"

int sss_rand(void)
{
    static bool srand_done = false;
    int result;

    if (RAND_bytes((unsigned char*)&result, (int)sizeof(int)) == 1) {
        return result;
    }

    /* Fallback to libc `rand()`
     * Coverity might complain here: "DC.WEAK_CRYPTO (CWE-327)"
     * But if `RAND_bytes()` failed then there is no entropy available
     * so it doesn't make any sense to try reading "/dev/[u]random"
     */
    if (!srand_done) {
        srand(time(NULL) * getpid());
        srand_done = true;
    }
    return rand();
}

int sss_generate_csprng_buffer(uint8_t *buf, size_t size)
{
    if ((buf == NULL) || (size > INT_MAX)) {
        return EINVAL;
    }

    if (RAND_bytes((unsigned char *)buf, (int)size) == 1) {
        return EOK;
    }

    return EAGAIN;
}
