#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#pragma once
// 基于 Crypto++ 的加解密封装 ai 封装

#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>
#include <cryptopp/filters.h>
#include <cryptopp/sha.h>
#include <cryptopp/md5.h>
#include <cryptopp/hmac.h>
#include <cryptopp/pwdbased.h>
#include <cryptopp/gcm.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/twofish.h>
#include <cryptopp/secblock.h>
#include <cryptopp/base64.h>
#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>
#include <cryptopp/pssr.h>
#include <cryptopp/files.h>
#include <cryptopp/elgamal.h>
#include <cryptopp/nbtheory.h>
#include <cryptopp/integer.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/filters.h>

#include <string>
#include <vector>
#include <stdexcept>
#include <memory>
#include <sstream>
#include <utility>
#include <cstring>

namespace encryption
{

    using crypto_byte = unsigned char;

    /**
   * @brief 简短说明: 随机数生成器封装，使用 RAII 管理 Crypto++ aeon_random
   * @details RAII 保证 AutoSeededRandomPool 的生命周期与对象一致，方便线程局部或对象池化。
   */
    class aeon_random
    {
    public:
        aeon_random() = default;
        CryptoPP::AutoSeededRandomPool &pool() { return _rng; }

    private:
        CryptoPP::AutoSeededRandomPool _rng;
    };

