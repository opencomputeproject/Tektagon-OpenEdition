#include <zephyr.h>
#include <sys/printk.h>
#include <string.h>
#include <crypto/rsa_structs.h>
#include <crypto/rsa.h>
#include "rsa_aspeed.h"

int decrypt_aspeed(const struct rsa_key *key, const uint8_t *encrypted, size_t in_length, uint8_t *decrypted, size_t out_length)
{
	const struct device *dev = device_get_binding(RSA_DRV_NAME);
	struct rsa_ctx ini;
	struct rsa_pkt pkt;
	struct rsa_key *rk;
	int ret;
	int j;

	rk = key;
	pkt.in_buf = encrypted;
	pkt.in_len = in_length;
	pkt.out_buf = decrypted;
	pkt.out_buf_max = out_length;
	ret = rsa_begin_session(dev, &ini, rk);
	if (ret)
		printk("rsa_begin_session fail: %d", ret);

	rsa_decrypt(&ini, &pkt);
	rsa_free_session(dev, &ini);

	return ret;
}

/**
 * Verify that a signature matches the expected SHA-256 hash.  The signature is expected to be
 * in PKCS v1.5 format.
 *
 * @param key The public key to decrypt the signature.
 * @param signature The signature to validate.
 * @param sig_length The length of the signature.
 * @param match The value that should match the decrypted signature.
 * @param match_length The length of the match value.
 *
 * @return 0 if the signature matches the digest or an error code.
 */
int sig_verify_aspeed(const struct rsa_key *key, const uint8_t *signature, int sig_length, const uint8_t *match, size_t match_length)
{
	const struct device *dev = device_get_binding(RSA_DRV_NAME);
	struct rsa_ctx ini;
	struct rsa_pkt pkt;
	char plain_text[sig_length];
	int ret;

	pkt.in_buf = signature;
	pkt.in_len = sig_length;
	pkt.out_buf = plain_text;// match;
	pkt.out_buf_max = sig_length;
	memset(plain_text, 0, sig_length);
	ret = rsa_begin_session(dev, &ini, key);

	if (ret)
		printk("rsa_begin_session fail: %d", ret);

	rsa_verify(&ini, &pkt);// decrypt signature

	rsa_free_session(dev, &ini);

	ret = memcmp(plain_text + pkt.out_len - match_length, match, match_length);
	// if (ret != 0)
	// {
	//      printk("verify Fail:\n");
	//      printk("Result Text:");
	//      for(int i = 0; i < sig_length; i++){
	//              if(i % 16 == 0)
	//                      printk("\n");
	//              printk("%2x,", plain_text[i]);
	//      }
	//      printk("\n");
	//      printk("Signature:");
	//      for(int i = 0; i < sig_length; i++){
	//              if(i % 16 ==0)
	//                      printk("\n");
	//              printk("%2x,", *(signature+i));
	//      }
	//      printk("\n");

	//      printk("Hash:\n");
	//      for(int i = 0; i < match_length; i++){
	//              if(i % 16 == 0)
	//                      printk("\n");
	//              printk("%x,", *(match+i));
	//      }
	//      printk("\n");
	// }else{
	//      printk("Verification Successful \n");
	// }

	return ret;
}

#if ZEPHYR_RSA_API_MIDLEYER_TEST_SUPPORT

struct rsa_testvec {
	struct rsa_key k;
	char *p;
	char *c;
	unsigned int p_size;
	unsigned int c_size;
};

