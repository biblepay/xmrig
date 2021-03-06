/* XMRig
 * Copyright 2010      Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2012-2014 pooler      <pooler@litecoinpool.org>
 * Copyright 2014      Lucas Jones <https://github.com/lucasjones>
 * Copyright 2014-2016 Wolf9466    <https://github.com/OhGodAPet>
 * Copyright 2016      Jay D Dee   <jayddee246@gmail.com>
 * Copyright 2017-2018 XMR-Stak    <https://github.com/fireice-uk>, <https://github.com/psychocrypt>
 * Copyright 2018      Lee Clagett <https://github.com/vtnerd>
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


#include <cassert>
#include <thread>
#include <mutex>
#include <map>
#include <cmath>

#include "backend/cpu/CpuWorker.h"
#include "core/Miner.h"
#include "crypto/cn/CnCtx.h"
#include "crypto/cn/CryptoNight_test.h"
#include "crypto/cn/CryptoNight.h"
#include "crypto/common/Nonce.h"
#include "crypto/common/VirtualMemory.h"
#include "crypto/rx/Rx.h"
#include "crypto/rx/RxVm.h"
#include "net/JobResults.h"
#include "base/net/stratum/BiblePay.h"

#ifdef XMRIG_ALGO_RANDOMX
#   include "crypto/randomx/randomx.h"
#endif


#ifdef XMRIG_ALGO_ASTROBWT
#   include "crypto/astrobwt/AstroBWT.h"
#endif


namespace xmrig {

static constexpr uint32_t kReserveCount = 32768;

xmrig::gbbp::bbpjob m_bbpjob;
std::map<std::string, int> xmrig::gbbp::m_mapResultSuccess;
std::map<std::string, int> xmrig::gbbp::m_mapResultFail;
std::map<std::string, std::string> xmrig::gbbp::m_mapBBPJob;

template<size_t N>
inline bool nextRound(WorkerJob<N> &job)
{
    if (!job.nextRound(kReserveCount, 1)) {
        JobResults::done(job.currentJob());

        return false;
    }

    return true;
}

} // namespace xmrig


template<size_t N>
xmrig::CpuWorker<N>::CpuWorker(size_t id, const CpuLaunchData &data) :
    Worker(id, data.affinity, data.priority),
    m_algorithm(data.algorithm),
    m_assembly(data.assembly),
    m_astrobwtAVX2(data.astrobwtAVX2),
    m_hwAES(data.hwAES),
    m_yield(data.yield),
    m_av(data.av()),
    m_astrobwtMaxSize(data.astrobwtMaxSize * 1000),
    m_miner(data.miner),
    m_ctx()
{
    m_memory = new VirtualMemory(m_algorithm.l3() * N, data.hugePages, false, true, m_node);
}


template<size_t N>
xmrig::CpuWorker<N>::~CpuWorker()
{
#   ifdef XMRIG_ALGO_RANDOMX
    delete m_vm;
#   endif

    CnCtx::release(m_ctx, N);
    delete m_memory;
}


#ifdef XMRIG_ALGO_RANDOMX
template<size_t N>
void xmrig::CpuWorker<N>::allocateRandomX_VM()
{
    RxDataset *dataset = Rx::dataset(m_job.currentJob(), m_node);

    while (dataset == nullptr) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        if (Nonce::sequence(Nonce::CPU) == 0) {
            return;
        }

        dataset = Rx::dataset(m_job.currentJob(), m_node);
    }

    if (!m_vm) {
        m_vm = new RxVm(dataset, m_memory->scratchpad(), !m_hwAES, m_assembly);
    }
}
#endif


template<size_t N>
bool xmrig::CpuWorker<N>::selfTest()
{
#   ifdef XMRIG_ALGO_RANDOMX
    if (m_algorithm.family() == Algorithm::RANDOM_X) {
        return N == 1;
    }
#   endif

    allocateCnCtx();

    if (m_algorithm.family() == Algorithm::CN) {
        const bool rc = verify(Algorithm::CN_0,      test_output_v0)   &&
                        verify(Algorithm::CN_1,      test_output_v1)   &&
                        verify(Algorithm::CN_2,      test_output_v2)   &&
                        verify(Algorithm::CN_FAST,   test_output_msr)  &&
                        verify(Algorithm::CN_XAO,    test_output_xao)  &&
                        verify(Algorithm::CN_RTO,    test_output_rto)  &&
                        verify(Algorithm::CN_HALF,   test_output_half) &&
                        verify2(Algorithm::CN_R,     test_output_r)    &&
                        verify(Algorithm::CN_RWZ,    test_output_rwz)  &&
                        verify(Algorithm::CN_ZLS,    test_output_zls)  &&
                        verify(Algorithm::CN_DOUBLE, test_output_double);

#       ifdef XMRIG_ALGO_CN_GPU
        if (!rc || N > 1) {
            return rc;
        }

        return verify(Algorithm::CN_GPU, test_output_gpu);
#       else
        return rc;
#       endif
    }

#   ifdef XMRIG_ALGO_CN_LITE
    if (m_algorithm.family() == Algorithm::CN_LITE) {
        return verify(Algorithm::CN_LITE_0,    test_output_v0_lite) &&
               verify(Algorithm::CN_LITE_1,    test_output_v1_lite);
    }
#   endif

#   ifdef XMRIG_ALGO_CN_HEAVY
    if (m_algorithm.family() == Algorithm::CN_HEAVY) {
        return verify(Algorithm::CN_HEAVY_0,    test_output_v0_heavy)  &&
               verify(Algorithm::CN_HEAVY_XHV,  test_output_xhv_heavy) &&
               verify(Algorithm::CN_HEAVY_TUBE, test_output_tube_heavy);
    }
#   endif

#   ifdef XMRIG_ALGO_CN_PICO
    if (m_algorithm.family() == Algorithm::CN_PICO) {
        return verify(Algorithm::CN_PICO_0, test_output_pico_trtl) &&
               verify(Algorithm::CN_PICO_TLO, test_output_pico_tlo);
    }
#   endif

#   ifdef XMRIG_ALGO_ARGON2
    if (m_algorithm.family() == Algorithm::ARGON2) {
        return verify(Algorithm::AR2_CHUKWA, argon2_chukwa_test_out) &&
               verify(Algorithm::AR2_WRKZ, argon2_wrkz_test_out);
    }
#   endif

#   ifdef XMRIG_ALGO_ASTROBWT
    if (m_algorithm.family() == Algorithm::ASTROBWT) {
        return verify(Algorithm::ASTROBWT_DERO, astrobwt_dero_test_out);
    }
#   endif

    return false;
}



template<size_t N>
void xmrig::CpuWorker<N>::start()
{
	l_bbpjob localbbpjob;

	while (Nonce::sequence(Nonce::CPU) > 0) {
        if (Nonce::isPaused()) {
            do {
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
            while (Nonce::isPaused() && Nonce::sequence(Nonce::CPU) > 0);

            if (Nonce::sequence(Nonce::CPU) == 0) {
                break;
            }

            consumeJob();
        }

        uint64_t storeStatsMask = 7;

#       ifdef XMRIG_ALGO_RANDOMX
        bool first = true;
        uint64_t tempHash[8] = {};
		
        // RandomX is faster, we don't need to store stats so often
        if (m_job.currentJob().algorithm().family() == Algorithm::RANDOM_X) {
            storeStatsMask = 63;
        }
#       endif

        memcpy(localbbpjob.prevhash, gbbp::m_bbpjob.prevblockhash, 32);
        localbbpjob.nDifficulty = gbbp::m_bbpjob.difficulty;

        while (!Nonce::isOutdated(Nonce::CPU, m_job.sequence())) {
            if ((m_count & storeStatsMask) == 0) {
                storeStats();
            }

            const Job &job = m_job.currentJob();
            if (job.algorithm().l3() != m_algorithm.l3()) {
                break;
            }

            uint32_t current_job_nonces[N];

            for (size_t i = 0; i < N; ++i) 
            {
                current_job_nonces[i] = *m_job.nonce(i);
            }

            bool valid = true;

#           ifdef XMRIG_ALGO_RANDOMX
            if (job.algorithm().family() == Algorithm::RANDOM_X) {
                if (first) {
                    first = false;
                    randomx_calculate_hash_first(m_vm->get(), tempHash, m_job.blob(), job.size());
                }

                if (m_count % (2000 + N) == 0)
                {
				    if (gbbp::m_bbpjob.fInitialized)
                    {
						int r1 = memcmp(localbbpjob.prevhash, gbbp::m_bbpjob.prevblockhash, 32);
						if (r1 != 0)
							memcpy(localbbpjob.prevhash, gbbp::m_bbpjob.prevblockhash, 32);
						if (gbbp::m_bbpjob.fInitialized == true && gbbp::m_bbpjob.fSolutionFound == false && localbbpjob.fSolved == true)
							localbbpjob.fSolved = false;
                    }
                }

                memcpy(localbbpjob.priorRandomXHeader, m_job.blob(), job.size());
                
				if (!nextRound(m_job)) {
					break;
				}
				// MINING LOOP
				randomx_calculate_hash_next_dual(m_vm->get(), localbbpjob.prevhash, localbbpjob.out_bbphash, tempHash, m_job.blob(), job.size(), m_hash);
				double nDiff1 = FullTest3(localbbpjob.out_bbphash);
				if ((!localbbpjob.fSolved && localbbpjob.nDifficulty > 0 && nDiff1 >= localbbpjob.nDifficulty))
				{
					if (gbbp::m_bbpjob.fInitialized)
					{
						// The randomx_calculate_hash_next_dual provides the solution to the *last* hash in the prior round, so here we have to glean results from the *priorRandomXHeader*
						auto memory1 = new VirtualMemory(job.algorithm().l3(), false, false, false);
						RxDataset *dataset1 = Rx::dataset(job, 0);
						if (dataset1 != nullptr) {
							auto vm1 = new RxVm(dataset1, memory1->scratchpad(), false, Assembly::NONE);
							randomx_calculate_dual_hash(vm1->get(), localbbpjob.prevhash, localbbpjob.out_bbphash, localbbpjob.priorRandomXHeader, job.size(), localbbpjob.out_rxhash);
							delete vm1;
						}
						delete memory1;
						// This RandomX hash has solved a biblepay-pool job!
						localbbpjob.fSolved = true;
						// Verify and gather information
						std::string seed = Buffer::toHex(job.seed().data(), 32).data();
						std::string rxhash = Buffer::toHex(localbbpjob.out_rxhash, 32).data();
						std::string bbphash = Buffer::toHex(localbbpjob.out_bbphash, 32).data();
						std::string lprevhash = Buffer::toHex(localbbpjob.prevhash, 32).data();
						std::string data = Buffer::toHex(localbbpjob.priorRandomXHeader, job.size()).data();
						JobResults::submitBBP(data, m_count, rxhash, bbphash, seed, localbbpjob.nDifficulty, MathRound(nDiff1));
					}
				}

				for (size_t i = 0; i < N; ++i) 
				{
					if (*reinterpret_cast<uint64_t*>(m_hash + (i * 32) + 24) < job.target())
					{
						// This dual-hash has solved a RandomX header
						double nDiff = FullTest3(m_hash);
						if (nDiff < 1) 
							nDiff = 1; 
						JobResults::submit(job, current_job_nonces[i], m_hash + (i * 32), MathRound(nDiff));
					}
				}
				
            }
            else
#           endif
            {
#               ifdef XMRIG_ALGO_ASTROBWT
                if (job.algorithm().family() == Algorithm::ASTROBWT) {
                    if (!astrobwt::astrobwt_dero(m_job.blob(), job.size(), m_ctx[0]->memory, m_hash, m_astrobwtMaxSize, m_astrobwtAVX2))
                        valid = false;
                }
                else
#               endif
                {
                    fn(job.algorithm())(m_job.blob(), job.size(), m_hash, m_ctx, job.height());
                }

                if (!nextRound(m_job)) {
                    break;
                };
            }

            if (false && valid) {
                for (size_t i = 0; i < N; ++i) {
                    if (*reinterpret_cast<uint64_t*>(m_hash + (i * 32) + 24) < job.target()) {
						double nDiff = FullTest3(m_hash);
                        JobResults::submit(job, current_job_nonces[i], m_hash + (i * 32), MathRound(nDiff));
                    }
                }
            }

			m_count += N;

            if (m_yield) {
                std::this_thread::yield();

            }
        }

        consumeJob();
    }
}


template<size_t N>
bool xmrig::CpuWorker<N>::verify(const Algorithm &algorithm, const uint8_t *referenceValue)
{
    cn_hash_fun func = fn(algorithm);
    if (!func) {
        return false;
    }

    func(test_input, 76, m_hash, m_ctx, 0);
    return memcmp(m_hash, referenceValue, sizeof m_hash) == 0;
}


template<size_t N>
bool xmrig::CpuWorker<N>::verify2(const Algorithm &algorithm, const uint8_t *referenceValue)
{
    cn_hash_fun func = fn(algorithm);
    if (!func) {
        return false;
    }

    for (size_t i = 0; i < (sizeof(cn_r_test_input) / sizeof(cn_r_test_input[0])); ++i) {
        const size_t size = cn_r_test_input[i].size;
        for (size_t k = 0; k < N; ++k) {
            memcpy(m_job.blob() + (k * size), cn_r_test_input[i].data, size);
        }

        func(m_job.blob(), size, m_hash, m_ctx, cn_r_test_input[i].height);

        for (size_t k = 0; k < N; ++k) {
            if (memcmp(m_hash + k * 32, referenceValue + i * 32, sizeof m_hash / N) != 0) {
                return false;
            }
        }
    }

    return true;
}


namespace xmrig {

template<>
bool CpuWorker<1>::verify2(const Algorithm &algorithm, const uint8_t *referenceValue)
{
    cn_hash_fun func = fn(algorithm);
    if (!func) {
        return false;
    }

    for (size_t i = 0; i < (sizeof(cn_r_test_input) / sizeof(cn_r_test_input[0])); ++i) {
        func(cn_r_test_input[i].data, cn_r_test_input[i].size, m_hash, m_ctx, cn_r_test_input[i].height);

        if (memcmp(m_hash, referenceValue + i * 32, sizeof m_hash) != 0) {
            return false;
        }
    }

    return true;
}

} // namespace xmrig


template<size_t N>
void xmrig::CpuWorker<N>::allocateCnCtx()
{
    if (m_ctx[0] == nullptr) {
        CnCtx::create(m_ctx, m_memory->scratchpad(), m_algorithm.l3(), N);
    }
}


template<size_t N>
void xmrig::CpuWorker<N>::consumeJob()
{
    if (Nonce::sequence(Nonce::CPU) == 0) {
        return;
    }

    m_job.add(m_miner->job(), kReserveCount, Nonce::CPU);

#   ifdef XMRIG_ALGO_RANDOMX
    if (m_job.currentJob().algorithm().family() == Algorithm::RANDOM_X) {
        allocateRandomX_VM();
    }
    else
#   endif
    {
        allocateCnCtx();
    }
}


namespace xmrig {

template class CpuWorker<1>;
template class CpuWorker<2>;
template class CpuWorker<3>;
template class CpuWorker<4>;
template class CpuWorker<5>;

} // namespace xmrig

