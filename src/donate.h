/* XMRig
 * Copyright 2010      Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2012-2014 pooler      <pooler@litecoinpool.org>
 * Copyright 2014      Lucas Jones <https://github.com/lucasjones>
 * Copyright 2014-2016 Wolf9466    <https://github.com/OhGodAPet>
 * Copyright 2016      Jay D Dee   <jayddee246@gmail.com>
 * Copyright 2017-2018 XMR-Stak    <https://github.com/fireice-uk>, <https://github.com/psychocrypt>
 * Copyright 2016-2018 XMRig       <https://github.com/xmrig>, <support@xmrig.com>
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

#ifndef __DONATE_H__
#define __DONATE_H__


/*
 * Orphan-Charity donation for BIBLEPAY
 *
 * Example of how it works for the setting of 1%:
 * Your miner will contribute a percentage of hashpower to orphan charity.
 * NOTE: 100% of the funds will be directed to orphan-charity, with no overhead.
 * See https://accountability.biblepay.org for more information.
 * 
 * To make a donation to the lead developer of xmrig please send XMR here:
 * XMR: 48edfHu7V9Z84YzzMa6fUueoELZ9ZRXq9VetWzYGzKt52XU5xvqgzYnDK9URnRoJMk1j8nLwEVsaSWJ4fhdUyZijBGUicoD
 * or BTC here: 1P7ujsXeX7GxQwHNnJsRMgAdNkFZmNVqJT
 */
constexpr const int kDefaultDonateLevel = 0;
constexpr const int kMinimumDonateLevel = 1;


#endif /* __DONATE_H__ */
