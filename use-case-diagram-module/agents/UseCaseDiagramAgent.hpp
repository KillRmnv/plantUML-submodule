/*
 * This source file is part of an OSTIS project. For the latest info, see
 * http://ostis.net Distributed under the MIT License (See accompanying file
 * COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef C62835DC_2BAA_4144_A202_4763809FC3AA
#define C62835DC_2BAA_4144_A202_4763809FC3AA


#include <sc-memory/sc_agent.hpp>

class UseCaseDiagramAgent : public ScActionInitiatedAgent
{
public:
  UseCaseDiagramAgent();

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScActionInitiatedEvent const & event, ScAction & action) override;


};


#endif /* C62835DC_2BAA_4144_A202_4763809FC3AA */
