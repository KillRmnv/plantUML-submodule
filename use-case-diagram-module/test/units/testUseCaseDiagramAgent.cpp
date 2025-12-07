/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

 #include <sc-memory/test/sc_test.hpp>
 #include <sc-builder/scs_loader.hpp>
 
 #include <sc-agents-common/utils/IteratorUtils.hpp>
 
//  #include "agents/UseCaseDiagramAgent.hpp"
 #include "../../agents/UseCaseDiagramAgent.hpp"
 #include "keynodes/Keynodes.hpp"
 #include "utils/NumberUtils.hpp"
 
 #include "utils/TestUtils.hpp"
 
 namespace UseCaseDiagramAgentTest
 {
 ScsLoader loader;
 std::string const USE_CASE_DIAGRAM_MODULE_TEST_FILES_DIR_PATH = "../test-structures/";
 int const WAIT_TIME = 5000;
 
 using UseCaseDiagramAgentTest = ScMemoryTest;
 




 void successfulUseCaseDiagramAgent(
     ScAgentContext & context,
     std::string const & fileWithGraphName,
     std::string const & expectedPlantUMLCode,
     std::string const & expectedBase64)
 {
   loader.loadScsFile(context, USE_CASE_DIAGRAM_MODULE_TEST_FILES_DIR_PATH + fileWithGraphName);
   loader.loadScsFile(context, USE_CASE_DIAGRAM_MODULE_TEST_FILES_DIR_PATH + "templates.scs");
   loader.loadScsFile(context, USE_CASE_DIAGRAM_MODULE_TEST_FILES_DIR_PATH + "testAction.scs");
   loader.loadScsFile(context, USE_CASE_DIAGRAM_MODULE_TEST_FILES_DIR_PATH + "checkTemplates.scs");
 
   ScAddr testActionNode = context.SearchElementBySystemIdentifier("test_action");
   ASSERT_TRUE(testActionNode.IsValid());
   ScAction testAction = context.ConvertToAction(testActionNode);
   ASSERT_TRUE(testAction.InitiateAndWait(WAIT_TIME));
   ASSERT_TRUE(testAction.IsFinishedSuccessfully());
 
   ScStructure const & result = testAction.GetResult();
  //TODO: from this point









 }
 
 void initialize(ScAgentContext & context)
 {
   context.SubscribeAgent<UseCaseDiagramAgent>();
 }
 
 void shutdown(ScAgentContext & context)
 {
   context.UnsubscribeAgent<UseCaseDiagramAgent>();
 }
 
 TEST_F(UseCaseDiagramAgentTest, graphWithOnePackage)
 {
   ScAgentContext & context = *m_ctx;
 
   initialize(context);
   successfulUseCaseDiagramAgent(context, "graphWithOnePackage.scs", "two_step_path_template", "");
   shutdown(context);
 }
 
 TEST_F(UseCaseDiagramAgentTest, graphWithInternalPackages)
 {
   ScAgentContext & context = *m_ctx;
 
   initialize(context);
   successfulUseCaseDiagramAgent(context, "graphWithThreeStepsShortestPath.scs", "three_step_path_template", "");
   shutdown(context);
 }
 TEST_F(UseCaseDiagramAgentTest, graphWithEdgesBetweenPackages)
 {
   ScAgentContext & context = *m_ctx;
 
   initialize(context);
   successfulUseCaseDiagramAgent(context, "graphWithThreeStepsShortestPath.scs", "three_step_path_template", "");
   shutdown(context);
 }
 }  // namespace UseCaseDiagramAgentTest
 