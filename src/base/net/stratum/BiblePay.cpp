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

#include <math.h>

#include "base/net/stratum/BiblePay.h"
#include "base/tools/Buffer.h"

namespace xmrig {


	void phex(uint8_t data[], char *name)
	{
		char *d1 = (char*)malloc(160);
		xmrig::Buffer::toHex(reinterpret_cast<const char*>(data), 32, d1);
		printf(" H:%s Val:%s ", name, d1);
	}

	void phex(uint64_t data[], char* name)
	{
		char *d1 = (char*)malloc(160);
		xmrig::Buffer::toHex(reinterpret_cast<const char*>(data), 32, d1);
		printf(" H:%s Val:%s ", name, d1);
	}

	bool fulltest2(uint8_t hash[], uint8_t target[])
	{
		int i = 0;
		bool rc = true;

		for (i = 31; i >= 0; i--) {
			if (hash[i] > target[i]) {
				rc = false;
				break;
			}
			if (hash[i] < target[i]) {
				rc = true;
				break;
			}
		}

		return rc;
	}

	double FullTest3(uint8_t hash[])
	{
		// Converts the RandomX solution hash over to the original bitcoin difficulty level
		uint64_t nAdjHash = *reinterpret_cast<uint64_t*>(hash + 24);
		double nDiff = 0xFFFFFFFFFFFFULL / (nAdjHash + .01);
		return nDiff;
	}

	double MathRound(double d)
	{
		double r1 = ceilf(d * 100) / 100;
		return r1;
	}


} // namespace xmrig

