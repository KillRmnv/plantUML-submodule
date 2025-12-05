/*
 * This source file is part of an OSTIS project. For the latest info, see
 * http://ostis.net Distributed under the MIT License (See accompanying file
 * COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef BA77CABA_D11D_494C_B18E_675182E9A6D7
#define BA77CABA_D11D_494C_B18E_675182E9A6D7


#include <sc-memory/sc_agent.hpp>
#include <sc-memory/sc_agent_context.hpp>

class StateDiagramAgent : public ScActionInitiatedAgent
{
public:
  StateDiagramAgent();

  ScAddr GetActionClass() const override;

  ScResult DoProgram( ScAction & action) override;
  const ScAgentContext & GetContext() const {
    return m_context;
}

};


#endif /* BA77CABA_D11D_494C_B18E_675182E9A6D7 */