    static inline std::string ToHex(const crypto_byte *data, size_t len)
    {
        std::string out;
        CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(out), false);
        encoder.Put(data, len);
        encoder.MessageEnd();
        return out;
    }
    static inline std::string FromHexToRaw(const std::string &hex)
    {
        std::string out;
        CryptoPP::StringSource ss(hex, true, new CryptoPP::HexDecoder(new CryptoPP::StringSink(out)));
        return out;
    }
    static inline std::string ToBase64(const std::string &raw)
    {
        std::string out;
        CryptoPP::StringSource ss(raw, true,new CryptoPP::Base64Encoder(new CryptoPP::StringSink(out), false));
        return out;
    }
    static inline std::string FromBase64(const std::string &b64)
    {
        std::string out;
        CryptoPP::StringSource ss(b64, true,new CryptoPP::Base64Decoder(new CryptoPP::StringSink(out)));
        return out;
    }

    /**
   * @brief 从密码和 salt 派生密钥（PBKDF2-HMAC-SHA256）
   * @param pass 密码字符串
   * @param salt 指向 salt 的字节
   * @param saltLen salt 长度
   * @param keyLen 需要派生的密钥长度
   * @param iterations 迭代次数，默认 100000
   * @return 派生后的密钥（binary 字符串）
   */
    static inline std::string DeriveKeyFromPassword(const std::string &pass, const crypto_byte *salt, size_t saltLen, size_t keyLen, unsigned iterations = 10000)
    {
        std::string key;
        key.resize(keyLen);
        CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA256> pbkdf;
        pbkdf.DeriveKey((crypto_byte *)&key[0], keyLen, 0, (const crypto_byte *)pass.data(), pass.size(), salt, saltLen, iterations);
        return key;
    }

    // 包格式说明（简要）：
    // 二进制包 = "CW" + 1byte alg_id + salt(16) + 1byte iv_len + iv + 1byte tag_len/或 mac_len(当需要) + mac/tag + ciphertext

    enum binary : uint8_t
    {
        ALG_AES_GCM = 1,
        ALG_AES_CBC = 2,
        ALG_AES_CTR = 3,
        ALG_TWOFISH_CTR = 4
    };

    /**
   * @brief 对称加密帮助类，封装常用对称密码模式（AES-GCM, AES-CBC+HMAC, AES-CTR+HMAC, Twofish-CTR+HMAC）。
   * @details
   * - 接口设计：上层调用仅需两个 std::string（明文/密文 base64 与密码字符串）。
   * - 安全实践：对 CBC/CTR/Twofish-CTR 等非认证模式增加 HMAC-SHA256 作为完整性保护；GCM 使用内建 tag。
   * - 性能提示：高并发场景建议为每线程缓存 aeon_random 与加密上下文以减少构造成本。
   */
    class arcane_symmetric
    {
    public:
        /**
     * @brief AES-GCM 加密（Authenticated Encryption）
     * @param plaintext 明文
     * @param pass 用于派生对称密钥的密码（passphrase）
     * @return base64 编码的封装密文（包含 salt, iv, tag, ciphertext）
     * @note 性质：对称、认证加密（AEAD）。
     * @note 安全性：使用 AES-256-GCM，密钥从 PBKDF2(HMAC-SHA256) 派生，salt 16 字节，IV 12 字节，tag 16 字节；推荐使用随机密钥或 KMS 替代长密码。
     */
        static std::string AESGCM_Encrypt(const std::string &plaintext, const std::string &pass);

        /**
     * @brief AES-GCM 解密（Authenticated Decryption）
     * @param cipher_b64 AESGCM_Encrypt 返回的 base64 字符串
     * @param pass 与加密时相同的密码（passphrase）
     * @return 解密得到的明文
     * @throws std::runtime_error 解密失败或认证失败
     */
        static std::string AESGCM_Decrypt(const std::string &cipher_b64, const std::string &pass);

        /**
     * @brief AES-CBC 加密（带 HMAC-SHA256 做完整性）
     * @param plaintext 明文
     * @param pass 密码用于派生密钥
     * @return base64 封装密文（包含 salt, iv, mac, ciphertext）
     * @note 性质：对称、非认证模式 + 外部 HMAC 提供认证
     * @note 安全性：CBC 本身易受 padding oracle，故增加 HMAC；推荐首选 AES-GCM 或 ChaCha20-Poly1305。
     */
        static std::string AESCBC_Encrypt(const std::string &plaintext, const std::string &pass);

        /**
     * @brief AES-CBC 解密（并验证 HMAC）
     * @param b64 AESCBC_Encrypt 生成的 base64
     * @param pass 密码
     * @return 明文
     */
        static std::string AESCBC_Decrypt(const std::string &b64, const std::string &pass);

        /**
     * @brief AES-CTR 加密（流式）+ HMAC-SHA256
     * @note 性质：对称、流密码模式（无填充）+ HMAC 认证
     * @note 安全性：CTR 无认证，所以必须配合 HMAC 或使用 AEAD 模式；若平台支持 AES-NI，CTR 性能优良。
     */
        static std::string AESCTR_Encrypt(const std::string &plaintext, const std::string &pass);

        /**
     * @brief AES-CTR 解密（并验证 HMAC）
     */
        static std::string AESCTR_Decrypt(const std::string &b64, const std::string &pass);

        /**
     * @brief Twofish-CTR 加密（替代算法）+ HMAC-SHA256
     * @note 性质：对称、Twofish 流模式 + HMAC 认证
     * @note 安全性：Twofish 是成熟块加密算法，密钥长度与使用方式决定强度；兼容性不如 AES，但作为替代可用。
     */
        static std::string TwofishCTR_Encrypt(const std::string &plaintext, const std::string &pass);

        /**
     * @brief Twofish-CTR 解密（并验证 HMAC）
     */
        static std::string TwofishCTR_Decrypt(const std::string &b64, const std::string &pass);
    };

    /**
   * @brief 非对称算法封装（RSA, ElGamal）
   * @details
   * - RSA: 提供 RSA-OAEP（加密/解密）与 RSA-PSS（签名/验签）。
   * - ElGamal: 提供作为离散对数 (DL) 的替代实现。
   * - 密钥导出采用 base64(DER) 文本形式方便传输。
   */
    class penumbra_asymmetric
    {
    public:
        /**
     * @brief 生成 RSA 密钥对
     * @param bits 密钥长度（推荐 >= 2048）
     * @return pair<private_base64_der, public_base64_der>
     * @note 性质：非对称（RSA）
     * @note 安全性：使用 RSA-OAEP 推荐用于加密；推荐 2048 或 3072 位；对签名使用 RSA-PSS。
     */
        static std::pair<std::string, std::string> GenerateRSAKeypair(unsigned int bits = 2048);

        /**
     * @brief RSA-OAEP 加密（使用公钥）
     * @param plaintext 明文
     * @param pubDerBase64 公钥（base64(DER)）
     * @return base64 编码的密文
     */
        static std::string RSA_OAEP_Encrypt(const std::string &plaintext, const std::string &pubDerBase64);

        /**
     * @brief RSA-OAEP 解密（使用私钥）
     * @param cipherB64 base64 密文
     * @param privDerBase64 私钥（base64(DER)）
     * @return 明文
     */
        static std::string RSA_OAEP_Decrypt(const std::string &cipherB64, const std::string &privDerBase64);

        /**
     * @brief RSA-PSS 签名（SHA256）
     * @param msg 待签名消息
     * @param privDerBase64 私钥（base64(DER)）
     * @return base64 编码的签名
     * @note 性质：非对称签名（RSA-PSS）
     * @note 安全性：PSS 模式及 SHA256 哈希推荐用于现代签名，避免使用旧的 PKCS#1 v1.5 签名。
     */
        static std::string RSA_PSS_Sign(const std::string &msg, const std::string &privDerBase64);

        /**
     * @brief RSA-PSS 验签（SHA256）
     * @param msg 原始消息
     * @param sigB64 base64 签名
     * @param pubDerBase64 公钥（base64(DER)）
     * @return 验证结果，true 表示签名有效
     */
        static bool RSA_PSS_Verify(const std::string &msg, const std::string &sigB64, const std::string &pubDerBase64);

        /**
     * @brief 生成 ElGamal 密钥对（DL 基础）
     * @param pbits 素数位数（推荐 >= 2048）
     * @return pair<private_base64_der, public_base64_der>
     * @note 性质：非对称（ElGamal）
     * @note 安全性：基于离散对数问题，参数长度需足够大以防止 DL 攻击；一般建议使用现代 ECC 替代（如 Curve25519）。
     */
        static std::pair<std::string, std::string> GenerateElGamalKeypair(unsigned int pbits = 2048);

        /**
     * @brief ElGamal 加密（使用公钥）
     * @param plaintext 明文
     * @param pubDerBase64 公钥（base64(DER)）
     * @return base64 密文
     */
        static std::string ElGamal_Encrypt(const std::string &plaintext, const std::string &pubDerBase64);

        /**
     * @brief ElGamal 解密（使用私钥）
     * @param cipherB64 base64 密文
     * @param privDerBase64 私钥（base64(DER)）
     * @return 明文
     */
        static std::string ElGamal_Decrypt(const std::string &cipherB64, const std::string &privDerBase64);
    };

    // ---------- 哈希辅助 ----------
    /**
   * @brief 提供常用哈希函数（`MD5`, `SHA256`）。仅作为辅助工具。
   * @note `MD5` 被认为对抗碰撞攻击不再安全，不应用于安全签名场景，仅用于兼容或校验场合。
   */
    class umbrage_hash
    {
    public:
        /**
     * @brief 计算 MD5 摘要并返回 hex 字符串
     * @param data 输入数据
     * @return 32 字节 `hex` 字符串
     * @note 性质：散列（非加密）
     * @note 安全性：`MD5` 不再推荐用于安全目的（碰撞弱），仅用于兼容性或非安全校验。
     */
        inline static std::string MD5(const std::string &data);

        /**
     * @brief 计算 SHA256 摘要并返回 hex 字符串
     * @param data 输入数据
     * @return 64 字节 `hex` 字符串
     * @note 性质：散列（非加密）
     * @note 安全性：`SHA256` 目前被广泛接受为安全散列函数。
     */
        inline static std::string SHA256(const std::string &data);
    };

    // 实现: arcane_symmetric
    std::string arcane_symmetric::AESGCM_Encrypt(const std::string &plaintext, const std::string &pass)
    {
        aeon_random rng;
        const size_t SALT_LEN = 16;
        const size_t IV_LEN = 12;
        const size_t KEY_LEN = 32; // AES-256-GCM

        CryptoPP::SecByteBlock salt(SALT_LEN);
        rng.pool().GenerateBlock(salt, SALT_LEN);
        std::string key = DeriveKeyFromPassword(pass, salt.begin(), SALT_LEN, KEY_LEN);

        CryptoPP::SecByteBlock iv(IV_LEN);
        rng.pool().GenerateBlock(iv, IV_LEN);

        std::string cipherAndTag;
        try
        {
            CryptoPP::GCM<CryptoPP::AES>::Encryption enc;
            enc.SetKeyWithIV((const crypto_byte *)key.data(), key.size(), iv, iv.size());

            CryptoPP::StringSource ss(plaintext, true,new CryptoPP::AuthenticatedEncryptionFilter(enc,
            new CryptoPP::StringSink(cipherAndTag),false,16));
        }
        catch (const CryptoPP::Exception &e)
        {
            throw std::runtime_error(std::string("AESGCM encrypt error: ") + e.what());
        }

        std::string bin;
        bin.reserve(2 + 1 + SALT_LEN + 1 + IV_LEN + 1 + 16 + cipherAndTag.size());
        bin.append("CW", 2);
        bin.push_back((char)ALG_AES_GCM);
        bin.append((const char *)salt.BytePtr(), SALT_LEN);
        bin.push_back((char)IV_LEN);
        bin.append((const char *)iv.BytePtr(), IV_LEN);
        bin.push_back((char)16);
        bin.append(cipherAndTag);
        return ToBase64(bin);
    }

    std::string arcane_symmetric::AESGCM_Decrypt(const std::string &cipher_b64, const std::string &pass)
    {
        std::string bin = FromBase64(cipher_b64);
        if (bin.size() < 2 + 1 + 16 + 1 + 12 + 1 + 16)
            throw std::runtime_error("Invalid ciphertext (too short)");
        size_t pos = 0;
        if (bin[0] != 'C' || bin[1] != 'W')
            throw std::runtime_error("Invalid format");
        pos = 2;
        uint8_t alg = (uint8_t)bin[pos++];
        if (alg != ALG_AES_GCM)
            throw std::runtime_error("Algorithm mismatch (expect AES-GCM)");
        const size_t SALT_LEN = 16;
        const size_t saltPos = pos;
        pos += SALT_LEN;
        uint8_t ivLen = (uint8_t)bin[pos++];
        const size_t ivPos = pos;
        pos += ivLen;
        uint8_t tagLen = (uint8_t)bin[pos++];
        std::string cipherAndTag = bin.substr(pos);

        std::string key = DeriveKeyFromPassword(pass, (const crypto_byte *)bin.data() + saltPos, SALT_LEN, 32);

        try
        {
            CryptoPP::GCM<CryptoPP::AES>::Decryption dec;
            dec.SetKeyWithIV((const crypto_byte *)key.data(), key.size(), (const crypto_byte *)(bin.data() + ivPos), ivLen);

            std::string recovered;
            CryptoPP::AuthenticatedDecryptionFilter df(dec,new CryptoPP::StringSink(recovered),
            CryptoPP::AuthenticatedDecryptionFilter::THROW_EXCEPTION,tagLen);
            CryptoPP::StringSource ss(cipherAndTag, true, new CryptoPP::Redirector(df));
            return recovered;
        }
        catch (const CryptoPP::Exception &e)
        {
            throw std::runtime_error(std::string("AESGCM decrypt error: ") + e.what());
        }
    }

    std::string arcane_symmetric::AESCBC_Encrypt(const std::string &plaintext, const std::string &pass)
    {
        aeon_random rng;
        const size_t SALT_LEN = 16;
        const size_t IV_LEN = 16;
        const size_t KEY_LEN = 32; // AES-256
        CryptoPP::SecByteBlock salt(SALT_LEN);
        rng.pool().GenerateBlock(salt, SALT_LEN);
        std::string key = DeriveKeyFromPassword(pass, salt.BytePtr(), SALT_LEN, KEY_LEN);
        CryptoPP::SecByteBlock iv(IV_LEN);
        rng.pool().GenerateBlock(iv, IV_LEN);

        std::string ciphertext;
        try
        {
            CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption enc((const crypto_byte *)key.data(), key.size(), iv);
            CryptoPP::StringSource ss(plaintext, true,new CryptoPP::StreamTransformationFilter(enc,
            new CryptoPP::StringSink(ciphertext),CryptoPP::BlockPaddingSchemeDef::PKCS_PADDING));
        }
        catch (const CryptoPP::Exception &e)
        {
            throw std::runtime_error(std::string("AESCBC encrypt error: ") + e.what());
        }

        std::string macKey = DeriveKeyFromPassword(pass, salt.BytePtr(), SALT_LEN, 32, 200000);
        std::string mac;
        try
        {
            CryptoPP::HMAC<CryptoPP::SHA256> hmac((const crypto_byte *)macKey.data(), macKey.size());
            CryptoPP::StringSource ss(std::string((const char *)iv.BytePtr(), iv.size()) + ciphertext, true,
            new CryptoPP::HashFilter(hmac, new CryptoPP::StringSink(mac)));
        }
        catch (const CryptoPP::Exception &e)
        {
            throw std::runtime_error(std::string("AESCBC HMAC error: ") + e.what());
        }

        std::string bin;
        bin.append("CW", 2);
        bin.push_back((char)ALG_AES_CBC);
        bin.append((const char *)salt.BytePtr(), SALT_LEN);
        bin.push_back((char)IV_LEN);
        bin.append((const char *)iv.BytePtr(), IV_LEN);
        uint32_t macLen = (uint32_t)mac.size();
        bin.append((const char *)&macLen, sizeof(macLen));
        bin.append(mac);
        bin.append(ciphertext);
        return ToBase64(bin);
    }

    std::string arcane_symmetric::AESCBC_Decrypt(const std::string &b64, const std::string &pass)
    {
        std::string bin = FromBase64(b64);
        if (bin.size() < 2 + 1 + 16 + 1 + 16 + 4)
            throw std::runtime_error("Invalid ciphertext (AESCBC too short)");
        size_t pos = 2;
        uint8_t alg = (uint8_t)bin[pos++];
        if (alg != ALG_AES_CBC)
            throw std::runtime_error("Algorithm mismatch (expect AES-CBC)");
        const size_t SALT_LEN = 16;
        const size_t saltPos = pos;
        pos += SALT_LEN;
        uint8_t ivLen = (uint8_t)bin[pos++];
        const size_t ivPos = pos;
        pos += ivLen;
        uint32_t macLen;
        std::memcpy(&macLen, bin.data() + pos, sizeof(macLen));
        pos += sizeof(macLen);
        if (bin.size() < pos + macLen)
            throw std::runtime_error("Invalid ciphertext (mac missing)");
        std::string mac = bin.substr(pos, macLen);
        pos += macLen;
        std::string ciphertext = bin.substr(pos);

        std::string macKey = DeriveKeyFromPassword(pass, (const crypto_byte *)bin.data() + saltPos, SALT_LEN, 32, 200000);
        std::string macCheck;
        try
        {
            CryptoPP::HMAC<CryptoPP::SHA256> hmac((const crypto_byte *)macKey.data(), macKey.size());
            CryptoPP::StringSource ss(std::string((const char *)(bin.data() + ivPos), ivLen) + ciphertext, true,
            new CryptoPP::HashFilter(hmac, new CryptoPP::StringSink(macCheck)));
        }
        catch (const CryptoPP::Exception &e)
        {
            throw std::runtime_error(std::string("AESCBC HMAC verify error: ") + e.what());
        }
        if (mac != macCheck)
            throw std::runtime_error("AESCBC HMAC mismatch - integrity failure");

        std::string key = DeriveKeyFromPassword(pass, (const crypto_byte *)bin.data() + saltPos, SALT_LEN, 32);
        try
        {
            CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption dec((const crypto_byte *)key.data(), key.size(), (const crypto_byte *)(bin.data() + ivPos));
            std::string recovered;
            CryptoPP::StringSource ss(ciphertext, true,new CryptoPP::StreamTransformationFilter(dec,
            new CryptoPP::StringSink(recovered),CryptoPP::BlockPaddingSchemeDef::PKCS_PADDING));
            return recovered;
        }
        catch (const CryptoPP::Exception &e)
        {
            throw std::runtime_error(std::string("AESCBC decrypt error: ") + e.what());
        }
    }

    std::string arcane_symmetric::AESCTR_Encrypt(const std::string &plaintext, const std::string &pass)
    {
        aeon_random rng;
        const size_t SALT_LEN = 16;
        const size_t IV_LEN = 16;
        const size_t KEY_LEN = 32;
        CryptoPP::SecByteBlock salt(SALT_LEN);
        rng.pool().GenerateBlock(salt, SALT_LEN);
        std::string key = DeriveKeyFromPassword(pass, salt.BytePtr(), SALT_LEN, KEY_LEN);
        CryptoPP::SecByteBlock iv(IV_LEN);
        rng.pool().GenerateBlock(iv, IV_LEN);

        std::string ciphertext;
        try
        {
            CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption enc((const crypto_byte *)key.data(), key.size(), iv);
            CryptoPP::StringSource ss(plaintext, true,new CryptoPP::StreamTransformationFilter(enc, 
            new CryptoPP::StringSink(ciphertext)));
        }
        catch (const CryptoPP::Exception &e)
        {
            throw std::runtime_error(std::string("AESCTR encrypt error: ") + e.what());
        }

        std::string macKey = DeriveKeyFromPassword(pass, salt.BytePtr(), SALT_LEN, 32, 200000);
        std::string mac;
        CryptoPP::HMAC<CryptoPP::SHA256> hmac((const crypto_byte *)macKey.data(), macKey.size());
        CryptoPP::StringSource ss(std::string((const char *)iv.BytePtr(), iv.size()) + ciphertext, true,
        new CryptoPP::HashFilter(hmac, new CryptoPP::StringSink(mac)));

        std::string bin;
        bin.append("CW", 2);
        bin.push_back((char)ALG_AES_CTR);
        bin.append((const char *)salt.BytePtr(), SALT_LEN);
        bin.push_back((char)IV_LEN);
        bin.append((const char *)iv.BytePtr(), IV_LEN);
        uint32_t macLen = (uint32_t)mac.size();
        bin.append((const char *)&macLen, sizeof(macLen));
        bin.append(mac);
        bin.append(ciphertext);
        return ToBase64(bin);
    }

    std::string arcane_symmetric::AESCTR_Decrypt(const std::string &b64, const std::string &pass)
    {
        std::string bin = FromBase64(b64);
        size_t pos = 2;
        if (bin[0] != 'C' || bin[1] != 'W')
            throw std::runtime_error("Invalid format");
        uint8_t alg = (uint8_t)bin[pos++];
        if (alg != ALG_AES_CTR)
            throw std::runtime_error("Algorithm mismatch (expect AES-CTR)");
        const size_t SALT_LEN = 16;
        size_t saltPos = pos;
        pos += SALT_LEN;
        uint8_t ivLen = (uint8_t)bin[pos++];
        size_t ivPos = pos;
        pos += ivLen;
        uint32_t macLen;
        std::memcpy(&macLen, bin.data() + pos, sizeof(macLen));
        pos += sizeof(macLen);
        std::string mac = bin.substr(pos, macLen);
        pos += macLen;
        std::string ciphertext = bin.substr(pos);

        std::string macKey = DeriveKeyFromPassword(pass, (const crypto_byte *)bin.data() + saltPos, SALT_LEN, 32, 200000);
        std::string macCheck;
        CryptoPP::HMAC<CryptoPP::SHA256> hmac((const crypto_byte *)macKey.data(), macKey.size());
        CryptoPP::StringSource ss(std::string((const char *)(bin.data() + ivPos), ivLen) + ciphertext, true,
        new CryptoPP::HashFilter(hmac, new CryptoPP::StringSink(macCheck)));
        if (mac != macCheck)
            throw std::runtime_error("AESCTR HMAC mismatch - integrity failure");

        std::string key = DeriveKeyFromPassword(pass, (const crypto_byte *)bin.data() + saltPos, SALT_LEN, 32);
        try
        {
            std::string recovered;
            CryptoPP::CTR_Mode<CryptoPP::AES>::Decryption dec((const crypto_byte *)key.data(), key.size(), (const crypto_byte *)(bin.data() + ivPos));
            CryptoPP::StringSource ss2(ciphertext, true,new CryptoPP::StreamTransformationFilter(dec,
            new CryptoPP::StringSink(recovered)));
            return recovered;
        }
        catch (const CryptoPP::Exception &e)
        {
            throw std::runtime_error(std::string("AESCTR decrypt error: ") + e.what());
        }
    }

    std::string arcane_symmetric::TwofishCTR_Encrypt(const std::string &plaintext, const std::string &pass)
    {
        aeon_random rng;
        const size_t SALT_LEN = 16;
        const size_t IV_LEN = 16;
        const size_t KEY_LEN = 32;
        CryptoPP::SecByteBlock salt(SALT_LEN);
        rng.pool().GenerateBlock(salt, SALT_LEN);
        std::string key = DeriveKeyFromPassword(pass, salt.BytePtr(), SALT_LEN, KEY_LEN);
        CryptoPP::SecByteBlock iv(IV_LEN);
        rng.pool().GenerateBlock(iv, IV_LEN);

        std::string ciphertext;
        try
        {
            CryptoPP::CTR_Mode<CryptoPP::Twofish>::Encryption enc((const crypto_byte *)key.data(), key.size(), iv);
            CryptoPP::StringSource ss(plaintext, true,new CryptoPP::StreamTransformationFilter(enc, 
            new CryptoPP::StringSink(ciphertext)));
        }
        catch (const CryptoPP::Exception &e)
        {
            throw std::runtime_error(std::string("TwofishCTR encrypt error: ") + e.what());
        }

        std::string macKey = DeriveKeyFromPassword(pass, salt.BytePtr(), SALT_LEN, 32, 200000);
        std::string mac;
        CryptoPP::HMAC<CryptoPP::SHA256> hmac((const crypto_byte *)macKey.data(), macKey.size());
        CryptoPP::StringSource ss(std::string((const char *)iv.BytePtr(), iv.size()) + ciphertext, true,
        new CryptoPP::HashFilter(hmac, new CryptoPP::StringSink(mac)));

        std::string bin;
        bin.append("CW", 2);
        bin.push_back((char)ALG_TWOFISH_CTR);
        bin.append((const char *)salt.BytePtr(), SALT_LEN);
        bin.push_back((char)IV_LEN);
        bin.append((const char *)iv.BytePtr(), IV_LEN);
        uint32_t macLen = (uint32_t)mac.size();
        bin.append((const char *)&macLen, sizeof(macLen));
        bin.append(mac);
        bin.append(ciphertext);
        return ToBase64(bin);
    }

    std::string arcane_symmetric::TwofishCTR_Decrypt(const std::string &b64, const std::string &pass)
    {
        std::string bin = FromBase64(b64);
        if (bin.size() < 2 + 1 + 16 + 1 + 16 + 4)
            throw std::runtime_error("Invalid Twofish ciphertext");
        size_t pos = 2;
        uint8_t alg = (uint8_t)bin[pos++];
        if (alg != ALG_TWOFISH_CTR)
            throw std::runtime_error("Algorithm mismatch (expect Twofish-CTR)");
        const size_t SALT_LEN = 16;
        size_t saltPos = pos;
        pos += SALT_LEN;
        uint8_t ivLen = (uint8_t)bin[pos++];
        size_t ivPos = pos;
        pos += ivLen;
        uint32_t macLen;
        std::memcpy(&macLen, bin.data() + pos, sizeof(macLen));
        pos += sizeof(macLen);
        std::string mac = bin.substr(pos, macLen);
        pos += macLen;
        std::string ciphertext = bin.substr(pos);

        std::string macKey = DeriveKeyFromPassword(pass, (const crypto_byte *)bin.data() + saltPos, SALT_LEN, 32, 200000);
        std::string macCheck;
        CryptoPP::HMAC<CryptoPP::SHA256> hmac((const crypto_byte *)macKey.data(), macKey.size());
        CryptoPP::StringSource ss(std::string((const char *)(bin.data() + ivPos), ivLen) + ciphertext, true,
        new CryptoPP::HashFilter(hmac, new CryptoPP::StringSink(macCheck)));
        if (mac != macCheck)
            throw std::runtime_error("Twofish HMAC mismatch");

        std::string key = DeriveKeyFromPassword(pass, (const crypto_byte *)bin.data() + saltPos, SALT_LEN, 32);
        try
        {
            std::string recovered;
            CryptoPP::CTR_Mode<CryptoPP::Twofish>::Decryption dec((const crypto_byte *)key.data(), key.size(), (const crypto_byte *)(bin.data() + ivPos));
            CryptoPP::StringSource ss2(ciphertext, true,new CryptoPP::StreamTransformationFilter(dec, 
            new CryptoPP::StringSink(recovered)));
            return recovered;
        }
        catch (const CryptoPP::Exception &e)
        {
            throw std::runtime_error(std::string("TwofishCTR decrypt error: ") + e.what());
        }
    }

    // 实现: penumbra_asymmetric
    std::pair<std::string, std::string> penumbra_asymmetric::GenerateRSAKeypair(unsigned int bits)
    {
        aeon_random rng;
        CryptoPP::AutoSeededRandomPool &prng = rng.pool();
        CryptoPP::InvertibleRSAFunction params;
        params.Initialize(prng, bits);

        CryptoPP::RSA::PrivateKey priv(params);
        CryptoPP::RSA::PublicKey pub(params);

        std::string privDer, pubDer;
        CryptoPP::ByteQueue qb;
        priv.Save(qb);
        CryptoPP::StringSink ss1(privDer);
        qb.CopyTo(ss1);
        ss1.MessageEnd();

        CryptoPP::ByteQueue qb2;
        pub.Save(qb2);
        CryptoPP::StringSink ss2(pubDer);
        qb2.CopyTo(ss2);
        ss2.MessageEnd();

        return {ToBase64(privDer), ToBase64(pubDer)};
    }

    std::string penumbra_asymmetric::RSA_OAEP_Encrypt(const std::string &plaintext, const std::string &pubDerBase64)
    {
        std::string pubDer = FromBase64(pubDerBase64);
        CryptoPP::ByteQueue queue;
        queue.Put((const crypto_byte *)pubDer.data(), pubDer.size());
        queue.MessageEnd();
        CryptoPP::RSA::PublicKey pub;
        pub.Load(queue);

        aeon_random rng;
        CryptoPP::AutoSeededRandomPool &prng = rng.pool();
        std::string cipher;
        try
        {
            CryptoPP::RSAES_OAEP_SHA_Encryptor enc(pub);
            CryptoPP::StringSource ss(plaintext, true,new CryptoPP::PK_EncryptorFilter(prng, enc,
            new CryptoPP::StringSink(cipher)));
        }
        catch (const CryptoPP::Exception &e)
        {
            throw std::runtime_error(std::string("RSA OAEP encrypt error: ") + e.what());
        }
        return ToBase64(cipher);
    }

    std::string penumbra_asymmetric::RSA_OAEP_Decrypt(const std::string &cipherB64, const std::string &privDerBase64)
    {
        std::string privDer = FromBase64(privDerBase64);
        CryptoPP::ByteQueue queue;
        queue.Put((const crypto_byte *)privDer.data(), privDer.size());
        queue.MessageEnd();
        CryptoPP::RSA::PrivateKey priv;
        priv.Load(queue);

        std::string cipher = FromBase64(cipherB64);
        aeon_random rng;
        CryptoPP::AutoSeededRandomPool &prng = rng.pool();
        try
        {
            CryptoPP::RSAES_OAEP_SHA_Decryptor dec(priv);
            std::string recovered;
            CryptoPP::StringSource ss(cipher, true,new CryptoPP::PK_DecryptorFilter(prng, dec,
            new CryptoPP::StringSink(recovered)));
            return recovered;
        }
        catch (const CryptoPP::Exception &e)
        {
            throw std::runtime_error(std::string("RSA OAEP decrypt error: ") + e.what());
        }
    }

    std::string penumbra_asymmetric::RSA_PSS_Sign(const std::string &msg, const std::string &privDerBase64)
    {
        std::string privDer = FromBase64(privDerBase64);
        CryptoPP::ByteQueue qb;
        qb.Put((const crypto_byte *)privDer.data(), privDer.size());
        qb.MessageEnd();
        CryptoPP::RSA::PrivateKey priv;
        priv.Load(qb);

        aeon_random rng;
        CryptoPP::AutoSeededRandomPool &prng = rng.pool();
        std::string signature;
        try
        {
            CryptoPP::RSASS<CryptoPP::PSS, CryptoPP::SHA256>::Signer signer(priv);
            CryptoPP::StringSource ss(msg, true,new CryptoPP::SignerFilter(prng, signer,
            new CryptoPP::StringSink(signature)));
            return ToBase64(signature);
        }
        catch (const CryptoPP::Exception &e)
        {
            throw std::runtime_error(std::string("RSA PSS sign error: ") + e.what());
        }
    }

    bool penumbra_asymmetric::RSA_PSS_Verify(const std::string &msg, const std::string &sigB64, const std::string &pubDerBase64)
    {
        std::string pubDer = FromBase64(pubDerBase64);
        CryptoPP::ByteQueue qb;
        qb.Put((const crypto_byte *)pubDer.data(), pubDer.size());
        qb.MessageEnd();
        CryptoPP::RSA::PublicKey pub;
        pub.Load(qb);

        std::string signature = FromBase64(sigB64);
        try
        {
            CryptoPP::RSASS<CryptoPP::PSS, CryptoPP::SHA256>::Verifier verifier(pub);
            bool ok = verifier.VerifyMessage((const crypto_byte *)msg.data(), msg.size(), (const crypto_byte *)signature.data(), signature.size());
            return ok;
        }
        catch (...)
        {
            return false;
        }
    }

    std::pair<std::string, std::string> penumbra_asymmetric::GenerateElGamalKeypair(unsigned int pbits)
    {
        aeon_random rng;
        CryptoPP::AutoSeededRandomPool &prng = rng.pool();

        CryptoPP::DL_GroupParameters_GFP params;
        params.GenerateRandomWithKeySize(prng, pbits);
        CryptoPP::ElGamalKeys::PrivateKey priv;
        CryptoPP::ElGamalKeys::PublicKey pub;
        priv.GenerateRandom(prng, params);
        priv.MakePublicKey(pub);



        std::string privDer, pubDer;
        CryptoPP::ByteQueue qb;
        priv.Save(qb);
        qb.MessageEnd();
        CryptoPP::StringSink ss1(privDer);
        qb.CopyTo(ss1);
        ss1.MessageEnd();
        CryptoPP::ByteQueue qb2;
        pub.Save(qb2);
        qb2.MessageEnd();
        CryptoPP::StringSink ss2(pubDer);
        qb2.CopyTo(ss2);
        ss2.MessageEnd();
        return {ToBase64(privDer), ToBase64(pubDer)};
    }

    std::string penumbra_asymmetric::ElGamal_Encrypt(const std::string &plaintext, const std::string &pubDerBase64)
    {
        std::string pubDer = FromBase64(pubDerBase64);
        CryptoPP::ByteQueue qb;
        qb.Put((const crypto_byte *)pubDer.data(), pubDer.size());
        qb.MessageEnd();
        CryptoPP::ElGamalKeys::PublicKey pub;
        pub.Load(qb);
        aeon_random rng;
        CryptoPP::AutoSeededRandomPool &prng = rng.pool();
        std::string cipher;
        try
        {
            CryptoPP::ElGamalEncryptor enc(pub);
            CryptoPP::StringSource ss(plaintext, true,new CryptoPP::PK_EncryptorFilter(prng, enc, new CryptoPP::StringSink(cipher)));
        }
        catch (const CryptoPP::Exception &e)
        {
            throw std::runtime_error(std::string("ElGamal encrypt error: ") + e.what());
        }
        return ToBase64(cipher);
    }

    std::string penumbra_asymmetric::ElGamal_Decrypt(const std::string &cipherB64, const std::string &privDerBase64)
    {
        std::string privDer = FromBase64(privDerBase64);
        CryptoPP::ByteQueue qb;
        qb.Put((const crypto_byte *)privDer.data(), privDer.size());
        qb.MessageEnd();
        CryptoPP::ElGamalKeys::PrivateKey priv;
        priv.Load(qb);
        std::string cipher = FromBase64(cipherB64);
        aeon_random rng;
        CryptoPP::AutoSeededRandomPool &prng = rng.pool();
        try
        {
            CryptoPP::ElGamalDecryptor dec(priv);
            std::string recovered;
            CryptoPP::StringSource ss(cipher, true,new CryptoPP::PK_DecryptorFilter(prng, dec, new CryptoPP::StringSink(recovered)));
            return recovered;
        }
        catch (const CryptoPP::Exception &e)
        {
            throw std::runtime_error(std::string("ElGamal decrypt error: ") + e.what());
        }
    }

    inline std::string umbrage_hash::MD5(const std::string &data)
    {
        std::string out;
        CryptoPP::Weak::MD5 md;
        CryptoPP::StringSource ss(data, true,new CryptoPP::HashFilter(md, new CryptoPP::HexEncoder(new CryptoPP::StringSink(out), false)));
        return out;
    }

    inline std::string umbrage_hash::SHA256(const std::string &data)
    {
        std::string out;
        CryptoPP::SHA256 sha;
        CryptoPP::StringSource ss(data, true,new CryptoPP::HashFilter(sha, new CryptoPP::HexEncoder(new CryptoPP::StringSink(out), false)));
        return out;
    }

    inline std::uint32_t CRC32(const std::string &data,const std::uint64_t length = 16ULL)
    {
        static constexpr std::uint32_t mapping_table[256] = 
        {
            0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
            0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
            0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
            0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
            0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
            0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
            0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
            0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
            0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
            0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
            0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
            0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
            0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
            0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
            0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
            0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
            0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
            0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
            0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
            0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
            0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
            0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
            0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
            0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
            0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
            0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
            0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
            0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
            0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
            0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
            0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
            0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
        };
        const uint8_t *bytes = reinterpret_cast<const uint8_t *>(data.data());
        uint32_t crc_value = 0xFFFFFFFF;

        for (size_t i = 0; i < length; ++i)
        {
            crc_value = (crc_value >> 8) ^ mapping_table[(crc_value ^ bytes[i]) & 0xFF];
        }

        return ~crc_value;
    }

    inline std::string mix64() noexcept
    {
        alignas(64) char buf[64];

        static constexpr char kKey[32] =
        "\x87\x31\xa2\x4b\xfa\x8f\x63\x27"
        "\xad\x2e\xeb\x19\x1c\x6b\x93\x0f"
        "\x4d\x5a\x76\x42\x13\x37\x55\xde"
        "\xad\xbe\xef\x8a\x24\xbc\xda";
        std::memcpy(buf, kKey, 32);

        uint64_t ts = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        std::memcpy(buf + 32, &ts, 8);

        uint64_t tsc = __builtin_ia32_rdtsc();
        std::memcpy(buf + 40, &tsc, 8);

        *reinterpret_cast<uint64_t*>(buf + 48) = 0xDEADBEEFC0FFEEUL;
        *reinterpret_cast<uint64_t*>(buf + 56) = 0x1234567890ABCDEFUL;

        return std::string(buf, 64); 
    }
} // end encryption
