/*
 * This source file is part of an OSTIS project. For the latest info, see
 * http://ostis.net Distributed under the MIT License (See accompanying file
 * COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef BF1713E5_2E50_420B_9DDC_4A4A55CBEE35
#define BF1713E5_2E50_420B_9DDC_4A4A55CBEE35


#include <sc-memory/sc_agent.hpp>

class ErDiagramAgent : public ScActionInitiatedAgent
{
public:
  ErDiagramAgent();

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScActionInitiatedEvent const & event, ScAction & action) override;


};


#endif /* BF1713E5_2E50_420B_9DDC_4A4A55CBEE35 */
