// Copyright (c) 2019, The Monero Project
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <stdint.h>
#include <vector>
#include "cryptonote_basic/cryptonote_basic.h"
#include "cryptonote_basic/cryptonote_format_utils.h"
#include "blockchain.h"
#include "tx_sanity_check.h"

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "verify"

namespace cryptonote
{

bool tx_sanity_check(const cryptonote::blobdata &tx_blob, uint64_t rct_outs_available, uint64_t current_height)
{
  cryptonote::transaction tx;

  if (!cryptonote::parse_and_validate_tx_from_blob(tx_blob, tx))
  {
    MERROR("Failed to parse transaction");
    return false;
  }

  if (cryptonote::is_coinbase(tx))
  {
    MERROR("Transaction is coinbase");
    return false;
  }
  
  std::set<uint64_t> rct_indices;
  size_t n_indices = 0;

  for (const auto &txin : tx.vin)
  {
    if (txin.type() == typeid(cryptonote::txin_to_key)) {
      const cryptonote::txin_to_key &in_to_key = boost::get<cryptonote::txin_to_key>(txin);
      if (in_to_key.amount != 0)
	      continue;
      const std::vector<uint64_t> absolute = cryptonote::relative_output_offsets_to_absolute(in_to_key.key_offsets);
      for (uint64_t offset: absolute)
	      rct_indices.insert(offset);
      n_indices += in_to_key.key_offsets.size();
    } else if (txin.type() == typeid(cryptonote::txin_offshore)) {
      const cryptonote::txin_offshore &in_to_key = boost::get<cryptonote::txin_offshore>(txin);
      if (in_to_key.amount != 0)
	      continue;
      const std::vector<uint64_t> absolute = cryptonote::relative_output_offsets_to_absolute(in_to_key.key_offsets);
      for (uint64_t offset: absolute)
	      rct_indices.insert(offset);
      n_indices += in_to_key.key_offsets.size();
    } else if (txin.type() == typeid(cryptonote::txin_onshore)) {
      const cryptonote::txin_onshore &in_to_key = boost::get<cryptonote::txin_onshore>(txin);
      if (in_to_key.amount != 0)
	      continue;
      const std::vector<uint64_t> absolute = cryptonote::relative_output_offsets_to_absolute(in_to_key.key_offsets);
      for (uint64_t offset: absolute)
	      rct_indices.insert(offset);
      n_indices += in_to_key.key_offsets.size();
    } else if (txin.type() == typeid(cryptonote::txin_xasset)) {
      const cryptonote::txin_xasset &in_to_key = boost::get<cryptonote::txin_xasset>(txin);
      if (in_to_key.amount != 0)
	      continue;
      const std::vector<uint64_t> absolute = cryptonote::relative_output_offsets_to_absolute(in_to_key.key_offsets);
      for (uint64_t offset: absolute)
	      rct_indices.insert(offset);
      n_indices += in_to_key.key_offsets.size();
    } else {
      continue;
    }
  }

  return tx_sanity_check(rct_indices, n_indices, rct_outs_available);
}

bool tx_sanity_check(const std::set<uint64_t> &rct_indices, size_t n_indices, uint64_t rct_outs_available)
{
  if (n_indices <= 10)
  {
    MDEBUG("n_indices is only " << n_indices << ", not checking");
    return true;
  }

  if (rct_outs_available < 10000)
    return true;
  /*
    This check is failing regularly for offshore/xasset txs that uses too many inputs.
    The reason for that is that, we still don't have enough outputs for non-xhv assets.
    When the tx has too many inputs it has to construct too many rings. Which increases the probability
    of 2 input using the same ring rember significantly giving that we don't have much outputs for those assets.
    Therefore, the number of unique indices are usually < 80% of the total usable outputs.
    We disable the check for now(to be actitaved later in the future), since there is no solution till we have enough outputs in the chain.
  */
  // if (rct_indices.size() < n_indices * 8 / 10)
  // {
  //   MERROR("amount of unique indices is too low (amount of rct indices is " << rct_indices.size() << ", out of total " << n_indices << "indices.");
  //   return false;
  // }

  std::vector<uint64_t> offsets(rct_indices.begin(), rct_indices.end());
  uint64_t median = epee::misc_utils::median(offsets);
  if (median < rct_outs_available * 6 / 10)
  {
    MERROR("median offset index is too low (median is " << median << " out of total " << rct_outs_available << "offsets). Transactions should contain a higher fraction of recent outputs.");
    return false;
  }

  return true;
}

}