static struct rsa_testvec rsa_tv[] = {
	{
		.k = {
			.m = "\xcd\x6b\xb1\xa0\xdb\x68\xde\x42\x18\xcc\x86\x68\xed\x33\x44\xc7\x9b\x2c\xb1\xa9\x25\xe8\x7f\xfa\x58\x73\x34\xa4\xd2\x84\x72\x25\x6d\xab\x26\xda\x4a\xa0\xe1\xa6\xcd\x64\xb9\x5d\x91\xf9\x1a\x79\x3f\x3d\x76\xe8\xfa\x69\x1c\x8a\x4f\xd2\xc3\xe6\xbf\xe3\x6a\x4d\x56\xf0\x8a\xfc\x7a\x95\x29\xe8\xa4\xe2\x85\xe3\xa0\xf3\x6c\x90\x3c\x1b\x41\x7f\xb8\x6c\x49\xf9\x22\xfe\x37\xce\x81\xa8\x9c\x3d\xa9\xf1\xaa\x80\x94\xb1\xb8\x59\xd4\x74\xc3\x2e\x26\x32\xa3\xe3\x82\xf5\xf2\x59\xe8\xf9\xa8\x3e\x64\x5e\xd6\xef\xf9\x6d\xda\xb8\x17\x14\x7f\x71\x6a\xfc\xdc\x37\x37\xc9\x3a\x11\x89\xb2\xa3\x3c\x1a\x91\x8b\x09\x48\xa0\x38\x11\xf5\x08\x4b\x21\xb2\x6e\xb8\x25\xa9\x17\xa0\xf8\xa0\xf4\x40\xa3\x15\x87\xca\xaa\xf1\xe0\x64\xcf\x19\xf8\xad\xe1\x5b\x25\xe8\x36\x73\x42\x58\xa4\x7b\x48\xd6\xae\x8a\xfb\x75\x6e\xfa\x2a\x74\x3a\x55\x7e\x3b\xd8\x90\x5d\xbe\x04\xf3\x00\x4a\xa7\x6a\xc1\x03\x39\xb8\xfa\xfc\xba\x5a\x60\x12\x03\xf5\x47\xd1\x15\x6f\x85\x41\xa9\x78\x16\xde\x5e\x2e\xe6\x3e\xda\xdc\x72\x9b\xfd\xe0\x5a\x98\x90\xed\xf1\x0e\x1e\x57\x8e\x7a\xaf",
			.d = "0x00\xCD\x6B\xB1\xA0\xDB\x68\xDE\x42\x18\xCC\x86\x68\xED\x33\x44\xC7\x9B\x2C\xB1\xA9\x25\xE8\x7F\xFA\x58\x73\x34\xA4\xD2\x84\x72\x25\x6D\xAB\x26\xDA\x4A\xA0\xE1\xA6\xCD\x64\xB9\x5D\x91\xF9\x1A\x79\x3F\x3D\x76\xE8\xFA\x69\x1C\x8A\x4F\xD2\xC3\xE6\xBF\xE3\x6A\x4D\x56\xF0\x8A\xFC\x7A\x95\x29\xE8\xA4\xE2\x85\xE3\xA0\xF3\x6C\x90\x3C\x1B\x41\x7F\xB8\x6C\x49\xF9\x22\xFE\x37\xCE\x81\xA8\x9C\x3D\xA9\xF1\xAA\x80\x94\xB1\xB8\x59\xD4\x74\xC3\x2E\x26\x32\xA3\xE3\x82\xF5\xF2\x59\xE8\xF9\xA8\x3E\x64\x5E\xD6\xEF\xF9\x6D\xDA\xB8\x17\x14\x7F\x71\x6A\xFC\xDC\x37\x37\xC9\x3A\x11\x89\xB2\xA3\x3C\x1A\x91\x8B\x09\x48\xA0\x38\x11\xF5\x08\x4B\x21\xB2\x6E\xB8\x25\xA9\x17\xA0\xF8\xA0\xF4\x40\xA3\x15\x87\xCA\xAA\xF1\xE0\x64\xCF\x19\xF8\xAD\xE1\x5B\x25\xE8\x36\x73\x42\x58\xA4\x7B\x48\xD6\xAE\x8A\xFB\x75\x6E\xFA\x2A\x74\x3A\x55\x7E\x3B\xD8\x90\x5D\xBE\x04\xF3\x00\x4A\xA7\x6A\xC1\x03\x39\xB8\xFA\xFC\xBA\x5A\x60\x12\x03\xF5\x47\xD1\x15\x6F\x85\x41\xA9\x78\x16\xDE\x5E\x2E\xE6\x3E\xDA\xDC\x72\x9B\xFD\xE0\x5A\x98\x90\xED\xF1\x0E\x1E\x57\x8E\x7A\xAF",
			.e = "\x01\x00\x01",
			.m_bits = 2048,
			.d_bits = 2048,
			.e_bits = 24
		},
		.p = // Hash
		     "\x53\x2e\xaa\xbd\x95\x74\x88\x0d\xbf\x76\xb9\xb8\xcc\x00\x83\x2c"
		     "\x20\xa6\xec\x11\x3d\x68\x22\x99\x55\x0d\x7a\x6e\x0f\x34\x5e\x25",
		.c = // Sig
		     "\x7B\x5E\x36\x99\x75\xCE\x5E\x25\xEC\xAC\xA0\xFA\x3C\xFA\xA1\x3A\xFC\x94\x1F\xD4\xC7\xFC\xF7\x68\x96\xA2\x90\xAF\xDA\xDC\xF5\xD2\x7D\xD2\x0D\xC1\x3B\xB3\xD0\x7B\x43\x36\x37\x3C\x43\xF3\x39\xA6\x32\x06\x45\x63\x7C\x26\xE7\x06\x0C\x19\xF4\xB4\x01\xF6\x4E\x18\x61\x8A\x6E\xE5\x24\xEF\x2E\x35\x5C\x67\x3B\x18\xD2\xC9\xAA\x91\x59\xD2\x09\xAF\xFC\xB7\x5C\x0D\x48\xB2\xBE\x56\xA0\x45\xA1\x65\xFA\xCF\xFF\x15\xB1\x5C\x82\x1E\x05\x25\x6D\x3D\x10\x5A\x1E\xB0\x50\x00\x80\x5F\x4A\xFE\x93\xE3\x54\x0F\xCE\xE3\xB2\x48\xBC\xF4\x50\x5C\x4A\x3D\x8B\x5D\xA3\xFC\x23\x0A\xDD\xA1\x46\x8E\xE7\xD7\x8F\x0B\x77\x9A\x9C\x28\xF8\xA0\x5F\x57\x62\x4A\xAE\x23\xBF\xAD\xD5\xF6\xCF\x3B\xD6\x1B\xA6\xC5\xD6\xA4\x75\x26\x6E\xDF\xD4\x78\x9F\xC4\x74\x56\x9D\xD3\xB1\x81\xD1\x55\x08\x19\x87\xA2\x8F\x61\x31\x2D\xCC\x07\x30\xDC\x0C\xF7\x57\xC9\x99\x7B\xFB\x7F\x17\x3B\x2D\x33\x8A\xAE\x1B\x72\x9E\x15\x6F\x97\x37\xFA\xB8\x41\xD8\xA8\xB8\x4E\x78\x6E\x36\x5A\xD4\x08\xEC\xCA\x59\x52\x1A\xE8\x6B\xE5\x60\x6F\x01\x70\x13\xF2\xA4\xF1\x02\x63\xD1\x57\xEB\x48\x5B\xD0",
		.p_size = 32,
		.c_size = 256,
	},
};

