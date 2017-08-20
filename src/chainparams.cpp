// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2017 The SLING developers
// Copyright (c) 2017 The Sling developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"

#include "genesis.h"
#include "random.h"
#include "util.h"
#include "utilstrencodings.h"
#include "chainparamsseeds.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

using namespace std;
using namespace boost::assign;

/**
 * Main network
 */

//! Convert the pnSeeds6 array into usable address objects.
static void convertSeed6(std::vector<CAddress>& vSeedsOut, const SeedSpec6* data, unsigned int count)
{
    // It'll only connect to one or two seed nodes because once it connects,
    // it'll get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7 * 24 * 60 * 60;
    for (unsigned int i = 0; i < count; i++) {
        struct in6_addr ip;
        memcpy(&ip, data[i].addr, sizeof(ip));
        CAddress addr(CService(ip, data[i].port));
        addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

//   What makes a good checkpoint block?
// + Is surrounded by blocks with reasonable timestamps
//   (no blocks before with a timestamp after, none after with
//    timestamp before)
// + Contains no strange transactions
static Checkpoints::MapCheckpoints mapCheckpoints =
    boost::assign::map_list_of
    (0, uint256("0x0000021e90d6ff8188c9b61fb30851922bde228df648a8324ac547ddb2693c26"))
    ;
static const Checkpoints::CCheckpointData data = {
    &mapCheckpoints,
    1501986576,     // * UNIX timestamp of last checkpoint block
    0,              // * total number of transactions between genesis and last checkpoint
                    //   (the tx=... number in the SetBestChain debug.log lines)
    1440            // * estimated number of transactions per day after checkpoint
};

static Checkpoints::MapCheckpoints mapCheckpointsTestnet =
    boost::assign::map_list_of(0, uint256("0x00000da7f9cfca0b5a66eae610cb8c2e567f272a54c5ea7cf480c8df135967b3"));
static const Checkpoints::CCheckpointData dataTestnet = {
    &mapCheckpointsTestnet,
    1501987810,
    0,
    1440};

static Checkpoints::MapCheckpoints mapCheckpointsRegtest =
    boost::assign::map_list_of(0, uint256("0x0"));
static const Checkpoints::CCheckpointData dataRegtest = {
    &mapCheckpointsRegtest,
    1501985300,
    0,
    1440};

class CMainParams : public CChainParams
{
public:
    CMainParams()
    {
        networkID = CBaseChainParams::MAIN;
        strNetworkID = "main";
        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 4-byte int at any alignment.
         */
        pchMessageStart[0] = 0x13;
        pchMessageStart[1] = 0x37;
        pchMessageStart[2] = 0x17;
        pchMessageStart[3] = 0x71;
        vAlertPubKey = ParseHex("048216a20abab9c0edb9a813328bbc4dad2082dda77c4b990bccbade46a8a331928c3a165aa4bd8ef1d8e1b9c9660da46dd6371eed67f92ec711d2e0cbfb13b47e");
        nDefaultPort = 31135;
        bnProofOfWorkLimit = ~uint256(0) >> 20; // Sling starting difficulty is 1 / 2^20
        nSubsidyHalvingInterval = 210000;
        nMaxReorganizationDepth = 100;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 0;
        nTargetPoWSpacing = 7 * 60;  // Sling: 7 minutes
        nTargetPoSTimespan = 67 * 40;  // Sling: not sure?
        nTargetPoSSpacing = 67;   // Sling: 67 seconds
        nStartMasternodePayments = 1501965300; //Sat, 2017-08-05 20:35:00 GMT
        nFirstRewardBlock = 35; //TODO: CryptoDJ, increase to 3500
        nFirstPoSBlock = 11; //TODO: CryptoDJ, increase to 35000
        nMaturity = 10; //TODO: CryptoDJ, increase to 100
        nMasternodeCountDrift = 20;
        nModifierUpdateBlock = 615800;
        startNewChain = false;

        genesis = CreateGenesisBlock(1502865184, 54026, Convert_UintToArith256_GetCompact(bnProofOfWorkLimit), 1, (1.337 * COIN));
        if(startNewChain == true) {
            MineGenesis(genesis, bnProofOfWorkLimit, true);
        }

        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("0x0000021e90d6ff8188c9b61fb30851922bde228df648a8324ac547ddb2693c26"));
        assert(genesis.hashMerkleRoot == uint256("0xcf8acb7ce43a07bc66a8b9daec515df36bd0d919bae147b13ea509131cc98bf5"));

        vSeeds.push_back(CDNSSeedData("slingmarket.net", "seed.slingmarket.net"));     // Primary DNS Seeder from slingmarket.net
        
        base58Prefixes[PUBKEY_ADDRESS] = boost::assign::list_of(63).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[SCRIPT_ADDRESS] = boost::assign::list_of(85).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[SECRET_KEY]     = boost::assign::list_of(153).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();
        // 	BIP44 coin type is from https://github.com/satoshilabs/slips/blob/master/slip-0044.md
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x04)(0x88)(0x00)(0x77).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main));

        fRequireRPCPassword = true;
        fMiningRequiresPeers = false; //TODO: CryptoDJ, change back to true
        fAllowMinDifficultyBlocks = true; //TODO: CryptoDJ, change back to false
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fSkipProofOfWorkCheck = false;
        fTestnetToBeDeprecatedFieldRPC = false;
        fHeadersFirstSyncingActive = false;

        nPoolMaxTransactions = 3;
        strSporkKey = "048216a20abab9c0edb9a813328bbc4dad2082dda77c4b990bccbade46a8a331928c3a165aa4bd8ef1d8e1b9c9660da46dd6371eed67f92ec711d2e0cbfb13b47e";
        strCoinMixPoolDummyAddress = "ShAEbDNFkjjtTLykrGDJ36SU2Hwroxf3P6";
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return data;
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CMainParams
{
public:
    CTestNetParams()
    {
        networkID = CBaseChainParams::TESTNET;
        strNetworkID = "test";
        pchMessageStart[0] = 0x13;
        pchMessageStart[1] = 0x37;
        pchMessageStart[2] = 0x1f;
        pchMessageStart[3] = 0x31;
        vAlertPubKey = ParseHex("048216a20abab9c0edb9a813328bbc4dad2082dda77c4b990bccbade46a8a331928c3a165aa4bd8ef1d8e1b9c9660da46dd6371eed67f92ec711d2e0cbfb13b47e");
        bnProofOfWorkLimit = ~uint256(0) >> 20; // Sling starting difficulty is 1 / 2^20
        nDefaultPort = 31136;
        nEnforceBlockUpgradeMajority = 51;
        nRejectBlockOutdatedMajority = 75;
        nToCheckBlockUpgradeMajority = 100;
        nMinerThreads = 0;
        nTargetPoWSpacing = 5 * 60;  // Sling: 5 minutes
        nTargetPoSTimespan = 35 * 40;  // Sling: not sure?
        nTargetPoSSpacing = 35;   // Sling: 35 seconds
        nStartMasternodePayments = 1501965335; //Sat, 2017-08-05 20:35:35 GMT
        nFirstRewardBlock = 15;
        nFirstPoSBlock = 11;
        nMaturity = 10;
        nModifierUpdateBlock = 51197; //approx Mon, 17 Apr 2017 04:00:00 GMT
        startNewChain = false;

        genesis = CreateGenesisBlock(1502865349, 417171, Convert_UintToArith256_GetCompact(bnProofOfWorkLimit), 1, (1.337 * COIN));
        if(startNewChain == true) {
            MineGenesis(genesis, bnProofOfWorkLimit, true);
        }

        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("0x00000da7f9cfca0b5a66eae610cb8c2e567f272a54c5ea7cf480c8df135967b3"));
        assert(genesis.hashMerkleRoot == uint256("0xcf8acb7ce43a07bc66a8b9daec515df36bd0d919bae147b13ea509131cc98bf5"));

        vFixedSeeds.clear();
        vSeeds.clear();
        vSeeds.push_back(CDNSSeedData("slingmarket.net", "testnet.seed.slingmarket.net"));

        base58Prefixes[PUBKEY_ADDRESS] = boost::assign::list_of(125).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[SCRIPT_ADDRESS] = boost::assign::list_of(196).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[SECRET_KEY]     = boost::assign::list_of(239).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();
        //  BIP44 coin type is from https://github.com/satoshilabs/slips/blob/master/slip-0044.md
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x04)(0x00)(0x00)(0x77).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_test, ARRAYLEN(pnSeed6_test));

        fRequireRPCPassword = true;
        fMiningRequiresPeers = true;
        fAllowMinDifficultyBlocks = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        nPoolMaxTransactions = 2;
        strSporkKey = "048216a20abab9c0edb9a813328bbc4dad2082dda77c4b990bccbade46a8a331928c3a165aa4bd8ef1d8e1b9c9660da46dd6371eed67f92ec711d2e0cbfb13b47e";
    }
    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataTestnet;
    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CTestNetParams
{
public:
    CRegTestParams()
    {
        networkID = CBaseChainParams::REGTEST;
        strNetworkID = "regtest";
        strNetworkID = "regtest";
        pchMessageStart[0] = 0x13;
        pchMessageStart[1] = 0x37;
        pchMessageStart[2] = 0x1f;
        pchMessageStart[3] = 0x31;
        nSubsidyHalvingInterval = 150;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 1;
        nTargetPoWSpacing = 3 * 60;  // Sling: 3 minutes
        nTargetPoSTimespan = 15 * 40;  // Sling: not sure?
        nTargetPoSSpacing = 35;   // Sling: 35 seconds
        bnProofOfWorkLimit = ~uint256(0) >> 1;
        nFirstRewardBlock = 10;
        genesis.nTime = 1454124731;
        genesis.nBits = 0x207fffff;
        genesis.nNonce = 12345;
        startNewChain = false;

        hashGenesisBlock = genesis.GetHash();
        nDefaultPort = 31137;
        //assert(hashGenesisBlock == uint256("0x")); TODO: Get new genesis
        //assert(genesis.hashMerkleRoot == uint256("0xcf8acb7ce43a07bc66a8b9daec515df36bd0d919bae147b13ea509131cc98bf5"));

        vFixedSeeds.clear(); //! Reg Testnet mode doesn't have any fixed seeds.
        vSeeds.clear();      //! Reg Testnet mode doesn't have any DNS seeds.

        fRequireRPCPassword = false;
        fMiningRequiresPeers = false;
        fAllowMinDifficultyBlocks = true;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;
    }
    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataRegtest;
    }
};
static CRegTestParams regTestParams;

