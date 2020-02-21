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

#ifndef XMRIG_BIBLEPAY_H
#define XMRIG_BIBLEPAY_H


#include <vector>
#include <map>
#include <mutex>

namespace xmrig {

	double FullTest3(uint8_t hash[]);
	double MathRound(double d);

	static std::mutex m_minermutex;
	struct l_bbpjob
	{
		uint8_t prevhash[64] = { 0x0 };
		uint8_t priorRandomXHeader[160] = { 0x0 };
		uint8_t out_bbphash[64] = { 0x0 };
		uint8_t out_rxhash[32] = { 0x0 };
		bool fSolved = false;
		double nDifficulty = 0;
		double nActualDifficulty = 0;
		bool fDebug = true;
	};


class gbbp
{

	

public:


	static void initbbp()
	{

		gbbp::bbpjob b;
		gbbp::m_bbpjob = b;

	}
    
    struct bbpjob
    {
		uint32_t target[8] = { 0x0 };
		uint8_t prevblockhash[128] = { 0x0 };
		bool fInitialized = false;
		bool fSolutionFound = false;
		int64_t nSubmitTime = 0;
		double difficulty = 0;
		double JobDifficulty = 0;
		double SolvedDifficulty = 0;
		bool fRequestedRestart = false;
		int CharityPort = 0;
		bool fCharityInitialized = false;
		bool fNeedsReconnect = false;
		int64_t iStale = 0;
    };

	static bbpjob m_bbpjob;
	static std::map<std::string, int> m_mapResultSuccess;
	static std::map<std::string, int> m_mapResultFail;
	static std::map<std::string, std::string> m_mapBBPJob;

private:
    // Disallow creating an instance of this object
    gbbp () {}
};




} /* namespace xmrig */


#endif /* XMRIG_BIBLEPAY_H */
