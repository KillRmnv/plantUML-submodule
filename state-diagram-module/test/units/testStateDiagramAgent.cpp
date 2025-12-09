/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

 #include <sc-memory/test/sc_test.hpp>
 #include <sc-builder/scs_loader.hpp>
 #include <sc-agents-common/utils/IteratorUtils.hpp>
 #include "../../../utils/sc-builder-local/src/gwf_translator.hpp"
 #include "../../../utils/sc-builder-local/src/scs_translator.hpp"

//  #include "agents/StateDiagramAgent.hpp"
 #include "../../agents/StateDiagramAgent.hpp"
 #include "keynodes/Keynodes.hpp"
#include <sc-builder/translator.hpp>
#include <string>
  
 namespace StateDiagramAgentTest
 {
 ScsLoader loader;
 std::string const STATE_DIAGRAM_MODULE_TEST_FILES_DIR_PATH = "../test-structures/";
 int const WAIT_TIME = 5000;
 
 using StateDiagramAgentTest = ScMemoryTest;
 

 bool saveStringToFile(const std::string& fileName, const std::string& content)
 {
     std::ofstream outputFile(fileName);
 
     if (outputFile.is_open())
     {
         outputFile << content;
 
         outputFile.close();
         
         if (outputFile.fail())
         {
             std::cerr << "Error writing to file: " << fileName << std::endl;
             return false;
         }
 
         std::cout << "Successfully saved content to: " << fileName << std::endl;
         return true;
     }
     else
     {
         
         std::cerr << "Unable to open file for writing: " << fileName << std::endl;
         return false;
     }
 }


 void successfulStateDiagramAgentTest(
     ScAgentContext & context,
     std::string const & fileWithGraphName,
     std::string const & expectedPathTemplateIdtf,
     unsigned expectedPathLength)
 {
  GWFTranslator translator(context);
   std::string scsCode=translator.TranslateXMLFileContentToSCs(STATE_DIAGRAM_MODULE_TEST_FILES_DIR_PATH+"some_name.gwf");
    saveStringToFile(STATE_DIAGRAM_MODULE_TEST_FILES_DIR_PATH+"example.scs", scsCode);


   loader.loadScsFile(context, STATE_DIAGRAM_MODULE_TEST_FILES_DIR_PATH + fileWithGraphName);

 
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
   context.SubscribeAgent<StateDiagramAgent>();
 }
 
 void shutdown(ScAgentContext & context)
 {
   context.UnsubscribeAgent<StateDiagramAgent>();
 }
 
 TEST_F(StateDiagramAgentTest, graphWithOnePackage)
 {
   ScAgentContext & context = *m_ctx;
 
   initialize(context);
   successfulStateDiagramAgentTest(context, "graphWithOnePackage.scs", "two_step_path_template", 450);
   shutdown(context);
 }
 
 TEST_F(StateDiagramAgentTest, graphWithInternalPackages)
 {
   ScAgentContext & context = *m_ctx;
 
   initialize(context);
   successfulStateDiagramAgentTest(context, "graphWithThreeStepsShortestPath.scs", "three_step_path_template", 250);
   shutdown(context);
 }
 TEST_F(StateDiagramAgentTest, graphWithInternalPackagesAndNonAtomicArgument)
 {
   ScAgentContext & context = *m_ctx;
 
   initialize(context);
   successfulStateDiagramAgentTest(context, "graphWithThreeStepsShortestPath.scs", "three_step_path_template", 250);
   shutdown(context);
 }
 }  // namespace StateDiagramAgentTest
 