static struct rsa_key rsa_pub_key[] = {
	{
		.m = "\xcd\x6b\xb1\xa0\xdb\x68\xde\x42\x18\xcc\x86\x68\xed\x33\x44\xc7\x9b\x2c\xb1\xa9\x25\xe8\x7f\xfa\x58\x73\x34\xa4\xd2\x84\x72\x25\x6d\xab\x26\xda\x4a\xa0\xe1\xa6\xcd\x64\xb9\x5d\x91\xf9\x1a\x79\x3f\x3d\x76\xe8\xfa\x69\x1c\x8a\x4f\xd2\xc3\xe6\xbf\xe3\x6a\x4d\x56\xf0\x8a\xfc\x7a\x95\x29\xe8\xa4\xe2\x85\xe3\xa0\xf3\x6c\x90\x3c\x1b\x41\x7f\xb8\x6c\x49\xf9\x22\xfe\x37\xce\x81\xa8\x9c\x3d\xa9\xf1\xaa\x80\x94\xb1\xb8\x59\xd4\x74\xc3\x2e\x26\x32\xa3\xe3\x82\xf5\xf2\x59\xe8\xf9\xa8\x3e\x64\x5e\xd6\xef\xf9\x6d\xda\xb8\x17\x14\x7f\x71\x6a\xfc\xdc\x37\x37\xc9\x3a\x11\x89\xb2\xa3\x3c\x1a\x91\x8b\x09\x48\xa0\x38\x11\xf5\x08\x4b\x21\xb2\x6e\xb8\x25\xa9\x17\xa0\xf8\xa0\xf4\x40\xa3\x15\x87\xca\xaa\xf1\xe0\x64\xcf\x19\xf8\xad\xe1\x5b\x25\xe8\x36\x73\x42\x58\xa4\x7b\x48\xd6\xae\x8a\xfb\x75\x6e\xfa\x2a\x74\x3a\x55\x7e\x3b\xd8\x90\x5d\xbe\x04\xf3\x00\x4a\xa7\x6a\xc1\x03\x39\xb8\xfa\xfc\xba\x5a\x60\x12\x03\xf5\x47\xd1\x15\x6f\x85\x41\xa9\x78\x16\xde\x5e\x2e\xe6\x3e\xda\xdc\x72\x9b\xfd\xe0\x5a\x98\x90\xed\xf1\x0e\x1e\x57\x8e\x7a\xaf",
		.m_bits = 2048,
		.d = "0x00\xCD\x6B\xB1\xA0\xDB\x68\xDE\x42\x18\xCC\x86\x68\xED\x33\x44\xC7\x9B\x2C\xB1\xA9\x25\xE8\x7F\xFA\x58\x73\x34\xA4\xD2\x84\x72\x25\x6D\xAB\x26\xDA\x4A\xA0\xE1\xA6\xCD\x64\xB9\x5D\x91\xF9\x1A\x79\x3F\x3D\x76\xE8\xFA\x69\x1C\x8A\x4F\xD2\xC3\xE6\xBF\xE3\x6A\x4D\x56\xF0\x8A\xFC\x7A\x95\x29\xE8\xA4\xE2\x85\xE3\xA0\xF3\x6C\x90\x3C\x1B\x41\x7F\xB8\x6C\x49\xF9\x22\xFE\x37\xCE\x81\xA8\x9C\x3D\xA9\xF1\xAA\x80\x94\xB1\xB8\x59\xD4\x74\xC3\x2E\x26\x32\xA3\xE3\x82\xF5\xF2\x59\xE8\xF9\xA8\x3E\x64\x5E\xD6\xEF\xF9\x6D\xDA\xB8\x17\x14\x7F\x71\x6A\xFC\xDC\x37\x37\xC9\x3A\x11\x89\xB2\xA3\x3C\x1A\x91\x8B\x09\x48\xA0\x38\x11\xF5\x08\x4B\x21\xB2\x6E\xB8\x25\xA9\x17\xA0\xF8\xA0\xF4\x40\xA3\x15\x87\xCA\xAA\xF1\xE0\x64\xCF\x19\xF8\xAD\xE1\x5B\x25\xE8\x36\x73\x42\x58\xA4\x7B\x48\xD6\xAE\x8A\xFB\x75\x6E\xFA\x2A\x74\x3A\x55\x7E\x3B\xD8\x90\x5D\xBE\x04\xF3\x00\x4A\xA7\x6A\xC1\x03\x39\xB8\xFA\xFC\xBA\x5A\x60\x12\x03\xF5\x47\xD1\x15\x6F\x85\x41\xA9\x78\x16\xDE\x5E\x2E\xE6\x3E\xDA\xDC\x72\x9B\xFD\xE0\x5A\x98\x90\xED\xF1\x0E\x1E\x57\x8E\x7A\xAF",
	},
};

