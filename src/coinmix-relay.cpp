
#include "coinmix-relay.h"


CCoinMixRelay::CCoinMixRelay()
{
    vinMasternode = CTxIn();
    nBlockHeight = 0;
    nRelayType = 0;
    in = CTxIn();
    out = CTxOut();
}

CCoinMixRelay::CCoinMixRelay(CTxIn& vinMasternodeIn, vector<unsigned char>& vchSigIn, int nBlockHeightIn, int nRelayTypeIn, CTxIn& in2, CTxOut& out2)
{
    vinMasternode = vinMasternodeIn;
    vchSig = vchSigIn;
    nBlockHeight = nBlockHeightIn;
    nRelayType = nRelayTypeIn;
    in = in2;
    out = out2;
}

std::string CCoinMixRelay::ToString()
{
    std::ostringstream info;

    info << "vin: " << vinMasternode.ToString() << " nBlockHeight: " << (int)nBlockHeight << " nRelayType: " << (int)nRelayType << " in " << in.ToString() << " out " << out.ToString();

    return info.str();
}

bool CCoinMixRelay::Sign(std::string strSharedKey)
{
    std::string strMessage = in.ToString() + out.ToString();

    CKey key2;
    CPubKey pubkey2;
    std::string errorMessage = "";

    if (!CoinMixSigner.SetKey(strSharedKey, errorMessage, key2, pubkey2)) {
        LogPrintf("CCoinMixRelay():Sign - ERROR: Invalid shared key: '%s'\n", errorMessage.c_str());
        return false;
    }

    if (!CoinMixSigner.SignMessage(strMessage, errorMessage, vchSig2, key2)) {
        LogPrintf("CCoinMixRelay():Sign - Sign message failed\n");
        return false;
    }

    if (!CoinMixSigner.VerifyMessage(pubkey2, vchSig2, strMessage, errorMessage)) {
        LogPrintf("CCoinMixRelay():Sign - Verify message failed\n");
        return false;
    }

    return true;
}

bool CCoinMixRelay::VerifyMessage(std::string strSharedKey)
{
    std::string strMessage = in.ToString() + out.ToString();

    CKey key2;
    CPubKey pubkey2;
    std::string errorMessage = "";

    if (!CoinMixSigner.SetKey(strSharedKey, errorMessage, key2, pubkey2)) {
        LogPrintf("CCoinMixRelay()::VerifyMessage - ERROR: Invalid shared key: '%s'\n", errorMessage.c_str());
        return false;
    }

    if (!CoinMixSigner.VerifyMessage(pubkey2, vchSig2, strMessage, errorMessage)) {
        LogPrintf("CCoinMixRelay()::VerifyMessage - Verify message failed\n");
        return false;
    }

    return true;
}

void CCoinMixRelay::Relay()
{
    int nCount = std::min(mnodeman.CountEnabled(ActiveProtocol()), 20);
    int nRank1 = (rand() % nCount) + 1;
    int nRank2 = (rand() % nCount) + 1;

    //keep picking another second number till we get one that doesn't match
    while (nRank1 == nRank2)
        nRank2 = (rand() % nCount) + 1;

    //printf("rank 1 - rank2 %d %d \n", nRank1, nRank2);

    //relay this message through 2 separate nodes for redundancy
    RelayThroughNode(nRank1);
    RelayThroughNode(nRank2);
}

void CCoinMixRelay::RelayThroughNode(int nRank)
{
    CMasternode* pmn = mnodeman.GetMasternodeByRank(nRank, nBlockHeight, ActiveProtocol());

    if (pmn != NULL) {
        //printf("RelayThroughNode %s\n", pmn->addr.ToString().c_str());
        CNode* pnode = ConnectNode((CAddress)pmn->addr, NULL, false);
        if (pnode) {
            //printf("Connected\n");
            pnode->PushMessage("dsr", (*this));
            pnode->Release();
            return;
        }
    } else {
        //printf("RelayThroughNode NULL\n");
    }
}