/**
 * Unit test
 */
class CUnitTestParams : public CMainParams, public CModifiableParams
{
public:
    CUnitTestParams()
    {
        networkID = CBaseChainParams::UNITTEST;
        strNetworkID = "unittest";
        nDefaultPort = 31138;
        vFixedSeeds.clear(); //! Unit test mode doesn't have any fixed seeds.
        vSeeds.clear();      //! Unit test mode doesn't have any DNS seeds.

        fRequireRPCPassword = false;
        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fAllowMinDifficultyBlocks = false;
        fMineBlocksOnDemand = true;
        startNewChain = false;
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        // UnitTest share the same checkpoints as MAIN
        return data;
    }

    //! Published setters to allow changing values in unit test cases
    virtual void setSubsidyHalvingInterval(int anSubsidyHalvingInterval) { nSubsidyHalvingInterval = anSubsidyHalvingInterval; }
    virtual void setEnforceBlockUpgradeMajority(int anEnforceBlockUpgradeMajority) { nEnforceBlockUpgradeMajority = anEnforceBlockUpgradeMajority; }
    virtual void setRejectBlockOutdatedMajority(int anRejectBlockOutdatedMajority) { nRejectBlockOutdatedMajority = anRejectBlockOutdatedMajority; }
    virtual void setToCheckBlockUpgradeMajority(int anToCheckBlockUpgradeMajority) { nToCheckBlockUpgradeMajority = anToCheckBlockUpgradeMajority; }
    virtual void setDefaultConsistencyChecks(bool afDefaultConsistencyChecks) { fDefaultConsistencyChecks = afDefaultConsistencyChecks; }
    virtual void setAllowMinDifficultyBlocks(bool afAllowMinDifficultyBlocks) { fAllowMinDifficultyBlocks = afAllowMinDifficultyBlocks; }
    virtual void setSkipProofOfWorkCheck(bool afSkipProofOfWorkCheck) { fSkipProofOfWorkCheck = afSkipProofOfWorkCheck; }
};
static CUnitTestParams unitTestParams;


static CChainParams* pCurrentParams = 0;

CModifiableParams* ModifiableParams()
{
    assert(pCurrentParams);
    assert(pCurrentParams == &unitTestParams);
    return (CModifiableParams*)&unitTestParams;
}

const CChainParams& Params()
{
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(CBaseChainParams::Network network)
{
    switch (network) {
    case CBaseChainParams::MAIN:
        return mainParams;
    case CBaseChainParams::TESTNET:
        return testNetParams;
    case CBaseChainParams::REGTEST:
        return regTestParams;
    case CBaseChainParams::UNITTEST:
        return unitTestParams;
    default:
        assert(false && "Unimplemented network");
        return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectParams(network);
    return true;
}