/* The SHA256 hash for the string "Test". */
// const uint8_t SIG_HASH_TEST[] = {
//     0x53,0x2e,0xaa,0xbd,0x95,0x74,0x88,0x0d,0xbf,0x76,0xb9,0xb8,0xcc,0x00,0x83,0x2c,
//     0x20,0xa6,0xec,0x11,0x3d,0x68,0x22,0x99,0x55,0x0d,0x7a,0x6e,0x0f,0x34,0x5e,0x25
// };

int rsa_sig_verify_test(void)
{
	int ret;
	// const struct rsa_key *key;
	// const uint8_t *signature;
	// size_t sig_length;
	// const uint8_t *match;
	size_t match_length;
	int j;

	ret = sig_verify_aspeed(rsa_pub_key, rsa_tv[0].c, rsa_tv[0].c_size, rsa_tv[0].p, rsa_tv[0].p_size);

	// ret = memcmp(match, , rsa_tv[0].p_size);//rsa_tv[i].p = OTP hash data
	if (ret != 0) {
		printk("decrypt fail: %d\n", ret);
		// printk("input text:\n");
		// printk("c_size=%x\n", rsa_tv[0].c_size);
		// printk("p_size=%x\n", rsa_tv[0].p_size);
		// for (j = 0; j < rsa_tv[0].c_size; j++)
		// {
		//     printk("%x ", rsa_tv[0].c[j]);
		//     if ((j + 1) % 16 == 0)
		//         printk("\n");
		// }
		// printk("result text:\n");
		// for (j = 0; j < rsa_tv[0].p_size; j++)
		// {
		//     printk("%x ", match[j]);
		//     if ((j + 1) % 16 == 0)
		//         printk("\n");
		// }
		// printk( "\n");
	} else   {
		printk("decrypt pass\n");
	}

	return 0;
}
// mark_rsa_test+>

