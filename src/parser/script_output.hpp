//
//  script_output.hpp
//  blocksci
//
//  Created by Harry Kalodner on 3/18/17.
//
//

#ifndef script_output_hpp
#define script_output_hpp

#include "basic_types.hpp"

#include <blocksci/scripts/bitcoin_script.hpp>
#include <blocksci/scripts/bitcoin_pubkey.hpp>
#include <blocksci/address/address_info.hpp>
#include <blocksci/address/address.hpp>

#include <boost/variant/variant_fwd.hpp>

#include <array>
#include <stdio.h>

class AddressWriter;
class AddressState;


struct ScriptOutputBase {
    void processOutput(AddressState &) {}
    void checkOutput(const AddressState &) {}
};

template<blocksci::AddressType::Enum type>
struct ScriptOutput;

template <>
struct ScriptOutput<blocksci::AddressType::Enum::PUBKEY> : public ScriptOutputBase {
    CPubKey pubkey;
    
    ScriptOutput(const std::vector<unsigned char> &vch1);
    ScriptOutput(const CPubKey &pub) : pubkey(pub) {}
    
    bool isValid() const { return true; }
    
    blocksci::uint160 getHash() const;
};

template <>
struct ScriptOutput<blocksci::AddressType::Enum::PUBKEYHASH> : public ScriptOutputBase {
    
    CKeyID hash;
    
    ScriptOutput(blocksci::uint160 &pubkeyHash) : hash{pubkeyHash} {}
    
    bool isValid() const { return true; }
    
    blocksci::uint160 getHash() const;
};

template <>
struct ScriptOutput<blocksci::AddressType::Enum::WITNESS_PUBKEYHASH> : public ScriptOutputBase {
    
    CKeyID hash;
    
    ScriptOutput(blocksci::uint160 &&pubkeyHash) : hash{pubkeyHash} {}
    
    bool isValid() const { return true; }
    
    blocksci::uint160 getHash() const;
};

template <>
struct ScriptOutput<blocksci::AddressType::Enum::SCRIPTHASH> : public ScriptOutputBase {
    CKeyID hash;
    
    ScriptOutput(blocksci::uint160 hash_) : hash(hash_) {}
    
    blocksci::uint160 getHash() const;
    
    bool isValid() const {
        return true;
    }
};

template <>
struct ScriptOutput<blocksci::AddressType::Enum::WITNESS_SCRIPTHASH> : public ScriptOutputBase {
    blocksci::uint256 hash;
    
    ScriptOutput(blocksci::uint256 hash_) : hash(hash_) {}
    
    blocksci::uint160 getHash() const;
    
    bool isValid() const {
        return true;
    }
};

template <>
struct ScriptOutput<blocksci::AddressType::Enum::MULTISIG> {
    static constexpr int MAX_ADDRESSES = 16;
    using RawAddressArray = std::array<CPubKey, MAX_ADDRESSES>;
    using ProcessedAddressArray = std::array<uint32_t, MAX_ADDRESSES>;
    using FirstSeenArray = std::array<bool, MAX_ADDRESSES>;
    uint8_t numRequired;
    uint8_t numTotal;
    uint16_t addressCount;
    
    ProcessedAddressArray processedAddresses;
    RawAddressArray addresses;
    FirstSeenArray firstSeen;
    
    ScriptOutput() : addressCount(0) {}
    
    void addAddress(const std::vector<unsigned char> &vch1);
    
    bool isValid() const {
        return numRequired <= numTotal && numTotal == addressCount;
    }
    
    blocksci::uint160 getHash() const;
    void processOutput(AddressState &state);
    void checkOutput(const AddressState &state);
};

template <>
struct ScriptOutput<blocksci::AddressType::Enum::NONSTANDARD> : public ScriptOutputBase {
    CScript script;
    
    ScriptOutput<blocksci::AddressType::Enum::NONSTANDARD>() {};
    ScriptOutput<blocksci::AddressType::Enum::NONSTANDARD>(const CScript &script);
    
    bool isValid() const {
        return true;
    }
};

template <>
struct ScriptOutput<blocksci::AddressType::Enum::NULL_DATA> : public ScriptOutputBase {
    std::vector<unsigned char> fullData;
    
    ScriptOutput<blocksci::AddressType::Enum::NULL_DATA>(const CScript &script);
    
    bool isValid() const {
        return true;
    }
};

using ScriptOutputType = blocksci::to_script_variant_t<ScriptOutput, blocksci::AddressInfoList>;

template <blocksci::AddressType::Enum type>
std::pair<blocksci::Address, bool> getAddressNum(ScriptOutput<type> &data, AddressState &state);

template <blocksci::AddressType::Enum type>
std::pair<blocksci::Address, bool> checkAddressNum(ScriptOutput<type> &data, const AddressState &state);

blocksci::AddressType::Enum addressType(const ScriptOutputType &type);

ScriptOutputType extractScriptData(const unsigned char *scriptBegin, const unsigned char *scriptEnd);

#endif /* script_output_hpp */
