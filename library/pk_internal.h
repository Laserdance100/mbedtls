/**
 * \file pk_internal.h
 *
 * \brief Public Key abstraction layer: internal (i.e. library only) functions
 *        and definitions.
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#ifndef MBEDTLS_PK_INTERNAL_H
#define MBEDTLS_PK_INTERNAL_H

#include "mbedtls/pk.h"

#if defined(MBEDTLS_ECP_LIGHT)
#include "mbedtls/ecp.h"
#endif

#if defined(MBEDTLS_USE_PSA_CRYPTO)
#include "psa/crypto.h"
#endif

#if defined(MBEDTLS_PSA_CRYPTO_C)
#include "mbedtls/psa_util.h"
#define PSA_PK_TO_MBEDTLS_ERR(status) psa_pk_status_to_mbedtls(status)
#define PSA_PK_RSA_TO_MBEDTLS_ERR(status) PSA_TO_MBEDTLS_ERR_LIST(status,     \
                                                                  psa_to_pk_rsa_errors,            \
                                                                  psa_pk_status_to_mbedtls)
#define PSA_PK_ECDSA_TO_MBEDTLS_ERR(status) PSA_TO_MBEDTLS_ERR_LIST(status,   \
                                                                    psa_to_pk_ecdsa_errors,        \
                                                                    psa_pk_status_to_mbedtls)
#endif

#if defined(MBEDTLS_ECP_LIGHT)
/**
 * Public function mbedtls_pk_ec() can be used to get direct access to the
 * wrapped ecp_keypair structure pointed to the pk_ctx. However this is not
 * ideal because it bypasses the PK module on the control of its internal
 * structure (pk_context) fields.
 * For backward compatibility we keep mbedtls_pk_ec() when ECP_C is defined, but
 * we provide 2 very similar functions when only ECP_LIGHT is enabled and not
 * ECP_C.
 * These variants embed the "ro" or "rw" keywords in their name to make the
 * usage of the returned pointer explicit. Of course the returned value is
 * const or non-const accordingly.
 */
static inline const mbedtls_ecp_keypair *mbedtls_pk_ec_ro(const mbedtls_pk_context pk)
{
    switch (mbedtls_pk_get_type(&pk)) {
        case MBEDTLS_PK_ECKEY:
        case MBEDTLS_PK_ECKEY_DH:
        case MBEDTLS_PK_ECDSA:
            return (const mbedtls_ecp_keypair *) (pk).MBEDTLS_PRIVATE(pk_ctx);
        default:
            return NULL;
    }
}

static inline mbedtls_ecp_keypair *mbedtls_pk_ec_rw(const mbedtls_pk_context pk)
{
    switch (mbedtls_pk_get_type(&pk)) {
        case MBEDTLS_PK_ECKEY:
        case MBEDTLS_PK_ECKEY_DH:
        case MBEDTLS_PK_ECDSA:
            return (mbedtls_ecp_keypair *) (pk).MBEDTLS_PRIVATE(pk_ctx);
        default:
            return NULL;
    }
}

static inline mbedtls_ecp_group_id mbedtls_pk_get_group_id(const mbedtls_pk_context *pk)
{
    mbedtls_ecp_group_id id;
#if defined(MBEDTLS_PK_USE_PSA_EC_DATA)
    id = mbedtls_ecc_group_of_psa(pk->ec_family, pk->ec_bits, 0);
#else /* MBEDTLS_PK_USE_PSA_EC_DATA */
    id = mbedtls_pk_ec_ro(*pk)->grp.id;
#endif /* MBEDTLS_PK_USE_PSA_EC_DATA */
    return id;
}

/* Helper for Montgomery curves */
#if defined(MBEDTLS_ECP_DP_CURVE25519_ENABLED) || defined(MBEDTLS_ECP_DP_CURVE448_ENABLED)
#define MBEDTLS_PK_HAVE_RFC8410_CURVES
#endif /* MBEDTLS_ECP_DP_CURVE25519_ENABLED || MBEDTLS_ECP_DP_CURVE448_ENABLED */
#endif /* MBEDTLS_ECP_LIGHT */

#if defined(MBEDTLS_PK_USE_PSA_EC_DATA)
/**
 * \brief   Copy the public key content in raw format from "ctx->pk_ctx"
 *          (which is an ecp_keypair) into the internal "ctx->pub_raw" buffer.
 *
 * \note    This is a temporary function that can be removed as soon as the pk
 *          module is free from ECP_C
 *
 * \param pk   It is the pk_context which is going to be updated. It acts both
 *             as input and output.
 */
int mbedtls_pk_update_public_key_from_keypair(mbedtls_pk_context *pk,
                                              mbedtls_ecp_keypair *ecp_keypair);
#endif /* MBEDTLS_PK_USE_PSA_EC_DATA */

#endif /* MBEDTLS_PK_INTERNAL_H */
