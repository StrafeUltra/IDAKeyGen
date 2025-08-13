#include "Utils.hpp"

#include <iostream>
#include <fstream>

#if defined(_WIN64)
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "WS2_32.lib")

#endif

int main()
{
#if defined(_WIN64)
    ::SetConsoleTitleA("IDAKeyGen");
    ::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), 11);
#endif

    std::cout << "Please enter your desired name and email: ";

    std::string Input, Name, Email;
    std::getline(std::cin, Input);
    std::size_t LastPos = Input.rfind(' ');

    if (LastPos != std::string::npos) // too lazy to make a more proper implementation and i dont wanna copy the same garbage i was using in tetanus
    {
        Name = Input.substr(0, LastPos);
        Email = Input.substr(LastPos + 1);
    }
    else
    {
        std::cerr << "Invalid input, please enter name and email!" << std::endl;

#if defined(_WIN64)
        std::getchar(); // fuck you windows
#endif
        return 1;
    }

    nlohmann::json License = {
        {"header", {{"version", 1}}},
        {"payload", {{"name", Name}, {"email", Email }, {"licenses", nlohmann::json::array()}}}
    };

    const std::tm StartTime = Utils::LocalNowTime();
    const std::tm EndTime = Utils::EndOfYearPlus10(StartTime);
    const std::string StartStr = Utils::TimeToString(StartTime);
    const std::string EndStr = Utils::TimeToString(EndTime);

    const std::string LicenseID = Utils::GenLicID();

    nlohmann::json LicEntry = {
        {"id", LicenseID},
        {"license_type", "named"},
        {"product", "IDA"},
        {"seats", 1},
        {"start_date", StartStr},
        {"end_date", EndStr},
        {"issued_on", StartStr},
        {"owner", Name},
        {"edition_id", "ida-pro"},
        {"product_id", "IDAPRO"},
        {"add_ons", nlohmann::json::array()},
        {"features", nlohmann::json::array()}
    };

    static constexpr const char* AllAddons[] = {
        "LUMINA", "TEAMS", // Lumina and Teams support
        "HEXX86", "HEXX64", // i386 and AMD64 decompilers
        "HEXRV", "HEXRV64", // RISC-V and RISC-V64 decompilers
        "HEXPPC", "HEXPPC64", // PowerPC and PowerPC64 decompilers
        "HEXARM", "HEXARM64", // ARM and AArch64 decompilers
        "HEXMIPS", "HEXMIPS64", // MIPS and MIPS64 decompilers
        "HEXARC", "HEXARC64" // ARC and ARC64 decompilers
    };

    const std::string AddonPfx = Utils::GenAddonPrefix();

    for (int i = 0; i < static_cast<int>(std::size(AllAddons)); ++i)
    {
        std::ostringstream idbuf;
        idbuf << AddonPfx << std::setw(2) << std::setfill('0') << (i + 1);
        LicEntry["add_ons"].push_back(nlohmann::json{{"id", idbuf.str()}, {"code", AllAddons[i]}, {"owner", LicenseID}, {"start_date", StartStr}, {"end_date", EndStr}});

    }

    License["payload"]["licenses"].push_back(LicEntry);

    const std::string Signature = Utils::ComputeSignature(License["payload"]);

    License["signature"] = Signature;

    const std::string Serialized = Utils::DumpSortedPretty(License, 2);
    const std::string FileName = "idapro_" + LicenseID + ".hexlic";

    std::ofstream ofs(FileName, std::ios::binary);
    if (!ofs)
    {
        std::cerr << "Failed to create " << FileName << "!" << std::endl;

#if defined(_WIN64)
        std::getchar(); // fuck you windows
#endif
        return 1;
    }

    ofs << Serialized << "\n";

    ofs.close();

    std::cout << "Saved new license to " << FileName << "!" << std::endl;

#if defined(_WIN64)
    std::getchar(); // fuck you windows
#endif

    return 0;
}
