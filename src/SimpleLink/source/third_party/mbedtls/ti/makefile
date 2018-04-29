SIMPLELINK_SDK_INSTALL_DIR ?= $(abspath ../../../../)

-include $(SIMPLELINK_SDK_INSTALL_DIR)/imports.mak

# Default POSIX is in the SDK/source directory
POSIX_ROOT = $(SIMPLELINK_SDK_INSTALL_DIR)/source

# Enable local overrides via a 'products.mak' file
-include ./products.mak

OBJS_CRYPTO=    aes.obj       aesni.obj     arc4.obj      \
        asn1parse.obj asn1write.obj base64.obj    \
        bignum.obj    blowfish.obj  camellia.obj  \
        ccm.obj       cipher.obj    cipher_wrap.obj   \
        cmac.obj      ctr_drbg.obj  des.obj       \
        dhm.obj       ecdh.obj      ecdsa.obj     \
        ecjpake.obj   ecp.obj               \
        ecp_curves.obj    entropy.obj   entropy_poll.obj  \
        error.obj     gcm.obj       havege.obj    \
        hmac_drbg.obj md.obj        md2.obj       \
        md4.obj       md5.obj       md_wrap.obj   \
        memory_buffer_alloc.obj       oid.obj       \
        padlock.obj   pem.obj       pk.obj        \
        pk_wrap.obj   pkcs12.obj    pkcs5.obj     \
        pkparse.obj   pkwrite.obj   platform.obj  \
        ripemd160.obj rsa.obj       sha1.obj      \
        sha256.obj    sha512.obj    threading.obj \
        timing.obj    version.obj           \
        version_features.obj      xtea.obj

OBJS_X509=  certs.obj     pkcs11.obj    x509.obj      \
        x509_create.obj   x509_crl.obj  x509_crt.obj  \
        x509_csr.obj  x509write_crt.obj x509write_csr.obj

OBJS_TLS=   debug.obj     net_sockets.obj       \
        ssl_cache.obj ssl_ciphersuites.obj  \
        ssl_cli.obj   ssl_cookie.obj        \
        ssl_srv.obj   ssl_ticket.obj        \
        ssl_tls.obj

OBJS_PORT= entropy_alt.obj threading_alt.obj

OBJS= $(OBJS_CRYPTO) \
        $(OBJS_X509) \
        $(OBJS_TLS) \
        $(OBJS_PORT)

INCS = -I../../../../include -I../../../../ti/port

POSIX_INCS_CCS = -I$(POSIX_ROOT)/ti/posix/ccs
POSIX_INCS_IAR = -I$(POSIX_ROOT)/ti/posix/iar
POSIX_INCS_GCC = -I$(POSIX_ROOT)/ti/posix/gcc

export CCS_ARMCOMPILER
export IAR_ARMCOMPILER
export GCC_ARMCOMPILER
export OBJS
export INCS
export POSIX_INCS_CCS
export POSIX_INCS_IAR
export POSIX_INCS_GCC

all:
	@ $(MAKE) -C lib/ccs/m4f
	@ $(MAKE) -C lib/iar/m4f
	@ $(MAKE) -C lib/gcc/m4f

clean:
	@ $(MAKE) -C lib/ccs/m4f clean
	@ $(MAKE) -C lib/iar/m4f clean
	@ $(MAKE) -C lib/gcc/m4f clean
