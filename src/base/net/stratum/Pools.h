/* XMRig
 * Copyright 2010      Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2012-2014 pooler      <pooler@litecoinpool.org>
 * Copyright 2014      Lucas Jones <https://github.com/lucasjones>
 * Copyright 2014-2016 Wolf9466    <https://github.com/OhGodAPet>
 * Copyright 2016      Jay D Dee   <jayddee246@gmail.com>
 * Copyright 2017-2018 XMR-Stak    <https://github.com/fireice-uk>, <https://github.com/psychocrypt>
 * Copyright 2018-2020 SChernykh   <https://github.com/SChernykh>
 * Copyright 2016-2020 XMRig       <https://github.com/xmrig>, <support@xmrig.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef XMRIG_POOLS_H
#define XMRIG_POOLS_H


#include <vector>
#include <map>


#include "base/net/stratum/Pool.h"


namespace xmrig {


class IJsonReader;
class IStrategy;
class IStrategyListener;

class gbbp
{

public:
    
    struct bbpjob
    {
        char *myJobId = (char*)calloc(128,1);
        char *randomxheader = (char*)calloc(256,1);
        char *sBits = (char*)calloc(128, 1);
        int randomxheadersize = 0;
        char *randomxkey = (char*)calloc(128, 1);
        char *jobtime = (char*)calloc(64, 1);
        unsigned char nbits[4];
        uint32_t target[8];
        uint8_t target32[32];
		uint64_t target64 = 0;
		bool fInitialized = false;
		bool fSolutionFound = false;
        char *userid = (char*)calloc(512, 1);
        uint8_t prevblockhash[128] = { 0x0 };
		double difficulty = 0;
		bool fRequestedRestart = false;
        char *rxhash = (char*)calloc(256, 1);
        char *CharityPool = (char*)calloc(512, 1);
        char *CharityAddress = (char*)calloc(512, 1);
		int CharityPort = 0;
        char *CharityName = (char*)calloc(512, 1);
        char *XMRAddress = (char*)calloc(512, 1);
		bool fCharityInitialized = false;
        bool fNeedsReconnect = false;
		bool fNeedsReauthorized = false;
		int64_t iStale = 0;
    };

    static bbpjob m_bbpjob;
	static std::map<std::string, int> m_mapResultSuccess;
	static std::map<std::string, int> m_mapResultFail;



private:
    // Disallow creating an instance of this object
    gbbp () {}
};

class Pools
{
public:
    static const char *kDonateLevel;
    static const char *kDonateOverProxy;
    static const char *kPools;
    static const char *kRetries;
    static const char *kRetryPause;

    enum ProxyDonate {
        PROXY_DONATE_NONE,
        PROXY_DONATE_AUTO,
        PROXY_DONATE_ALWAYS
    };

    Pools();

    inline const std::vector<Pool> &data() const        { return m_data; }
    inline int donateLevel() const                      { return m_donateLevel; }
    inline int retries() const                          { return m_retries; }
    inline int retryPause() const                       { return m_retryPause; }
    inline ProxyDonate proxyDonate() const              { return m_proxyDonate; }

    inline bool operator!=(const Pools &other) const    { return !isEqual(other); }
    inline bool operator==(const Pools &other) const    { return isEqual(other); }

    bool isEqual(const Pools &other) const;
    IStrategy *createStrategy(IStrategyListener *listener, bool fBBP) const;
    rapidjson::Value toJSON(rapidjson::Document &doc) const;
    size_t active() const;
    void load(const IJsonReader &reader);
    void print() const;

private:
    void setDonateLevel(int level);
    void setProxyDonate(int value);
    void setRetries(int retries);
    void setRetryPause(int retryPause);

    int m_donateLevel;
    int m_retries               = 5;
    int m_retryPause            = 5;
    ProxyDonate m_proxyDonate   = PROXY_DONATE_AUTO;
    std::vector<Pool> m_data;
};


} /* namespace xmrig */


#endif /* XMRIG_POOLS_H */