struct RsaTestData {
	struct rsa_key k;
	char *p;
	char *c;
	unsigned int p_size;
	unsigned int c_size;
};
static struct RsaTestData RsaTD[] = {
	{
		.k = {
			.m =
				"\xC3\x8B\x55\x7B\x73\x4D\xFF\xE9\x9B\xC6\xDC\x67\x3C\xB4\x8E"
				"\xA0\x86\xED\xF2\xB9\x50\x5C\x54\x5C\xBA\xE4\xA1\xB2\xA7\xAE\x2F"
				"\x1B\x7D\xF1\xFB\xAC\x79\xC5\xDF\x1A\x00\xC9\xB2\xC1\x61\x25\x33"
				"\xE6\x9C\xE9\xCF\xD6\x27\xC4\x4E\x44\x30\x44\x5E\x08\xA1\x87\x52"
				"\xCC\x6B\x97\x70\x8C\xBC\xA5\x06\x31\x0C\xD4\x2F\xD5\x7D\x26\x24"
				"\xA2\xE2\xAC\x78\xF4\x53\x14\xCE\xF7\x19\x2E\xD7\xF7\xE6\x0C\xB9"
				"\x56\x7F\x0B\xF1\xB1\xE2\x43\x70\xBD\x86\x1D\xA1\xCC\x2B\x19\x08"
				"\x76\xEF\x91\xAC\xBF\x20\x24\x0D\x38\xC0\x89\xB8\x9A\x70\xB3\x64"
				"\xD9\x8F\x80\x41\x10\x5B\x9F\xB1\xCB\x76\x43\x00\x21\x25\x36\xD4"
				"\x19\xFC\x55\x95\x10\xE4\x26\x74\x98\x2C\xD9\xBD\x0B\x2B\x04\xC2"
				"\xAC\x82\x38\xB4\xDD\x4C\x04\x7E\x51\x36\x40\x1E\x0B\xC4\x7C\x25"
				"\xDD\x4B\xB2\xE7\x20\x0A\x57\xF9\xB4\x94\xC3\x08\x33\x22\x6F\x8B"
				"\x48\xDB\x03\x68\x5A\x5B\xBA\xAE\xF3\xAD\xCF\xC3\x6D\xBA\xF1\x28"
				"\x67\x7E\x6C\x79\x07\xDE\xFC\xED\xE7\x96\xE3\x6C\xE0\x2C\x87\xF8"
				"\x02\x01\x28\x38\x43\x21\x53\x84\x69\x75\x78\x15\x7E\xEE\xD2\x1B"
				"\xB9\x23\x40\xA8\x86\x1E\x38\x83\xB2\x73\x1D\x53\xFB\x9E\x2A\x8A"
				"\xB2\x75\x35\x01\xC3\xC3\xC4\x94\xE8\x84\x86\x64\x81\xF4\x42\xAA"
				"\x3C\x0E\xD6\x4F\xBC\x0A\x09\x2D\xE7\x1B\xD4\x10\xA8\x54\xEA\x89"
				"\x84\x8A\xCB\xF7\x5A\x3C\xCA\x76\x08\x29\x62\xB4\x6A\x22\xDF\x14"
				"\x95\x71\xFD\xB6\x86\x39\xB8\x8B\xF8\x91\x7F\x38\xAA\x14\xCD\xE5"
				"\xF5\x1D\xC2\x6D\x53\x69\x52\x84\x7F\xA3\x1A\x5E\x26\x04\x83\x06"
				"\x73\x52\x56\xCF\x76\x26\xC9\xDD\x75\xD7\xFC\xF4\x69\xD8\x7B\x55"
				"\xB7\x68\x13\x53\xB9\xE7\x89\xC3\xE8\xD6\x6E\xA7\x6D\xEA\x81\xFD"
				"\xC4\xB7\x05\x5A\xB7\x41\x0A\x23\x8E\x03\x8A\x1C\xAE\xD3\x1E\xCE"
				"\xE3\x5E\xFC\x19\x4A\xEE\x61\x9B\x8E\xE5\xE5\xDD\x85\xF9\x41\xEC"
				"\x14\x53\x92\xF7\xDD\x06\x85\x02\x91\xE3\xEB\x6C\x43\x03\xB1\x36"
				"\x7B\x89\x5A\xA8\xEB\xFC\xD5\xA8\x35\xDC\x81\xD9\x5C\xBD\xCA\xDC"
				"\x9B\x98\x0B\x06\x5D\x0C\x5B\xEE\xF3\xD5\xCC\x57\xC9\x71\x2F\x90"
				"\x3B\x3C\xF0\x8E\x4E\x35\x48\xAE\x63\x74\xA9\xFC\x72\x75\x8E\x34"
				"\xA8\xF2\x1F\xEA\xDF\x3A\x37\x2D\xE5\x39\x39\xF8\x57\x58\x3C\x04"
				"\xFE\x87\x06\x98\xBC\x7B\xD3\x21\x36\x60\x25\x54\xA7\x3D\xFA\x91"
				"\xCC\xA8\x0B\x92\x8E\xB4\xF7\x06\xFF\x1E\x95\xCB\x07\x76\x97\x3B"
				"\x9D",
			.d =
				"\x74\xA9\xE0\x6A\x32\xB4\xCA\x85\xD9\x86\x9F\x60\x88\x7B\x40\xCC"
				"\xCD\x33\x91\xA8\xB6\x25\x1F\xBF\xE3\x51\x1C\x97\xB6\x2A\xD9\xB8"
				"\x11\x40\x19\xE3\x21\x13\xC8\xB3\x7E\xDC\xD7\x65\x40\x4C\x2D\xD6"
				"\xDC\xAF\x32\x6C\x96\x75\x2C\x2C\xCA\x8F\x3F\x7A\xEE\xC4\x09\xC6"
				"\x24\x3A\xC9\xCF\x6D\x8D\x17\x50\x94\x52\xD3\xE7\x0F\x2F\x7E\x94"
				"\x1F\xA0\xBE\xD9\x25\xE8\x38\x42\x7C\x27\xD2\x79\xF8\x2A\x87\x38"
				"\xEF\xBB\x74\x8B\xA8\x6E\x8C\x08\xC6\xC7\x4F\x0C\xBC\x79\xC6\xEF"
				"\x0E\xA7\x5E\xE4\xF8\x8C\x09\xC7\x5E\x37\xCC\x87\x77\xCD\xCF\xD1"
				"\x6D\x28\x1B\xA9\x62\xC0\xB8\x16\xA7\x8B\xF9\xBB\xCC\xB4\x15\x7F"
				"\x1B\x69\x03\xF2\x7B\xEB\xE5\x8C\x14\xD6\x23\x4F\x52\x6F\x18\xA6"
				"\x4B\x5B\x01\xAD\x35\xF9\x48\x53\xB3\x86\x35\x66\xD7\xE7\x29\xC0"
				"\x09\xB5\xC6\xE6\xFA\xC4\xDA\x19\xBE\xD7\x4D\x41\x14\xBE\x6F\xDF"
				"\x1B\xAB\xC0\xCA\x88\x07\xAC\xF1\x7D\x35\x83\x67\x28\x2D\x50\xE9"
				"\xCE\x27\x71\x5E\x1C\xCF\xD2\x30\x65\x79\x72\x2F\x9C\xE1\xD2\x39"
				"\x7F\xEF\x3B\x01\xF2\x14\x1D\xDF\xBD\x51\xD3\xA1\x53\x62\xCF\x5F"
				"\x79\x84\xCE\x06\x96\x69\x29\x49\x82\x1C\x71\x4A\xA1\x66\xC8\x2F"
				"\xFD\x7B\x96\x7B\xFC\xC4\x26\x58\xC4\xFC\x7C\xAF\xB5\xE8\x95\x83"
				"\x87\xCB\x46\xDE\x97\xA7\xB3\xA2\x54\x5B\xD7\xAF\xAB\xEB\xC8\xF3"
				"\x55\x9D\x48\x2B\x30\x9C\xDC\x26\x4B\xC2\x89\x45\x13\xB2\x01\x9A"
				"\xA4\x65\xC3\xEC\x24\x2D\x26\x97\xEB\x80\x8A\x9D\x03\xBC\x59\x66"
				"\x9E\xE2\xBB\xBB\x63\x19\x64\x93\x11\x7B\x25\x65\x30\xCD\x5B\x4B"
				"\x2C\xFF\xDC\x2D\x30\x87\x1F\x3C\x88\x07\xD0\xFC\x48\xCC\x05\x8A"
				"\xA2\xC8\x39\x3E\xD5\x51\xBC\x0A\xBE\x6D\xA8\xA0\xF6\x88\x06\x79"
				"\x13\xFF\x1B\x45\xDA\x54\xC9\x24\x25\x8A\x75\x0A\x26\xD1\x69\x81"
				"\x14\x14\xD1\x79\x7D\x8E\x76\xF2\xE0\xEB\xDD\x0F\xDE\xC2\xEC\x80"
				"\xD7\xDC\x16\x99\x92\xBE\xCB\x40\x0C\xCE\x7C\x3B\x46\xA2\x5B\x5D"
				"\x0C\x45\xEB\xE1\x00\xDE\x72\x50\xB1\xA6\x0B\x76\xC5\x8D\xFC\x82"
				"\x38\x6D\x99\x14\x1D\x1A\x4A\xD3\x7C\x53\xB8\x12\x46\xA2\x30\x38"
				"\x82\xF4\x96\x6E\x8C\xCE\x47\x0D\xAF\x0A\x3B\x45\xB7\x43\x95\x43"
				"\x9E\x02\x2C\x44\x07\x6D\x1F\x3C\x66\x89\x09\xB6\x1F\x06\x30\xCC"
				"\xAD\xCE\x7D\x9A\xDE\x3E\xFB\x6C\xE4\x58\x43\xD2\x4F\xA5\x9E\x5E"
				"\xA7\x7B\xAE\x3A\xF6\x7E\xD9\xDB\xD3\xF5\xC5\x41\xAF\xE6\x9C\x91",
			.e = "\x01\x00\x01",
			.m_bits = 4096,
			.d_bits = 4096,
			.e_bits = 24
		},
		.p = "\x54\x85\x9b\x34\x2c\x49\xea\x2a",
		.c =
			"\x5c\xce\x9c\xd7\x9a\x9e\xa1\xfe\x7a\x82\x3c\x68\x27\x98\xe3\x5d"
			"\xd5\xd7\x07\x29\xf5\xfb\xc3\x1a\x7f\x63\x1e\x62\x31\x3b\x19\x87"
			"\x79\x4f\xec\x7b\xf3\xcb\xea\x9b\x95\x52\x3a\x40\xe5\x87\x7b\x72"
			"\xd1\x72\xc9\xfb\x54\x63\xd8\xc9\xd7\x2c\xfc\x7b\xc3\x14\x1e\xbc"
			"\x18\xb4\x34\xa1\xbf\x14\xb1\x37\x31\x6e\xf0\x1b\x35\x19\x54\x07"
			"\xf7\x99\xec\x3e\x63\xe2\xcd\x61\x28\x65\xc3\xcd\xb1\x38\x36\xa5"
			"\xb2\xd7\xb0\xdc\x1f\xf5\xef\x19\xc7\x53\x32\x2d\x1c\x26\xda\xe4"
			"\x0d\xd6\x90\x7e\x28\xd8\xdc\xe4\x61\x05\xd2\x25\x90\x01\xd3\x96"
			"\x6d\xa6\xcf\x58\x20\xbb\x03\xf4\x01\xbc\x79\xb9\x18\xd8\xb8\xba"
			"\xbd\x93\xfc\xf2\x62\x5d\x8c\x66\x1e\x0e\x84\x59\x93\xdd\xe2\x93"
			"\xa2\x62\x7d\x08\x82\x7a\xdd\xfc\xb8\xbc\xc5\x4f\x9c\x4e\xbf\xb4"
			"\xfc\xf4\xc5\x01\xe8\x00\x70\x4d\x28\x26\xcc\x2e\xfe\x0e\x58\x41"
			"\x8b\xec\xaf\x7c\x4b\x54\xd0\xa0\x64\xf9\x32\xf4\x2e\x47\x65\x0a"
			"\x67\x88\x39\x3a\xdb\xb2\xdb\x7b\xb5\xf6\x17\xa8\xd9\xc6\x5e\x28"
			"\x13\x82\x8a\x99\xdb\x60\x08\xa5\x23\x37\xfa\x88\x90\x31\xc8\x9d"
			"\x8f\xec\xfb\x85\x9f\xb1\xce\xa6\x24\x50\x46\x44\x47\xcb\x65\xd1"
			"\xdf\xc0\xb1\x6c\x90\x1f\x99\x8e\x4d\xd5\x9e\x31\x07\x66\x87\xdf"
			"\x01\xaa\x56\x3c\x71\xe0\x2b\x6f\x67\x3b\x23\xed\xc2\xbd\x03\x30"
			"\x79\x76\x02\x10\x10\x98\x85\x8a\xff\xfd\x0b\xda\xa5\xd9\x32\x48"
			"\x02\xa0\x0b\xb9\x2a\x8a\x18\xca\xc6\x8f\x3f\xbb\x16\xb2\xaa\x98"
			"\x27\xe3\x60\x43\xed\x15\x70\xd4\x57\x15\xfe\x19\xd4\x9b\x13\x78"
			"\x8a\xf7\x21\xf1\xa2\xa2\x2d\xb3\x09\xcf\x44\x91\x6e\x08\x3a\x30"
			"\x81\x3e\x90\x93\x8a\x67\x33\x00\x59\x54\x9a\x25\xd3\x49\x8e\x9f"
			"\xc1\x4b\xe5\x86\xf3\x50\x4c\xbc\xc5\xd3\xf5\x3a\x54\xe1\x36\x3f"
			"\xe2\x5a\xb4\x37\xc0\xeb\x70\x35\xec\xf6\xb7\xe8\x44\x3b\x7b\xf3"
			"\xf1\xf2\x1e\xdb\x60\x7d\xd5\xbe\xf0\x71\x34\x90\x4c\xcb\xd4\x35"
			"\x51\xc7\xdd\xd8\xc9\x81\xf5\x5d\x57\x46\x2c\xb1\x7b\x9b\xaa\xcb"
			"\xd1\x22\x25\x49\x44\xa3\xd4\x6b\x29\x7b\xd8\xb2\x07\x93\xbf\x3d"
			"\x52\x49\x84\x79\xef\xb8\xe5\xc4\xad\xca\xa8\xc6\xf6\xa6\x76\x70"
			"\x5b\x0b\xe5\x83\xc6\x0e\xef\x55\xf2\xe7\xff\x04\xea\xe6\x13\xbe"
			"\x40\xe1\x40\x45\x48\x66\x75\x31\xae\x35\x64\x91\x11\x6f\xda\xee"
			"\x26\x86\x45\x6f\x0b\xd5\x9f\x03\xb1\x65\x5b\xdb\xa4\xe4\xf9\x45",
		.p_size = 8,
		.c_size = 512,
	},
};

