
name = aes-ctr
type = sync
std = -ansi

srcdir = .
root := $(shell \
  cd "$(srcdir)"; root="$(srcdir)"; \
  while [ "`pwd`" != "/" ]; do \
    if [ -r "`pwd`/test/ecrypt-test.mk" ]; then  \
      echo $$root; exit; \
    fi; \
    cd ..; root="$$root"/..; \
  done; \
  echo ".")

include $(root)/test/ecrypt-test.mk

aes-ctr.s: crypto_stream_aes128estream_e_schwabe_ultrasparc3_1_ECRYPT_process_bytes.s aux.s
	cat $^ > aes-ctr.s
