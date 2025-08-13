#include "Utils.hpp"

namespace Utils
{
    const std::string Modulus =
    "edfd42cbf978546e8911225884436c57140525650bcf6ebfe80edbc5fb1de68f4c66c29cb22eb668788afcb0abbb7180"
    "44584b810f8970cddf227385f75d5dddd91d4f18937a08aa83b28c49d12dc92e7505bb38809e91bd0fbd2f2e6ab1d2e3"
    "3c0c55d5bddd478ee8bf845fcef3c82b9d2929ecb71f4d1b3db96e3a8e7aaf93";

    const std::string Private =
    "77c86abbb7f3bb134436797b68ff47beb1a5457816608dbfb72641814dd464dd640d711d5732d3017a1c4e63d835822f"
    "00a4eab619a2c4791cf33f9f57f9c2ae4d9eed9981e79ac9b8f8a411f68f25b9f0c05d04d11e22a3a0d8d4672b56a61f"
    "1532282ff4e4e74759e832b70e98b9d102d07e9fb9ba8d15810b144970029874";


    nlohmann::json SortJSONKeys(const nlohmann::json& j)
    {
        if (j.is_object())
        {
            std::vector<std::pair<std::string, nlohmann::json>> Items;
            Items.reserve(j.size());

            for (auto it = j.begin(); it != j.end(); ++it)
                Items.emplace_back(it.key(), SortJSONKeys(it.value()));
            std::sort(Items.begin(), Items.end(), [](auto& a, auto& b) { return a.first < b.first; });

            nlohmann::json Out = nlohmann::json::object();
            for (auto& kv : Items) Out[kv.first] = kv.second;
            return Out;
        }
        else if (j.is_array())
        {
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& v : j) arr.push_back(SortJSONKeys(v));
            return arr;
        }
        return j;
    }

    std::string DumpSorted(const nlohmann::json& j)
    {
        return SortJSONKeys(j).dump();
    }

    std::string DumpSortedPretty(const nlohmann::json& j, std::uint16_t indent)
    {
        return SortJSONKeys(j).dump(indent);
    }

    std::vector<std::uint8_t> HexToBytes(std::string_view hex)
    {
        auto hv = [](char c)->int
        {
            if (c >= '0' && c <= '9') return c - '0';
            if (c >= 'a' && c <= 'f') return 10+(c-'a');
            if (c >= 'A' && c <= 'F') return 10+(c-'A');
            return -1;
        };
        if (hex.size() % 2) throw std::runtime_error("Hex length must be even!");
        std::vector<std::uint8_t> Out; Out.reserve(hex.size() / 2);

        for (std::size_t i = 0; i < hex.size(); i += 2)
        {
            int hi = hv(hex[i]), lo = hv(hex[i+1]);
            if (hi < 0 || lo < 0) throw std::runtime_error("Invalid hex!");
            Out.push_back(static_cast<std::uint8_t>((hi << 4) | lo));
        }
        return Out;
    }

    std::string BytesToUpperHex(const std::vector<std::uint8_t>& buffer)
    {
        std::ostringstream oss;
        oss << std::uppercase << std::hex << std::setfill('0');

        for (auto b : buffer) oss << std::setw(2) << static_cast<int>(b);
        return oss.str();
    }

    BN_ptr BNFromLEBytes(const std::vector<std::uint8_t>& le)
    {
        BIGNUM* bn = BN_lebin2bn(le.data(), static_cast<int>(le.size()), nullptr);
        if (!bn) throw std::runtime_error("BN_lebin2bn failed!");
        return BN_ptr(bn);
    }

    std::vector<std::uint8_t> BNToLEBytesMin(const BIGNUM* bn)
    {
        if (BN_is_zero(bn)) return {};
        const int nbytes = BN_num_bytes(bn);
        std::vector<std::uint8_t> Out(static_cast<std::size_t>(nbytes));
        const int rc = BN_bn2lebinpad(bn, Out.data(), nbytes);
        if (rc != nbytes) throw std::runtime_error("BN_bn2lebinpad failed!");
        return Out;
    }

    BN_ptr BNModExp(const BIGNUM* base, const BIGNUM* exp, const BIGNUM* mod)
    {
        CTX_ptr ctx(BN_CTX_new());
        if (!ctx) throw std::runtime_error("BN_CTX_new failed!");
        BN_ptr Out(BN_new());
        if (!Out) throw std::runtime_error("BN_new failed!");
        if (BN_mod_exp(Out.get(), base, exp, mod, ctx.get()) != 1)
            throw std::runtime_error("BN_mod_exp failed!");
        return Out;
    }

    void SecureRandBytes(std::uint8_t* dst, std::size_t n)
    {
        if (RAND_bytes(dst, static_cast<int>(n)) != 1)
            throw std::runtime_error("RAND_bytes failed");
    }

    std::string RandHex(std::size_t nbytes)
    {
        std::vector<std::uint8_t> b(nbytes);
        SecureRandBytes(b.data(), b.size());
        return BytesToUpperHex(b);
    }

    std::string TimeToString(const std::tm& t)
    {
        std::ostringstream oss;
        oss << std::put_time(&t, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    std::tm LocalNowTime()
    {
        std::time_t now = std::time(nullptr);
        std::tm out{};
#if defined(_WIN64)
        localtime_s(&out, &now);
#else
        localtime_r(&now, &out);
#endif
        return out;
    }

    std::tm EndOfYearPlus10(const std::tm& start)
    {
        std::tm end = start;
        end.tm_year = start.tm_year + 10;
        end.tm_mon = 11;
        end.tm_mday = 31;
        end.tm_hour = 23;
        end.tm_min = 59;
        end.tm_sec = 59;
        std::mktime(&end);
        return end;
    }

    std::string GenLicID()
    {
        const std::string p1 = RandHex(2);
        const std::string p2 = RandHex(2);
        const std::string p3 = RandHex(1);
        return "77-" + p1 + "-" + p2 + "-" + p3;
    }

    std::string GenAddonPrefix()
    {
        const std::string p1 = RandHex(2);
        const std::string p2 = RandHex(2);
        return "77-" + p1 + "-" + p2 + "-";
    }

    std::string ComputeSignature(const nlohmann::json& payload)
    {
        nlohmann::json Data = nlohmann::json::object();
        Data["payload"] = payload;
        const std::string DataStr = DumpSorted(Data);

        std::vector<std::uint8_t> buffer(128, 0x00);
        std::fill(buffer.begin(), buffer.begin() + 33, 0x77);

        std::uint8_t sha[32];
        SHA256(reinterpret_cast<const unsigned char*>(DataStr.data()), DataStr.size(), sha);

        for (std::size_t i = 0; i < 32; ++i) buffer[33 + i] = sha[i];

        std::vector<std::uint8_t> RevBuff(buffer.rbegin(), buffer.rend());

        BN_ptr n = BNFromLEBytes(HexToBytes(Modulus));
        BN_ptr d = BNFromLEBytes(HexToBytes(Private));
        BN_ptr m = BNFromLEBytes(RevBuff);

        BN_ptr c = BNModExp(m.get(), d.get(), n.get());
        const auto SigLE = BNToLEBytesMin(c.get());
        return BytesToUpperHex(SigLE);
    }
}