static int RsaDecryptTest(void)
{
	const struct device *dev = device_get_binding(RSA_DRV_NAME);
	struct rsa_ctx ini;
	struct rsa_pkt pkt;
	struct rsa_key *rk;
	char plain_text[512];
	char cipher_text[512];
	int j;
	int ret;

	printk("RsaDecryptTest start:\n");
	rk = &RsaTD[0].k;
	pkt.in_buf = RsaTD[0].p;
	pkt.in_len = RsaTD[0].p_size;
	pkt.out_buf = cipher_text;
	pkt.out_buf_max = 512;
	ret = rsa_begin_session(dev, &ini, rk);
	if (ret)
		printk("rsa_begin_session fail: %d", ret);

	rsa_encrypt(&ini, &pkt);
	rsa_free_session(dev, &ini);
	ret = memcmp(cipher_text, RsaTD[0].c, RsaTD[0].c_size);
	if (ret != 0) {
		printk("enc fail: %d\n", ret);
		printk("input text:\n");
		for (j = 0; j < RsaTD[0].p_size; j++) {
			printk("%02x", RsaTD[0].p[j]);
			if ((j + 1) % 32 == 0)
				printk("\n");
		}
		printk("\n");
		printk("result text:\n");
		for (j = 0; j < RsaTD[0].c_size; j++) {
			printk("%02x", cipher_text[j]);
			if ((j + 1) % 32 == 0)
				printk("\n");
		}
		printk("\n");
	} else   {
		printk("RsaDecryptTest enc pass\n");
	}

	decrypt_aspeed(&RsaTD[0].k, &cipher_text, RsaTD[0].c_size, &plain_text, 512);
	ret = memcmp(plain_text, RsaTD[0].p, RsaTD[0].p_size);
	if (ret != 0) {
		printk("dec fail: %d\n", ret);
		printk("input text:\n");
		for (j = 0; j < RsaTD[0].c_size; j++) {
			printk("%02x", RsaTD[0].p[j]);
			if ((j + 1) % 32 == 0)
				printk("\n");
		}
		printk("result text:\n");
		for (j = 0; j < RsaTD[0].p_size; j++) {
			printk("%02x", plain_text[j]);
			if ((j + 1) % 32 == 0)
				printk("\n");
		}
		printk("\n");
	} else {
		printk("MarkRsaDec pass\n");
	}
	return ret;
}
// <+mark_rsa_test

/**
 * @brief	test rsa functions
 */
void rsa_engine_function_test(void)
{
	uint8_t *status_string;

	status_string = (RsaDecryptTest()) ? ("FAIL") : ("PASS");

	printk("RsaDecryptTest : %s\n", status_string);

	status_string = (rsa_sig_verify_test()) ? ("FAIL") : ("PASS");

	printk("rsa_sig_verify_test : %s\n", status_string);
}

#endif  /* #if ZEPHYR_RSA_API_MIDLEYER_TEST_SUPPORT */
