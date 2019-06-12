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

#include "random-strategy.hpp"

#include <random>

namespace nfd {
namespace fw {

NFD_REGISTER_STRATEGY(RandomStrategy);

RandomStrategy::RandomStrategy(Forwarder& forwarder, const Name& name)
  : Strategy(forwarder)
{
  ParsedInstanceName parsed = parseInstanceName(name);
  if (!parsed.parameters.empty()) {
    BOOST_THROW_EXCEPTION(std::invalid_argument("RandomStrategy does not accept parameters"));
  }
  if (parsed.version && *parsed.version != getStrategyName()[-1].toVersion()) {
    BOOST_THROW_EXCEPTION(std::invalid_argument(
      "RandomStrategy does not support version " + to_string(*parsed.version)));
  }
  this->setInstanceName(makeInstanceName(name, getStrategyName()));
}

const Name&
RandomStrategy::getStrategyName()
{
  static Name strategyName("/localhost/nfd/strategy/random/%FD%01");
  return strategyName;
}

void
RandomStrategy::afterReceiveInterest(const Face& inFace, const Interest& interest,
    const shared_ptr<pit::Entry>& pitEntry)
{
  const fib::Entry& fibEntry = this->lookupFib(*pitEntry);
  fib::NextHopList nhs = fibEntry.getNextHops();

  // Remove incoming nexthop from options:
  for (auto nh = nhs.begin(); nh != nhs.end(); nh++) {
    if (nh->getFace().getId() == inFace.getId()) {
      nhs.erase(nh);
      break;
    }
  }

  std::uniform_real_distribution<double> m_dist(0, 1);
  std::random_device rand_dev;
  std::mt19937 m_mt(rand_dev());

  const double rand {m_dist(m_mt)};
  assert(rand >= 0 && rand <= 1);
  double sumFwPerc = 0;

  // Randomized forwarding:
  for (const auto &nh : nhs) {
    sumFwPerc += 1.0 / nhs.size();

    if (rand <= sumFwPerc) {
      assert(sumFwPerc <= 1.0001);
      assert(inFace.getId() != nh.getFace().getId());
      this->sendInterest(pitEntry, nh.getFace(), interest);
      return;
    }
  }

  std::cerr << "Couldn't find any outgoing face!\n";
  this->rejectPendingInterest(pitEntry);
}

} // namespace fw
} // namespace nfd
