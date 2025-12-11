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
//  #include "agents/UseCaseDiagramAgent.hpp"
 #include "../../agents/UseCaseDiagramAgent.hpp"
 #include "keynodes/Keynodes.hpp"

 
 namespace UseCaseDiagramAgentTest
 {
 ScsLoader loader;
 std::string const USE_CASE_DIAGRAM_MODULE_TEST_FILES_DIR_PATH = "../test-structures/";
 int const WAIT_TIME = 5000;
 std::string const USE_CASE_MODULE_RESULT_FILES_DIR_PATH = "../results/";

 using UseCaseDiagramAgentTest = ScMemoryTest;
 

 std::string readFile(const std::string& path) {
  std::ifstream file(path);

  if (!file.is_open()) {
      throw std::runtime_error("Cannot open file: " + path);
  }

  std::string content(
      (std::istreambuf_iterator<char>(file)),
      std::istreambuf_iterator<char>()
  );

  return content;
}
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




 void successfulUseCaseDiagramAgent(
     ScAgentContext & context,
     std::string const & fileWithGraphName,
     std::string const & expectedPlantUMLCode,
     std::string const & expectedBase64)
 {
  GWFTranslator translator(context);

  std::string scsCode=translator.TranslateXMLFileContentToSCs(USE_CASE_DIAGRAM_MODULE_TEST_FILES_DIR_PATH+"some_name.gwf");
  saveStringToFile(USE_CASE_DIAGRAM_MODULE_TEST_FILES_DIR_PATH+"example.scs", scsCode);



   loader.loadScsFile(context, USE_CASE_DIAGRAM_MODULE_TEST_FILES_DIR_PATH + fileWithGraphName+".scs");
   loader.loadScsFile(context, USE_CASE_DIAGRAM_MODULE_TEST_FILES_DIR_PATH + "testAction.scs");

 
   ScAddr testActionNode = context.SearchElementBySystemIdentifier("test_action");
   ASSERT_TRUE(testActionNode.IsValid());
   ScAction testAction = context.ConvertToAction(testActionNode);
   ASSERT_TRUE(testAction.InitiateAndWait(WAIT_TIME));
   ASSERT_TRUE(testAction.IsFinishedSuccessfully());
 
   ScStructure const & result = testAction.GetResult();
  std::set<std::string> set;
  ScIterator3Ptr it3=context.CreateIterator3(result, ScType::PosArc, ScType::ConstNodeLink);
  while (it3->Next()) {
   std::string code;
     ASSERT_TRUE(context.GetLinkContent(it3->Get(2),code));
     set.insert(code);
     ASSERT_TRUE(it3->Get(2).IsValid());
  }   
 for(int i=0;i<set.size();i++)
  ASSERT_TRUE(set.find(readFile(USE_CASE_MODULE_RESULT_FILES_DIR_PATH+fileWithGraphName+std::to_string(i)))!=set.end());
 
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
 