/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2019 Klaus Schneider, The University of Arizona
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Klaus Schneider <klaus@cs.arizona.edu>
 */

#ifndef NFD_DAEMON_FW_RANDOM_STRATEGY_HPP
#define NFD_DAEMON_FW_RANDOM_STRATEGY_HPP

#include "strategy.hpp"

namespace nfd {
namespace fw {

/** \brief Randomly chooses a nexthop
 *
 * Sends an incoming interest to a random outgoing face,
 * exluding the incoming face.
 *
 */
class RandomStrategy : public Strategy
{
public:
  explicit
  RandomStrategy(Forwarder& forwarder, const Name& name = getStrategyName());

  static const Name&
  getStrategyName();

  void
  afterReceiveInterest(const Face& inFace, const Interest& interest,
                       const shared_ptr<pit::Entry>& pitEntry) override;
};

} // namespace fw
} // namespace nfd

#endif // NFD_DAEMON_FW_RANDOM_STRATEGY_HPP
