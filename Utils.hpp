#pragma once

#include <cstdint>
#include <ctime>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <sstream>

#include <nlohmann/json.hpp>
#include <openssl/bn.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

namespace Utils
{
    nlohmann::json SortJSONKeys(const nlohmann::json& j);
    std::string DumpSorted(const nlohmann::json& j);
    std::string DumpSortedPretty(const nlohmann::json& j, std::uint16_t indent = 2);

    std::vector<std::uint8_t> HexToBytes(std::string_view hex);
    std::string BytesToUpperHex(const std::vector<std::uint8_t>& buffer);

    struct BN_Deleter { void operator()(BIGNUM* p) const noexcept { if (p) BN_free(p); } };
    struct CTX_Deleter { void operator()(BN_CTX* p) const noexcept { if (p) BN_CTX_free(p); } };
    using BN_ptr  = std::unique_ptr<BIGNUM, BN_Deleter>;
    using CTX_ptr = std::unique_ptr<BN_CTX,  CTX_Deleter>;

    BN_ptr BNFromLEBytes(const std::vector<std::uint8_t>& le); // little endian
    std::vector<std::uint8_t> BNToLEBytesMin(const BIGNUM* bn);
    BN_ptr BNModExp(const BIGNUM* base, const BIGNUM* exp, const BIGNUM* mod);

    void SecureRandBytes(std::uint8_t* dst, std::size_t n);

    std::string RandHex(std::size_t nbytes);

    std::string TimeToString(const std::tm& t);
    std::tm LocalNowTime();
    std::tm EndOfYearPlus10(const std::tm& start);

    std::string GenLicID();
    std::string GenAddonPrefix();

    std::string ComputeSignature(const nlohmann::json& payload);
}
