.PHONY: all

ifndef PP

$(error the PP variable no defined!)

endif

BIN=./passKeeper
ACTIONS=encryptFile decryptFile
DN=&>/dev/null

print_suc=echo "$(1)-$(2)-$(3) done"

all: test_aes test_idea test_camellia

test_aes:
	cd $(PP) && \
	$(BIN) test AES 128 ECB crypter $(ACTIONS) $(DN) && \
		$(call print_suc,aes,128,ecb) && \
	$(BIN) test AES 192 ECB crypter $(ACTIONS) $(DN) && \
		$(call print_suc,aes,192,ecb) && \
	$(BIN) test AES 256 ECB crypter $(ACTIONS) $(DN) && \
		$(call print_suc,aes,256,ecb) && \
	$(BIN) test AES 128 CBC crypter $(ACTIONS) $(DN) && \
		$(call print_suc,aes,128,cbc) && \
	$(BIN) test AES 192 CBC crypter $(ACTIONS) $(DN) && \
		$(call print_suc,aes,192,cbc) && \
	$(BIN) test AES 256 CBC crypter $(ACTIONS) $(DN) && \
		$(call print_suc,aes,256,cbc) && \
	$(BIN) test AES 128 CFB crypter $(ACTIONS) $(DN) && \
		$(call print_suc,aes,128,cfb) && \
	$(BIN) test AES 192 CFB crypter $(ACTIONS) $(DN) && \
		$(call print_suc,aes,192,cfb) && \
	$(BIN) test AES 256 CFB crypter $(ACTIONS) $(DN) && \
		$(call print_suc,aes,256,cfb) && \
	$(BIN) test AES 128 OFB crypter $(ACTIONS) $(DN) && \
		$(call print_suc,aes,128,ofb) && \
	$(BIN) test AES 192 OFB crypter $(ACTIONS) $(DN) && \
		$(call print_suc,aes,192,ofb) && \
	$(BIN) test AES 256 OFB crypter $(ACTIONS) $(DN) && \
		$(call print_suc,aes,256,ofb)

test_idea:
	cd $(PP) && \
	$(BIN) test IDEA 128 ECB crypter $(ACTIONS) $(DN) && \
		$(call print_suc,idea,128,ecb) && \
	$(BIN) test IDEA 128 CBC crypter $(ACTIONS) $(DN) && \
		$(call print_suc,idea,128,cbc) && \
	$(BIN) test IDEA 128 CFB crypter $(ACTIONS) $(DN) && \
		$(call print_suc,idea,128,cfb) && \
	$(BIN) test IDEA 128 OFB crypter $(ACTIONS) $(DN) && \
		$(call print_suc,idea,128,ofb)

test_camellia:
	cd $(PP) && \
	$(BIN) test Camellia 128 ECB crypter $(ACTIONS) $(DN) && \
		$(call print_suc,camellia,128,ecb) && \
	$(BIN) test Camellia 256 ECB crypter $(ACTIONS) $(DN) && \
		$(call print_suc,camellia,256,ecb) && \
	$(BIN) test Camellia 128 CBC crypter $(ACTIONS) $(DN) && \
		$(call print_suc,camellia,128,cbc) && \
	$(BIN) test Camellia 256 CBC crypter $(ACTIONS) $(DN) && \
		$(call print_suc,camellia,256,cbc) && \
	$(BIN) test Camellia 128 CFB crypter $(ACTIONS) $(DN) && \
		$(call print_suc,camellia,128,cfb) && \
	$(BIN) test Camellia 256 CFB crypter $(ACTIONS) $(DN) && \
		$(call print_suc,camellia,256,cfb) && \
	$(BIN) test Camellia 128 OFB crypter $(ACTIONS) $(DN) && \
		$(call print_suc,camellia,128,ofb) && \
	$(BIN) test Camellia 256 OFB crypter $(ACTIONS) $(DN) && \
		$(call print_suc,camellia,256,ofb)

